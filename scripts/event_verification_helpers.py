#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: Copyright OpenBMC Authors
#
# Event Verification Helpers
#
# This module provides utilities for validating and verifying Redfish events
# in test scenarios. It includes:
#   - Event schema validation
#   - Event content verification
#   - Test assertions and matchers
#   - Event comparison utilities

import json
import re
from dataclasses import dataclass
from typing import Any, Dict, List, Optional, Pattern


@dataclass
class EventExpectation:
    """Defines expected properties of an event"""
    message_id_pattern: Optional[Pattern] = None
    severity: Optional[str] = None
    resource_type: Optional[str] = None
    origin_pattern: Optional[Pattern] = None
    message_args_count: Optional[int] = None
    must_have_fields: List[str] = None

    def __post_init__(self):
        if self.must_have_fields is None:
            self.must_have_fields = []


class EventValidator:
    """Validates event structure and content"""

    @staticmethod
    def validate_event_structure(event: Dict[str, Any]) -> List[str]:
        """
        Validate that event has proper Redfish Event structure.
        
        Returns:
            List of validation errors (empty if valid)
        """
        errors = []

        # Check required Event fields
        required_fields = ['@odata.type', 'Name', 'Id', 'Events']
        for field in required_fields:
            if field not in event:
                errors.append(f"Missing required field: {field}")

        # Validate @odata.type
        if '@odata.type' in event:
            odata_type = event['@odata.type']
            if 'Event' not in odata_type:
                errors.append(f"Invalid @odata.type: {odata_type}")

        # Validate Events array
        if 'Events' in event:
            if not isinstance(event['Events'], list):
                errors.append("Events must be an array")
            elif len(event['Events']) == 0:
                errors.append("Events array is empty")
            else:
                for i, member in enumerate(event['Events']):
                    event_errors = EventValidator.validate_event_member(member)
                    for err in event_errors:
                        errors.append(f"Event[{i}]: {err}")

        return errors

    @staticmethod
    def validate_event_member(event_member: Dict[str, Any]) -> List[str]:
        """
        Validate individual event member structure.
        
        Returns:
            List of validation errors (empty if valid)
        """
        errors = []

        # Required event member fields
        required_fields = ['MessageId', 'MemberId']
        for field in required_fields:
            if field not in event_member:
                errors.append(f"Missing required field: {field}")

        # Validate MessageId format
        if 'MessageId' in event_member:
            msg_id = event_member['MessageId']
            if not isinstance(msg_id, str) or '.' not in msg_id:
                errors.append(
                    f"Invalid MessageId format: {msg_id} "
                    "(should be 'Registry.Major.Minor.MessageName')"
                )

        # Validate Severity if present
        if 'Severity' in event_member:
            severity = event_member['Severity']
            valid_severities = ['OK', 'Warning', 'Critical']
            if severity not in valid_severities:
                errors.append(
                    f"Invalid Severity: {severity}. "
                    f"Must be one of: {', '.join(valid_severities)}"
                )

        return errors

    @staticmethod
    def validate_resource_event(event_member: Dict[str, Any]) -> List[str]:
        """
        Validate ResourceEvent-specific message structure.
        
        Returns:
            List of validation errors (empty if valid)
        """
        errors = []

        if 'MessageId' not in event_member:
            errors.append("ResourceEvent member missing MessageId")
            return errors

        msg_id = event_member['MessageId']

        # ResourceEvent messages should follow specific patterns
        resource_event_types = [
            'ResourceCreated',
            'ResourceChanged',
            'ResourceRemoved',
            'ResourceStatusChangedOK',
            'ResourceStatusChangedWarning',
            'ResourceStatusChangedCritical',
            'AggregationSourceDiscovered',
        ]

        matched = False
        for event_type in resource_event_types:
            if event_type in msg_id:
                matched = True
                break

        if not matched:
            errors.append(
                f"MessageId '{msg_id}' does not appear to be a ResourceEvent"
            )

        # For status change events, MessageArgs should contain status
        if 'ResourceStatusChanged' in msg_id:
            if 'MessageArgs' not in event_member:
                errors.append(
                    "ResourceStatusChanged event missing MessageArgs"
                )
            elif len(event_member.get('MessageArgs', [])) < 2:
                errors.append(
                    "ResourceStatusChanged MessageArgs should have at least 2 elements"
                )

        return errors


