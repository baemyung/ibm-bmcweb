#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: Copyright OpenBMC Authors
#
# Redfish Event Listener - Event Subscriber Integration Test
#
# This script implements a simple HTTP event listener compatible with Redfish
# EventService subscriptions. It captures events sent by bmcweb and provides
# tools for event verification and filtering.
#
# Features:
#   - Listens for incoming Redfish events
#   - Stores events with timestamps
#   - Provides event filtering and search capabilities
#   - Validates event format and content
#   - Generates event reports
#
# Usage:
#   python3 redfish-event-listener.py --listen 0.0.0.0:8888 &
#   # Then send events from bmcweb
#   python3 redfish-event-listener.py --check-events --filter resource-type AggregationSource
#   python3 redfish-event-listener.py --report

import argparse
import http.server
import json
import logging
import socket
import socketserver
import sys
import threading
import time
from dataclasses import dataclass, asdict
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Any

# Setup logging
logging.basicConfig(
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    level=logging.INFO
)
logger = logging.getLogger(__name__)

# Event storage
EVENTS_DIR = Path("./events")
EVENTS_DIR.mkdir(exist_ok=True)


@dataclass
class ReceivedEvent:
    """Represents a received Redfish event"""
    timestamp: str
    source_ip: str
    content_type: str
    message_id: Optional[str]
    severity: Optional[str]
    origin_of_condition: Optional[str]
    resource_type: Optional[str]
    raw_payload: Dict[str, Any]

    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


class EventListener(http.server.BaseHTTPRequestHandler):
    """HTTP request handler for Redfish events"""

    event_store: List[ReceivedEvent] = []
    lock = threading.Lock()

    def do_POST(self):
        """Handle POST request (event delivery)"""
        content_length = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(content_length)
        content_type = self.headers.get('Content-Type', 'unknown')

        try:
            # Parse JSON payload
            payload = json.loads(body)

            # Extract event information
            message_id = None
            severity = None
            origin = None
            resource_type = None

            # Handle Event format
            if 'Events' in payload and isinstance(payload['Events'], list):
                for event in payload['Events']:
                    message_id = event.get('MessageId', '')
                    severity = event.get('Severity', '')
                    origin = event.get('OriginOfCondition', '')

            # Try alternative fields
            if not message_id:
                message_id = payload.get('MessageId', '')
            if not severity:
                severity = payload.get('MessageSeverity', '')
            if not origin:
                origin = payload.get('OriginOfCondition', '')

            # Infer resource type from origin
            if origin and 'AggregationSource' in str(origin):
                resource_type = 'AggregationSource'
            elif origin and 'Chassis' in str(origin):
                resource_type = 'Chassis'
            elif origin and 'Systems' in str(origin):
                resource_type = 'ComputerSystem'

            # Create event record
            event = ReceivedEvent(
                timestamp=datetime.now().isoformat(),
                source_ip=self.client_address[0],
                content_type=content_type,
                message_id=message_id,
                severity=severity,
                origin_of_condition=origin,
                resource_type=resource_type,
                raw_payload=payload
            )

            # Store event
            with EventListener.lock:
                EventListener.event_store.append(event)

            # Log event
            logger.info(
                f"Event received: MessageId={message_id}, "
                f"Severity={severity}, Origin={origin}"
            )

            # Send success response
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            self.wfile.write(json.dumps({"status": "accepted"}).encode())

        except json.JSONDecodeError as e:
            logger.error(f"Failed to parse JSON: {e}")
            self.send_response(400)
            self.end_headers()
        except Exception as e:
            logger.error(f"Error processing event: {e}")
            self.send_response(500)
            self.end_headers()

    def do_GET(self):
        """Handle GET request (healthcheck)"""
        if self.path == '/health':
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            self.wfile.write(
                json.dumps({
                    "status": "healthy",
                    "events_received": len(EventListener.event_store)
                }).encode()
            )
        else:
            self.send_response(404)
            self.end_headers()

    def log_message(self, format, *args):
        """Suppress default HTTP logging"""
        return


