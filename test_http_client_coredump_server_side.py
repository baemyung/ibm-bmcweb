#!/usr/bin/env python3
"""
HTTP Client Coredump Reproduction Script - Server Side Testing

This script reproduces the coredump issue in BMCWeb's HTTP CLIENT code
(not the server code). The issue occurs when BMCWeb makes OUTBOUND HTTP
connections (e.g., for event subscriptions, aggregation) and those connections
are rapidly destroyed while async operations are pending.

The key is to:
1. Make BMCWeb create outbound HTTP client connections
2. Trigger rapid destruction of those connections
3. This happens when event subscriptions are created/deleted rapidly

Usage:
    python3 test_http_client_coredump_server_side.py --host <bmcweb_host> --port <port>
    
Example:
    python3 test_http_client_coredump_server_side.py --host 192.168.1.100 --port 443
"""

import argparse
import requests
import threading
import time
import sys
import urllib3
from http.server import HTTPServer, BaseHTTPRequestHandler
from concurrent.futures import ThreadPoolExecutor
import socket

# Suppress SSL warnings for testing
urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)


class EventReceiver(BaseHTTPRequestHandler):
    """Simple HTTP server to receive events from BMCWeb"""
    
    def do_POST(self):
        """Handle POST requests from BMCWeb event subscriptions"""
        content_length = int(self.headers.get('Content-Length', 0))
        body = self.rfile.read(content_length)
        
        # Respond quickly
        self.send_response(200)
        self.send_header('Content-Type', 'text/plain')
        self.end_headers()
        self.wfile.write(b'OK')
    
    def log_message(self, format, *args):
        """Suppress log messages"""
        pass