class EventMatcher:
    """Matches events against expectations"""

    @staticmethod
    def matches(event: Dict[str, Any], expectation: EventExpectation) -> bool:
        """Check if event matches expectation"""
        if not isinstance(event, dict):
            return False

        # Extract message from Events array if present
        if 'Events' in event and isinstance(event['Events'], list):
            if len(event['Events']) > 0:
                event_member = event['Events'][0]
            else:
                return False
        else:
            event_member = event

        # Check message ID pattern
        if expectation.message_id_pattern:
            msg_id = event_member.get('MessageId', '')
            if not expectation.message_id_pattern.search(msg_id):
                return False

        # Check severity
        if expectation.severity:
            severity = event_member.get('Severity', '')
            if severity != expectation.severity:
                return False

        # Check origin pattern
        if expectation.origin_pattern:
            origin = event_member.get('OriginOfCondition', '')
            if not expectation.origin_pattern.search(origin):
                return False

        # Check required fields
        for field in expectation.must_have_fields:
            if field not in event_member:
                return False

        return True

    @staticmethod
    def find_events(
        events: List[Dict[str, Any]],
        expectation: EventExpectation
    ) -> List[Dict[str, Any]]:
        """Find all events matching expectation"""
        return [e for e in events if EventMatcher.matches(e, expectation)]


class EventAssertion:
    """Provides assertion methods for event testing"""

    @staticmethod
    def assert_event_valid(event: Dict[str, Any]) -> None:
        """Assert event has valid structure"""
        errors = EventValidator.validate_event_structure(event)
        if errors:
            raise AssertionError(
                f"Event validation failed:\n" + "\n".join(errors)
            )

    @staticmethod
    def assert_message_id_matches(
        event: Dict[str, Any],
        pattern: str
    ) -> None:
        """Assert event MessageId matches pattern"""
        event_member = event.get('Events', [{}])[0] if 'Events' in event else event
        msg_id = event_member.get('MessageId', '')

        if not re.search(pattern, msg_id):
            raise AssertionError(
                f"MessageId '{msg_id}' does not match pattern '{pattern}'"
            )

    @staticmethod
    def assert_severity(event: Dict[str, Any], expected: str) -> None:
        """Assert event severity"""
        event_member = event.get('Events', [{}])[0] if 'Events' in event else event
        severity = event_member.get('Severity', '')

        if severity != expected:
            raise AssertionError(
                f"Expected severity '{expected}', got '{severity}'"
            )

    @staticmethod
    def assert_origin_contains(
        event: Dict[str, Any],
        pattern: str
    ) -> None:
        """Assert event OriginOfCondition contains pattern"""
        event_member = event.get('Events', [{}])[0] if 'Events' in event else event
        origin = event_member.get('OriginOfCondition', '')

        if pattern not in origin:
            raise AssertionError(
                f"OriginOfCondition '{origin}' does not contain '{pattern}'"
            )

    @staticmethod
    def assert_message_args(
        event: Dict[str, Any],
        expected_args: List[Any]
    ) -> None:
        """Assert event MessageArgs match expected"""
        event_member = event.get('Events', [{}])[0] if 'Events' in event else event
        actual_args = event_member.get('MessageArgs', [])

        if actual_args != expected_args:
            raise AssertionError(
                f"Expected MessageArgs {expected_args}, got {actual_args}"
            )

    @staticmethod
    def assert_event_count(events: List[Dict[str, Any]], count: int) -> None:
        """Assert number of events"""
        if len(events) != count:
            raise AssertionError(
                f"Expected {count} events, got {len(events)}"
            )

    @staticmethod
    def assert_resource_created(event: Dict[str, Any], origin: str) -> None:
        """Assert event is ResourceCreated for specific origin"""
        EventAssertion.assert_message_id_matches(
            event,
            r'ResourceCreated'
        )
        EventAssertion.assert_severity(event, 'OK')
        EventAssertion.assert_origin_contains(event, origin)

    @staticmethod
    def assert_resource_removed(event: Dict[str, Any], origin: str) -> None:
        """Assert event is ResourceRemoved for specific origin"""
        EventAssertion.assert_message_id_matches(
            event,
            r'ResourceRemoved'
        )
        EventAssertion.assert_severity(event, 'OK')
        EventAssertion.assert_origin_contains(event, origin)

    @staticmethod
    def assert_resource_changed(event: Dict[str, Any], origin: str) -> None:
        """Assert event is ResourceChanged for specific origin"""
        EventAssertion.assert_message_id_matches(
            event,
            r'ResourceChanged'
        )
        EventAssertion.assert_severity(event, 'OK')
        EventAssertion.assert_origin_contains(event, origin)

    @staticmethod
    def assert_status_changed_critical(
        event: Dict[str, Any],
        origin: str
    ) -> None:
        """Assert event is ResourceStatusChangedCritical"""
        EventAssertion.assert_message_id_matches(
            event,
            r'ResourceStatusChangedCritical'
        )
        EventAssertion.assert_severity(event, 'Critical')
        EventAssertion.assert_origin_contains(event, origin)

    @staticmethod
    def assert_status_changed_ok(
        event: Dict[str, Any],
        origin: str
    ) -> None:
        """Assert event is ResourceStatusChangedOK"""
        EventAssertion.assert_message_id_matches(
            event,
            r'ResourceStatusChangedOK'
        )
        EventAssertion.assert_severity(event, 'OK')
        EventAssertion.assert_origin_contains(event, origin)


