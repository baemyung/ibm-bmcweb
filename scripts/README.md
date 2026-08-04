# Test Scripts and Event Listener - Quick Reference

## Scripts Location

```
scripts/
├── test-satellite-events-e2e.sh              # Curl-based HTTP API tests
├── redfish-event-listener.py                 # HTTP event listener
└── event_verification_helpers.py             # Event validation utilities
```

## Quick Start

### 1. Build Integration Tests

```bash
cd build
meson test satellite_event_forwarding_integration_test -v
```

### 2. Start Event Listener

```bash
python3 scripts/redfish-event-listener.py --listen 0.0.0.0:8888 --daemon &
```

### 3. Run End-to-End Tests

```bash
scripts/test-satellite-events-e2e.sh full
```

### 4. Check Results

```bash
python3 scripts/redfish-event-listener.py --report
```

## Script Reference

### test-satellite-events-e2e.sh

Curl-based HTTP testing script for bmcweb EventService.

**Common Commands**:

```bash
# Check prerequisites
./test-satellite-events-e2e.sh check

# List existing aggregation sources
./test-satellite-events-e2e.sh list-agg-sources

# Create a new satellite source
./test-satellite-events-e2e.sh create-agg-source satellite1.local

# Create event subscription
./test-satellite-events-e2e.sh create-subscription

# Send a test event
./test-satellite-events-e2e.sh send-test-event

# Verify satellite connectivity
./test-satellite-events-e2e.sh verify-satellite /redfish/v1/AggregationService/AggregationSources/sat1

# List event subscriptions
./test-satellite-events-e2e.sh list-subscriptions

# Delete a subscription
./test-satellite-events-e2e.sh delete-subscription 1

# Run complete test suite
./test-satellite-events-e2e.sh full
```

**Environment Variables**:

```bash
BMCWEB_HOST=localhost          # Default: localhost
BMCWEB_PORT=18080             # Default: 18080
BMCWEB_USER=root              # Default: root
BMCWEB_PASSWORD=0penBmc       # Default: 0penBmc
SUBSCRIBER_HOST=localhost     # Default: localhost
SUBSCRIBER_PORT=8888          # Default: 8888
```

**Example**:

```bash
export BMCWEB_HOST=bmc.example.com
export BMCWEB_PORT=8443
./test-satellite-events-e2e.sh list-agg-sources
```

### redfish-event-listener.py

HTTP event listener that captures and analyzes Redfish events.

**Common Commands**:

```bash
# Start as daemon
python3 redfish-event-listener.py --listen 0.0.0.0:8888 --daemon

# Check events
python3 redfish-event-listener.py --check-events

# Generate report
python3 redfish-event-listener.py --report

# Filter by message ID
python3 redfish-event-listener.py --filter-message-id ResourceCreated

# Filter by severity
python3 redfish-event-listener.py --filter-severity Critical

# Filter by resource type
python3 redfish-event-listener.py --filter-resource-type AggregationSource

# Filter by origin pattern
python3 redfish-event-listener.py --filter-origin AggregationSources

# Save to file
python3 redfish-event-listener.py --save events.json

# Clear event store
python3 redfish-event-listener.py --clear

# Show help
python3 redfish-event-listener.py --help
```

**Endpoints**:

- `POST /events` - Receives Redfish events
- `GET /health` - Healthcheck endpoint

**Example Session**:

```bash
# Terminal 1: Start listener
python3 scripts/redfish-event-listener.py --listen 0.0.0.0:8888 --daemon

# Terminal 2: Send test event
scripts/test-satellite-events-e2e.sh send-test-event

# Terminal 3: Check events
python3 scripts/redfish-event-listener.py --check-events
```

### event_verification_helpers.py

Python module for event validation and assertions.

**Usage in Python Scripts**:

```python
from event_verification_helpers import (
    EventValidator,
    EventAssertion,
    EventMatcher,
    EventComparator,
    verify_resource_lifecycle_events,
    verify_satellite_health_recovery
)

# Validate event structure
errors = EventValidator.validate_event_structure(event)
if errors:
    for error in errors:
        print(f"Error: {error}")

# Assert on specific properties
EventAssertion.assert_resource_created(event, "AggregationSources/sat1")
EventAssertion.assert_severity(event, "OK")
EventAssertion.assert_message_id_matches(event, r"ResourceCreated")

# Verify event sequences
verify_resource_lifecycle_events(events)  # Create -> Change -> Delete
verify_satellite_health_recovery(events)  # Critical -> OK

# Find matching events
expectation = EventExpectation(
    message_id_pattern=re.compile(r"ResourceCreated"),
    resource_type="AggregationSource"
)
matching = EventMatcher.find_events(events, expectation)
```

## Testing Workflows

### Workflow 1: Manual HTTP Testing

```bash
# 1. Check system
./test-satellite-events-e2e.sh check

# 2. Create satellite source
./test-satellite-events-e2e.sh create-agg-source satellite1.local

# 3. Create subscription
./test-satellite-events-e2e.sh create-subscription

# 4. Send test event
./test-satellite-events-e2e.sh send-test-event

# 5. Verify delivery
python3 scripts/redfish-event-listener.py --check-events
```