class CoredumpReproducer:
    def __init__(self, host, port, use_ssl=True, username='root', password='0penBmc'):
        self.host = host
        self.port = port
        self.protocol = "https" if use_ssl else "http"
        self.base_url = f"{self.protocol}://{host}:{port}"
        self.auth = (username, password)
        self.event_server = None
        self.event_server_thread = None
        self.event_port = 8888
        
    def start_event_receiver(self):
        """Start a local HTTP server to receive events"""
        try:
            self.event_server = HTTPServer(('0.0.0.0', self.event_port), EventReceiver)
            self.event_server_thread = threading.Thread(
                target=self.event_server.serve_forever,
                daemon=True
            )
            self.event_server_thread.start()
            print(f"✓ Event receiver started on port {self.event_port}")
            return True
        except Exception as e:
            print(f"✗ Failed to start event receiver: {e}")
            return False
    
    def stop_event_receiver(self):
        """Stop the event receiver"""
        if self.event_server:
            self.event_server.shutdown()
            self.event_server = None
    
    def get_local_ip(self):
        """Get local IP address for event destination"""
        try:
            # Create a socket to determine local IP
            s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            s.connect((self.host, self.port))
            local_ip = s.getsockname()[0]
            s.close()
            return local_ip
        except Exception:
            return '127.0.0.1'
    
    def create_subscription(self, context, destination=None):
        """Create an event subscription that triggers BMCWeb HTTP client"""
        if destination is None:
            local_ip = self.get_local_ip()
            destination = f'http://{local_ip}:{self.event_port}/events'
        
        session = requests.Session()
        session.verify = False
        
        try:
            response = session.post(
                f"{self.base_url}/redfish/v1/EventService/Subscriptions",
                json={
                    'Destination': destination,
                    'Protocol': 'Redfish',
                    'Context': context,
                    'SubscriptionType': 'RedfishEvent'
                },
                timeout=5,
                auth=self.auth
            )
            
            if response.status_code == 201:
                location = response.headers.get('Location', '')
                return location
            else:
                return None
                
        except Exception as e:
            return None
        finally:
            session.close()
    
    def delete_subscription(self, location):
        """Delete an event subscription"""
        if not location:
            return False
            
        session = requests.Session()
        session.verify = False
        
        try:
            response = session.delete(
                f"{self.base_url}{location}",
                timeout=2,
                auth=self.auth
            )
            return response.status_code in [200, 204]
        except Exception:
            return False
        finally:
            session.close()
    
    def test_rapid_subscription_cycling(self, iterations=100):
        """
        Test 1: Rapid Event Subscription Creation/Deletion
        
        This triggers BMCWeb's HTTP client to:
        1. Create outbound connections for event delivery
        2. Rapidly destroy those connections when subscriptions are deleted
        3. Race conditions occur when async operations are pending
        """
        print(f"\n[TEST 1] Rapid Event Subscription Cycling ({iterations} iterations)")
        print("=" * 70)
        print("This test creates and immediately deletes event subscriptions,")
        print("triggering rapid creation/destruction of BMCWeb's HTTP client connections.")
        print()
        
        created = 0
        deleted = 0
        
        for i in range(iterations):
            # Create subscription (triggers HTTP client connection)
            location = self.create_subscription(f'test-{i}')
            
            if location:
                created += 1
                # Immediately delete (triggers connection destruction)
                time.sleep(0.001)  # 1ms - very short window
                if self.delete_subscription(location):
                    deleted += 1
            
            if (i + 1) % 10 == 0:
                print(f"  Progress: {i + 1}/{iterations} - Created: {created}, Deleted: {deleted}")
        
        print(f"\n  Final: Created {created}, Deleted {deleted} subscriptions")
        print("✓ Test 1 complete")
    
    def test_concurrent_subscription_stress(self, num_concurrent=20):
        """
        Test 2: Concurrent Subscription Operations
        
        Creates multiple subscriptions concurrently, then deletes them all
        at once, maximizing the chance of race conditions.
        """
        print(f"\n[TEST 2] Concurrent Subscription Stress ({num_concurrent} concurrent)")
        print("=" * 70)
        print("Creating multiple subscriptions concurrently, then deleting all at once.")
        print()
        
        locations = []
        
        def create_sub(index):
            loc = self.create_subscription(f'concurrent-{index}')
            if loc:
                locations.append(loc)
        
        # Create subscriptions concurrently
        print(f"  Creating {num_concurrent} subscriptions...")
        with ThreadPoolExecutor(max_workers=10) as executor:
            futures = [executor.submit(create_sub, i) for i in range(num_concurrent)]
            for future in futures:
                future.result()
        
        print(f"  Created {len(locations)} subscriptions")
        
        # Wait briefly for connections to be established
        time.sleep(0.1)
        
        # Delete all subscriptions concurrently
        print(f"  Deleting all {len(locations)} subscriptions concurrently...")
        
        def delete_sub(loc):
            self.delete_subscription(loc)
        
        with ThreadPoolExecutor(max_workers=10) as executor:
            futures = [executor.submit(delete_sub, loc) for loc in locations]
            for future in futures:
                future.result()
        
        print("✓ Test 2 complete")
    
    def test_unreachable_destination(self, iterations=50):
        """
        Test 3: Subscriptions to Unreachable Destinations
        
        Creates subscriptions to unreachable IPs, causing connection timeouts
        and failures while async operations are pending.
        """
        print(f"\n[TEST 3] Unreachable Destination Test ({iterations} iterations)")
        print("=" * 70)
        print("Creating subscriptions to unreachable destinations to trigger")
        print("connection failures and timeouts in BMCWeb's HTTP client.")
        print()
        
        locations = []
        
        for i in range(iterations):
            # Use non-routable IP addresses (RFC 5737 TEST-NET-1)
            unreachable_ip = f'192.0.2.{(i % 254) + 1}'
            location = self.create_subscription(
                f'unreachable-{i}',
                destination=f'http://{unreachable_ip}:8080/events'
            )
            
            if location:
                locations.append(location)
            
            # Very short delay to maximize race conditions
            time.sleep(0.01)
            
            if (i + 1) % 10 == 0:
                print(f"  Created {i + 1}/{iterations} subscriptions to unreachable hosts")
        
        print(f"\n  Created {len(locations)} subscriptions")
        print("  Waiting 2 seconds for connection attempts...")
        time.sleep(2)
        
        # Now delete them all rapidly
        print("  Deleting all subscriptions rapidly...")
        for loc in locations:
            self.delete_subscription(loc)
            time.sleep(0.001)  # 1ms between deletions
        
        print("✓ Test 3 complete")
    
    def test_subscription_churn(self, duration_seconds=30):
        """
        Test 4: Continuous Subscription Churn
        
        Continuously creates and deletes subscriptions for a period of time,
        maintaining constant pressure on the HTTP client connection pool.
        """
        print(f"\n[TEST 4] Continuous Subscription Churn ({duration_seconds}s)")
        print("=" * 70)
        print("Continuously creating and deleting subscriptions to maintain")
        print("constant pressure on BMCWeb's HTTP client connection pool.")
        print()
        
        start_time = time.time()
        created = 0
        deleted = 0
        active_subscriptions = []
        
        while time.time() - start_time < duration_seconds:
            # Create a subscription
            location = self.create_subscription(f'churn-{created}')
            if location:
                created += 1
                active_subscriptions.append(location)
            
            # If we have more than 5 active, delete the oldest
            if len(active_subscriptions) > 5:
                old_location = active_subscriptions.pop(0)
                if self.delete_subscription(old_location):
                    deleted += 1
            
            # Very short delay
            time.sleep(0.05)
            
            # Progress update every 5 seconds
            elapsed = time.time() - start_time
            if int(elapsed) % 5 == 0 and elapsed > 0:
                print(f"  {int(elapsed)}s: Created {created}, Deleted {deleted}, Active {len(active_subscriptions)}")
                time.sleep(1)  # Avoid duplicate prints
        
        # Cleanup remaining subscriptions
        print("\n  Cleaning up remaining subscriptions...")
        for loc in active_subscriptions:
            if self.delete_subscription(loc):
                deleted += 1
        
        print(f"\n  Final: Created {created}, Deleted {deleted}")
        print("✓ Test 4 complete")
    
    def run_all_tests(self):
        """Run all reproduction tests"""
        print("\n" + "=" * 70)
        print("BMCWeb HTTP Client Coredump Reproduction Tests")
        print(f"Target: {self.base_url}")
        print("=" * 70)
        print("\nThese tests trigger BMCWeb's OUTBOUND HTTP client connections")
        print("(not inbound server connections) to reproduce the coredump issue.")
        print()
        
        # Start event receiver
        if not self.start_event_receiver():
            print("\n✗ Cannot proceed without event receiver")
            return
        
        try:
            self.test_rapid_subscription_cycling(iterations=100)
            time.sleep(2)
            
            self.test_concurrent_subscription_stress(num_concurrent=20)
            time.sleep(2)
            
            self.test_unreachable_destination(iterations=50)
            time.sleep(2)
            
            self.test_subscription_churn(duration_seconds=30)
            
            print("\n" + "=" * 70)
            print("All tests completed!")
            print("\nCheck BMCWeb for coredumps:")
            print("  - dmesg | grep segfault")
            print("  - journalctl -u bmcweb | grep -i 'segmentation\\|core'")
            print("  - ls -la /var/lib/systemd/coredump/")
            print("=" * 70)
            
        except KeyboardInterrupt:
            print("\n\nTests interrupted by user")
        finally:
            self.stop_event_receiver()