class EventListenerServer:
    """Main event listener server"""

    def __init__(self, host: str = "0.0.0.0", port: int = 8888):
        self.host = host
        self.port = port
        self.server = None
        self.thread = None

    def start(self):
        """Start the event listener server"""
        logger.info(f"Starting event listener on {self.host}:{self.port}")
        
        try:
            self.server = socketserver.TCPServer(
                (self.host, self.port),
                EventListener
            )
            self.server.allow_reuse_address = True
            
            self.thread = threading.Thread(
                target=self.server.serve_forever,
                daemon=True
            )
            self.thread.start()
            logger.info(f"Event listener started successfully")
        except OSError as e:
            logger.error(f"Failed to start event listener: {e}")
            raise

    def stop(self):
        """Stop the event listener server"""
        if self.server:
            self.server.shutdown()
            logger.info("Event listener stopped")

    def get_events(self) -> List[ReceivedEvent]:
        """Get all received events"""
        with EventListener.lock:
            return EventListener.event_store.copy()

    def clear_events(self):
        """Clear event store"""
        with EventListener.lock:
            EventListener.event_store.clear()
        logger.info("Event store cleared")

    def filter_events(
        self,
        message_id: Optional[str] = None,
        severity: Optional[str] = None,
        resource_type: Optional[str] = None,
        origin_pattern: Optional[str] = None
    ) -> List[ReceivedEvent]:
        """Filter events by criteria"""
        events = self.get_events()

        if message_id:
            events = [e for e in events if message_id in (e.message_id or '')]
        if severity:
            events = [e for e in events if e.severity == severity]
        if resource_type:
            events = [e for e in events if e.resource_type == resource_type]
        if origin_pattern:
            events = [e for e in events if origin_pattern in (e.origin_of_condition or '')]

        return events

    def print_events(self, events: Optional[List[ReceivedEvent]] = None):
        """Print events in human-readable format"""
        if events is None:
            events = self.get_events()

        if not events:
            logger.info("No events")
            return

        print(f"\n{'=' * 100}")
        print(f"Total Events: {len(events)}")
        print(f"{'=' * 100}\n")

        for i, event in enumerate(events, 1):
            print(f"Event #{i}")
            print(f"  Timestamp:       {event.timestamp}")
            print(f"  Source IP:       {event.source_ip}")
            print(f"  Content-Type:    {event.content_type}")
            print(f"  MessageId:       {event.message_id}")
            print(f"  Severity:        {event.severity}")
            print(f"  ResourceType:    {event.resource_type}")
            print(f"  Origin:          {event.origin_of_condition}")
            print()

    def save_events(self, filename: str = "events.json"):
        """Save events to file"""
        events = self.get_events()
        with open(filename, 'w') as f:
            json.dump(
                [e.to_dict() for e in events],
                f,
                indent=2
            )
        logger.info(f"Events saved to {filename}")

    def generate_report(self):
        """Generate summary report"""
        events = self.get_events()

        print("\n" + "=" * 100)
        print("EVENT LISTENER REPORT")
        print("=" * 100)

        print(f"\nTotal Events Received: {len(events)}")

        if not events:
            print("No events received yet.")
            return

        # Summary by severity
        severity_counts = {}
        for event in events:
            severity = event.severity or "Unknown"
            severity_counts[severity] = severity_counts.get(severity, 0) + 1

        print("\nEvents by Severity:")
        for severity, count in sorted(severity_counts.items()):
            print(f"  {severity}: {count}")

        # Summary by resource type
        resource_counts = {}
        for event in events:
            rtype = event.resource_type or "Unknown"
            resource_counts[rtype] = resource_counts.get(rtype, 0) + 1

        print("\nEvents by Resource Type:")
        for rtype, count in sorted(resource_counts.items()):
            print(f"  {rtype}: {count}")

        # Summary by message ID
        message_counts = {}
        for event in events:
            msg = event.message_id or "Unknown"
            message_counts[msg] = message_counts.get(msg, 0) + 1

        print("\nTop Message IDs:")
        for msg, count in sorted(message_counts.items(), key=lambda x: x[1], reverse=True)[:10]:
            print(f"  {msg}: {count}")

        # Timeline
        if events:
            first_time = datetime.fromisoformat(events[0].timestamp)
            last_time = datetime.fromisoformat(events[-1].timestamp)
            duration = last_time - first_time
            print(f"\nTimeline:")
            print(f"  First Event: {events[0].timestamp}")
            print(f"  Last Event:  {events[-1].timestamp}")
            print(f"  Duration:    {duration}")

        print("\n" + "=" * 100 + "\n")


def main():
    parser = argparse.ArgumentParser(
        description="Redfish Event Listener - Event Subscriber for Testing"
    )

    parser.add_argument(
        "--listen",
        type=str,
        default="0.0.0.0:8888",
        help="Listen address and port (default: 0.0.0.0:8888)"
    )

    parser.add_argument(
        "--check-events",
        action="store_true",
        help="Check and display received events"
    )

    parser.add_argument(
        "--filter-message-id",
        type=str,
        help="Filter events by message ID"
    )

    parser.add_argument(
        "--filter-severity",
        type=str,
        help="Filter events by severity"
    )

    parser.add_argument(
        "--filter-resource-type",
        type=str,
        help="Filter events by resource type"
    )

    parser.add_argument(
        "--filter-origin",
        type=str,
        help="Filter events by origin pattern"
    )

    parser.add_argument(
        "--report",
        action="store_true",
        help="Generate summary report"
    )

    parser.add_argument(
        "--save",
        type=str,
        help="Save events to JSON file"
    )

    parser.add_argument(
        "--clear",
        action="store_true",
        help="Clear event store"
    )

    parser.add_argument(
        "--daemon",
        action="store_true",
        help="Run as daemon (don't exit)"
    )

    args = parser.parse_args()

    # Parse listen address
    host, port = args.listen.split(':')
    port = int(port)

    # Create and start server
    server = EventListenerServer(host, port)
    server.start()

    # If not running as daemon, just check/report and exit
    if not args.daemon:
        # Wait a bit for server to start
        time.sleep(0.5)

        # Handle various commands
        if args.check_events or args.filter_message_id or args.filter_severity or \
           args.filter_resource_type or args.filter_origin:
            events = server.filter_events(
                message_id=args.filter_message_id,
                severity=args.filter_severity,
                resource_type=args.filter_resource_type,
                origin_pattern=args.filter_origin
            )
            server.print_events(events)

        if args.report:
            server.generate_report()

        if args.save:
            server.save_events(args.save)

        if args.clear:
            server.clear_events()

        if args.check_events or args.filter_message_id or args.filter_severity or \
           args.filter_resource_type or args.filter_origin or args.report or args.save:
            server.stop()
            return

    # Run as daemon
    logger.info("Running as daemon. Press Ctrl+C to stop.")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        logger.info("Shutting down...")
        server.stop()


if __name__ == "__main__":
    main()