### Workflow 2: Automated Testing

```bash
# Run full test suite
./test-satellite-events-e2e.sh full

# Generate report
python3 scripts/redfish-event-listener.py --report
```

### Workflow 3: CI/CD Pipeline

```bash
#!/bin/bash
set -e

# Start services
python3 scripts/redfish-event-listener.py --daemon &
LISTENER_PID=$!

# Run tests
./test-satellite-events-e2e.sh check
./test-satellite-events-e2e.sh full

# Verify results
python3 scripts/redfish-event-listener.py --save results.json

# Cleanup
kill $LISTENER_PID
```

## Common Issues

### Connection Refused

```bash
# Check bmcweb is running
curl -k https://localhost:18080/redfish/v1/

# Adjust host/port
export BMCWEB_HOST=localhost BMCWEB_PORT=18080
```

### Authentication Failed

```bash
# Check credentials
curl -k -X POST https://localhost:18080/redfish/v1/SessionService/Sessions \
    -H "Content-Type: application/json" \
    -d '{"UserName":"root","Password":"0penBmc"}'
```

### Event Listener Port in Use

```bash
# Find process
lsof -i :8888

# Use different port
export SUBSCRIBER_PORT=9999
```

### No Events Received

```bash
# Verify listener is running
curl http://localhost:8888/health

# Check subscription destination
./test-satellite-events-e2e.sh list-subscriptions

# Verify subscription filters match
```

## Debugging

### Verbose Output

```bash
# Shell script debugging
bash -x test-satellite-events-e2e.sh check

# Python debugging
python3 -u redfish-event-listener.py --listen 0.0.0.0:8888
```

### Inspect Events

```bash
# Show all events
python3 scripts/redfish-event-listener.py --check-events

# Show specific event type
python3 scripts/redfish-event-listener.py \
    --filter-message-id ResourceCreated

# Save for analysis
python3 scripts/redfish-event-listener.py --save events.json
jq '.[0]' events.json  # Show first event
```

## Integration Tests

### Run All Integration Tests

```bash
cd build
meson test -k "Integration" -v
```

### Run Specific Test

```bash
meson test satellite_event_forwarding_integration_test -v
```

### Run with Filtering

```bash
meson test satellite_event_forwarding_integration_test -v \
    -- --filter=MultipleEvent
```

## References

- **Main Testing Guide**: [docs/TESTING-SATELLITE-EVENTS.md](../docs/TESTING-SATELLITE-EVENTS.md)
- **Implementation Plan**: [docs/STEP-B-COMPLETION.md](../docs/STEP-B-COMPLETION.md)
- **Test Files**: [test/redfish-core/include/](../test/redfish-core/include/)
- **Redfish Specs**: [https://redfish.dmtf.org/](https://redfish.dmtf.org/)

## Examples

### Example 1: Test Satellite Discovery

```bash
# Start listener
python3 scripts/redfish-event-listener.py --daemon &

# Create satellite
./test-satellite-events-e2e.sh create-agg-source sat1.local

# Verify event was sent
python3 scripts/redfish-event-listener.py \
    --filter-message-id AggregationSourceDiscovered
```

### Example 2: Verify Event Filtering

```bash
# Send multiple events
./test-satellite-events-e2e.sh send-test-event

# Filter by resource type
python3 scripts/redfish-event-listener.py \
    --filter-resource-type AggregationSource

# Filter by severity
python3 scripts/redfish-event-listener.py \
    --filter-severity OK
```

### Example 3: Generate Test Report

```bash
# Run tests
./test-satellite-events-e2e.sh full

# Generate report
python3 scripts/redfish-event-listener.py --report

# Save for analysis
python3 scripts/redfish-event-listener.py --save test-results.json
```

## Performance Notes

- Event listener uses thread-safe storage (suitable for concurrent subscribers)
- Scripts support custom timeouts for slow networks
- Large event stores can be filtered to avoid memory issues
- JSON parsing optimized for typical Redfish event sizes

## Security Notes

- Scripts use HTTPS (self-signed cert OK for testing)
- Session tokens are managed securely
- Event listener stores events in memory only (cleared on restart)
- No credentials logged or saved by default

## Maintenance

### Update Event Listener

If bmcweb event format changes, update:
- `redfish-event-listener.py` - Event parsing logic
- `event_verification_helpers.py` - Validation rules

### Add New Tests

1. Add test case to appropriate integration test file
2. Update `docs/TESTING-SATELLITE-EVENTS.md`
3. Add scenario to this README if applicable

### Version Compatibility

- Tested with: Python 3.8+, bash 4.0+, curl 7.0+
- Requires: jq for JSON processing
- Optional: pytest for automated testing

## Support

For issues or questions:
1. Check this README's "Common Issues" section
2. Review main testing guide: [TESTING-SATELLITE-EVENTS.md](../docs/TESTING-SATELLITE-EVENTS.md)
3. Check test file comments for implementation details
4. Open issue on project repository with:
   - Test output (full log)
   - Environment details (OS, versions)
   - Steps to reproduce