def main():
    parser = argparse.ArgumentParser(
        description='Reproduce HTTP Client coredump issues in BMCWeb (server-side)'
    )
    parser.add_argument(
        '--host',
        required=True,
        help='BMCWeb host address (e.g., 192.168.1.100)'
    )
    parser.add_argument(
        '--port',
        type=int,
        default=443,
        help='BMCWeb port (default: 443)'
    )
    parser.add_argument(
        '--username',
        default='root',
        help='BMCWeb username (default: root)'
    )
    parser.add_argument(
        '--password',
        default='0penBmc',
        help='BMCWeb password (default: 0penBmc)'
    )
    parser.add_argument(
        '--no-ssl',
        action='store_true',
        help='Use HTTP instead of HTTPS'
    )
    parser.add_argument(
        '--test',
        type=int,
        choices=[1, 2, 3, 4],
        help='Run specific test only (1-4)'
    )
    parser.add_argument(
        '--event-port',
        type=int,
        default=8888,
        help='Local port for event receiver (default: 8888)'
    )
    
    args = parser.parse_args()
    
    reproducer = CoredumpReproducer(
        host=args.host,
        port=args.port,
        use_ssl=not args.no_ssl,
        username=args.username,
        password=args.password
    )
    reproducer.event_port = args.event_port
    
    if args.test:
        # Start event receiver
        if not reproducer.start_event_receiver():
            print("\n✗ Cannot proceed without event receiver")
            return
        
        try:
            # Run specific test
            test_methods = {
                1: lambda: reproducer.test_rapid_subscription_cycling(iterations=100),
                2: lambda: reproducer.test_concurrent_subscription_stress(num_concurrent=20),
                3: lambda: reproducer.test_unreachable_destination(iterations=50),
                4: lambda: reproducer.test_subscription_churn(duration_seconds=30)
            }
            test_methods[args.test]()
        finally:
            reproducer.stop_event_receiver()
    else:
        # Run all tests
        reproducer.run_all_tests()


if __name__ == '__main__':
    main()

# Made with Bob