class EventComparator:
    """Compares events and sequences"""

    @staticmethod
    def events_are_ordered_by_timestamp(events: List[Dict[str, Any]]) -> bool:
        """Check if events are in chronological order"""
        # Events from EventServiceManager should have EventId in order
        # (EventId increments monotonically)
        if len(events) < 2:
            return True

        for i in range(len(events) - 1):
            current = events[i]
            next_event = events[i + 1]

            # Extract event IDs
            current_id = None
            next_id = None

            if 'EventId' in current:
                current_id = current['EventId']
            elif 'Id' in current:
                current_id = current['Id']

            if 'EventId' in next_event:
                next_id = next_event['EventId']
            elif 'Id' in next_event:
                next_id = next_event['Id']

            # If we have IDs, they should be increasing
            if current_id is not None and next_id is not None:
                if int(current_id) >= int(next_id):
                    return False

        return True

    @staticmethod
    def extract_sequence(
        events: List[Dict[str, Any]],
        patterns: List[str]
    ) -> List[Dict[str, Any]]:
        """
        Extract events matching a sequence of patterns.
        
        Example:
            patterns = ['ResourceCreated', 'ResourceChanged', 'ResourceRemoved']
            sequence = extract_sequence(events, patterns)
        """
        sequence = []
        event_idx = 0

        for pattern in patterns:
            found = False
            while event_idx < len(events):
                if re.search(pattern, events[event_idx].get('MessageId', '')):
                    sequence.append(events[event_idx])
                    event_idx += 1
                    found = True
                    break
                event_idx += 1

            if not found:
                raise AssertionError(
                    f"Could not find event matching pattern '{pattern}' "
                    f"in sequence at position {len(sequence)}"
                )

        return sequence


# Example usage functions for testing
def verify_resource_lifecycle_events(events: List[Dict[str, Any]]) -> None:
    """Verify a complete resource lifecycle (Create -> Change -> Remove)"""
    patterns = ['ResourceCreated', 'ResourceChanged', 'ResourceRemoved']
    sequence = EventComparator.extract_sequence(events, patterns)

    assert len(sequence) == 3, f"Expected 3 events, got {len(sequence)}"
    for event in sequence:
        EventAssertion.assert_event_valid(event)


def verify_satellite_health_recovery(events: List[Dict[str, Any]]) -> None:
    """Verify satellite health recovery sequence (Critical -> OK)"""
    patterns = ['ResourceStatusChangedCritical', 'ResourceStatusChangedOK']
    sequence = EventComparator.extract_sequence(events, patterns)

    assert len(sequence) == 2, f"Expected 2 events, got {len(sequence)}"
    for event in sequence:
        EventAssertion.assert_event_valid(event)


if __name__ == "__main__":
    # Example test event
    example_event = {
        "@odata.type": "#Event.v1_4_0.Event",
        "Name": "Event Log",
        "Id": "1",
        "Events": [
            {
                "MemberId": "0",
                "MessageId": "ResourceEvent.1.3.ResourceCreated",
                "Severity": "OK",
                "OriginOfCondition": "/redfish/v1/AggregationService/AggregationSources/sat1",
                "MessageArgs": []
            }
        ]
    }

    # Validate example
    print("Validating example event...")
    errors = EventValidator.validate_event_structure(example_event)
    if errors:
        print("Validation errors:")
        for error in errors:
            print(f"  - {error}")
    else:
        print("✓ Event is valid")

    # Test assertion
    try:
        EventAssertion.assert_resource_created(
            example_event,
            "AggregationSources"
        )
        print("✓ ResourceCreated assertion passed")
    except AssertionError as e:
        print(f"✗ Assertion failed: {e}")
