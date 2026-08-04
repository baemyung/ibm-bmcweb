# Satellite Event Forwarding - Integration and End-to-End Test Guide

## Overview

This document provides comprehensive instructions for running the integration and end-to-end tests for satellite event forwarding in bmcweb (STEP B).

The test suite validates:
- **Satellite event forwarding infrastructure** with EventServiceManager
- **Aggregation source lifecycle events** (Create, Update, Delete)
- **Satellite health monitoring** events (Critical/OK status transitions)
- **End-to-end event delivery** through HTTP subscriptions

## Test Architecture

### Test Layers

1. **Unit Tests** (already exist)
   - `satellite_event_forwarding_test.cpp` - URI prefix-fixing, message formats
   - `missing_aggregation_events_test.cpp` - Placeholder tests with documentation
   - `event_matches_filter_test.cpp` - Event filtering logic

2. **Integration Tests** (NEW - STEP B)
   - `satellite_event_forwarding_integration_test.cpp` - Event forwarding with EventServiceManager
   - `aggregation_source_lifecycle_integration_test.cpp` - Lifecycle event delivery (placeholders)
   - `satellite_health_monitoring_integration_test.cpp` - Health monitoring events (placeholders)

3. **End-to-End Tests** (NEW - STEP B)
   - `test-satellite-events-e2e.sh` - Curl-based HTTP API tests
   - `redfish-event-listener.py` - Event subscriber and verification
   - `event_verification_helpers.py` - Event validation utilities

## Prerequisites

### System Requirements

- **bmcweb** running with EventService support
- **Python 3.8+** for test scripts
- **curl** for HTTP testing
- **jq** for JSON processing
- **Linux/macOS/Unix shell** for bash scripts

### Build Requirements

```bash
# Build bmcweb with event service support
meson setup build -Dbmcweb-experimental-redfish-dbus-log-subscription=enabled
cd build
ninja
ninja test
```

### Runtime Requirements

```bash
# Start bmcweb (or use existing running instance)
sudo ./build/bmcweb -D

# In another terminal, start the event listener
python3 scripts/redfish-event-listener.py --listen 0.0.0.0:8888 --daemon

# Run tests
cd tests
./run-satellite-event-tests.sh
```

## Running Tests

### Quick Start

```bash
# 1. Verify prerequisites
./scripts/test-satellite-events-e2e.sh check

# 2. Run integration tests (with meson)
cd build
meson test satellite_event_forwarding_integration_test

# 3. Run end-to-end curl tests
../scripts/test-satellite-events-e2e.sh full

# 4. Check event listener for received events
python3 scripts/redfish-event-listener.py --report
```

### Build and Run Unit Tests

```bash
# Navigate to build directory
cd build

# Run specific integration test suite
meson test satellite_event_forwarding_integration_test -v

# Run all event-related tests
meson test -k "Event" -v

# Run specific test case
meson test satellite_event_forwarding_integration_test -v -- --filter=SatelliteResourceCreatedEvent
```

### Run End-to-End Tests (Shell Scripts)

#### 1. Start Prerequisites

```bash
# Terminal 1: Start bmcweb
sudo ./build/bmcweb -D

# Terminal 2: Start event listener
python3 scripts/redfish-event-listener.py --listen 0.0.0.0:8888 --daemon

# Terminal 3: Run tests
cd scripts
```

#### 2. Check System Health

```bash
./test-satellite-events-e2e.sh check
```

Expected output:
```
[TEST] Checking prerequisites...
[PASS] Prerequisites check passed
```

#### 3. Run Full Test Suite

```bash
./test-satellite-events-e2e.sh full
```

This runs:
1. Prerequisites check
2. Creates test AggregationSource
3. Verifies satellite connectivity
4. Creates event subscription
5. Sends test events
6. Lists all resources

#### 4. Individual Test Commands

```bash
# List existing AggregationSources
./test-satellite-events-e2e.sh list-agg-sources

# Create a new satellite source
./test-satellite-events-e2e.sh create-agg-source satellite1.local

# Create event subscription
./test-satellite-events-e2e.sh create-subscription

# Send test event
./test-satellite-events-e2e.sh send-test-event

# Verify satellite health
./test-satellite-events-e2e.sh verify-satellite /redfish/v1/AggregationService/AggregationSources/sat1

# List subscriptions
./test-satellite-events-e2e.sh list-subscriptions

# Delete subscription
./test-satellite-events-e2e.sh delete-subscription 1
```

### Run Event Verification Tests (Python)

#### 1. Start Event Listener

```bash
# As daemon (runs in background)
python3 scripts/redfish-event-listener.py --listen 0.0.0.0:8888 --daemon &

# Or in foreground for debugging
python3 scripts/redfish-event-listener.py --listen 0.0.0.0:8888
```

#### 2. Check Received Events

```bash
# Display all events
python3 scripts/redfish-event-listener.py --check-events

# Filter by message ID
python3 scripts/redfish-event-listener.py \
    --filter-message-id ResourceCreated

# Filter by severity
python3 scripts/redfish-event-listener.py \
    --filter-severity Critical

# Filter by resource type
python3 scripts/redfish-event-listener.py \
    --filter-resource-type AggregationSource

# Filter by origin pattern
python3 scripts/redfish-event-listener.py \
    --filter-origin AggregationSources
```

#### 3. Generate Reports

```bash
# Summary report
python3 scripts/redfish-event-listener.py --report

# Save events to file
python3 scripts/redfish-event-listener.py --save events.json

# Clear event store
python3 scripts/redfish-event-listener.py --clear
```

#### 4. Use Event Verification Helpers

```python
from event_verification_helpers import EventValidator, EventAssertion

# Validate event structure
errors = EventValidator.validate_event_structure(event)

# Assert on specific event
EventAssertion.assert_resource_created(event, "AggregationSources/sat1")

# Verify lifecycle sequence
verify_resource_lifecycle_events(events)  # Create -> Change -> Delete

# Verify health recovery sequence
verify_satellite_health_recovery(events)  # Critical -> OK
```

## Test Scenarios

### Scenario 1: Satellite Event Forwarding

**Goal**: Verify that events with satellite URIs are correctly prefixed and forwarded.

**Steps**:
1. Create an AggregationSource with satellite BMC details
2. Create event subscription for AggregationSource resources
3. Emit a test event with satellite origin
4. Verify event reaches subscriber with correct prefix

**Verification**:
```bash
./test-satellite-events-e2e.sh create-agg-source satellite1.local
./test-satellite-events-e2e.sh create-subscription
./test-satellite-events-e2e.sh send-test-event
python3 scripts/redfish-event-listener.py --check-events
```

Expected: Event with ResourceCreated MessageId and satellite URI in origin.

### Scenario 2: Aggregation Source Lifecycle

**Goal**: Verify lifecycle events (Create, Update, Delete) are emitted.

**Note**: These are placeholder tests that will pass once STEP C is implemented.

**Steps**:
1. Run integration test suite for lifecycle events
2. Tests should skip with explanation until implementation
3. Once implemented, replace GTEST_SKIP() with actual assertions

**Verification**:
```bash
cd build
meson test aggregation_source_lifecycle_integration_test -v
```

Expected output (until STEP C):
```
[  SKIPPED ] AggregationSourceCreateEmitsResourceCreated
  [MISSING - STEP C] AggregationSource POST does not yet emit ResourceCreated events...
```

### Scenario 3: Satellite Health Monitoring

**Goal**: Verify health status change events (Critical/OK) are emitted.

**Note**: These are placeholder tests that will pass once STEP C is implemented.

**Steps**:
1. Run health monitoring integration test suite
2. Tests should skip with explanation until implementation
3. Once implemented, replace GTEST_SKIP() with actual assertions

**Verification**:
```bash
cd build
meson test satellite_health_monitoring_integration_test -v
```

Expected output (until STEP C):
```
[  SKIPPED ] SatelliteUnreachableEmitsStatusCriticalEvent
  [MISSING - STEP C] Satellite health state tracking does not yet exist...
```

## Environment Variables

Control test behavior with environment variables:

```bash
# bmcweb connection
export BMCWEB_HOST=bmc.example.com
export BMCWEB_PORT=18080
export BMCWEB_USER=root
export BMCWEB_PASSWORD=0penBmc

# Event subscriber
export SUBSCRIBER_HOST=subscriber.example.com
export SUBSCRIBER_PORT=8888

# Run tests
./test-satellite-events-e2e.sh full
```

## Test Data and Artifacts

### Event Storage

- Integration test events: Stored in-memory during test run
- End-to-end test events: Captured by `redfish-event-listener.py`
- Event store location: `./events/` directory
- Saved events: `./events.json` (when using `--save` flag)

### Log Files

- bmcweb logs: `./build/bmcweb.log` (if configured)
- Test runner logs: `./build/meson-logs/`
- Event listener logs: stdout/stderr from Python script

## Debugging

### Verbose Output

```bash
# Verbose integration test output
cd build
meson test satellite_event_forwarding_integration_test -v --setup plain

# Verbose curl testing
VERBOSE=1 ../scripts/test-satellite-events-e2e.sh full

# Verbose event listener
python3 scripts/redfish-event-listener.py --listen 0.0.0.0:8888
```

### Inspect HTTP Traffic

```bash
# Use curl with verbose output
curl -v -k -X GET \
    -H "X-Auth-Token: $TOKEN" \
    https://localhost:18080/redfish/v1/AggregationService/AggregationSources

# Use tcpdump to capture network traffic
sudo tcpdump -i lo -A 'tcp port 18080 or tcp port 8888'
```

### Event Listener Debugging

```bash
# Check listener health
curl http://localhost:8888/health

# Check received events (from listener process)
python3 scripts/redfish-event-listener.py --check-events
```

## Troubleshooting

### Connection Refused

**Error**: `Cannot connect to bmcweb`

**Solution**:
```bash
# Check bmcweb is running
sudo systemctl status bmcweb
# or
sudo ./build/bmcweb -D

# Check port
netstat -tlnp | grep 18080

# Adjust host/port
export BMCWEB_HOST=localhost BMCWEB_PORT=18080
```

### Authentication Failed

**Error**: `Failed to acquire session token`

**Solution**:
```bash
# Check credentials
export BMCWEB_USER=root
export BMCWEB_PASSWORD=0penBmc

# Test authentication directly
curl -k -X POST https://localhost:18080/redfish/v1/SessionService/Sessions \
    -H "Content-Type: application/json" \
    -d '{"UserName":"root","Password":"0penBmc"}'
```

### Event Listener Port in Use

**Error**: `Address already in use`

**Solution**:
```bash
# Find process using port 8888
lsof -i :8888

# Kill existing process
kill -9 <PID>

# Or use different port
export SUBSCRIBER_PORT=9999
```

### Events Not Received

**Error**: `No events received`

**Solution**:
1. Check listener is running: `curl http://localhost:8888/health`
2. Check subscription destination is correct: `curl ... list-subscriptions`
3. Check subscription filters match your events
4. Verify bmcweb can reach subscriber (firewall, network)
5. Enable debug logging in bmcweb for EventService

## Integration with CI/CD

### GitHub Actions Example

```yaml
name: Satellite Event Forwarding Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y python3 curl jq
      
      - name: Build bmcweb
        run: |
          meson setup build -Dbmcweb-experimental-redfish-dbus-log-subscription=enabled
          cd build
          ninja
      
      - name: Run integration tests
        run: |
          cd build
          meson test satellite_event_forwarding_integration_test -v
          meson test aggregation_source_lifecycle_integration_test -v
          meson test satellite_health_monitoring_integration_test -v
      
      - name: Run end-to-end tests
        run: |
          # Start bmcweb
          sudo ./build/bmcweb -D &
          sleep 2
          
          # Start event listener
          python3 scripts/redfish-event-listener.py --daemon &
          sleep 1
          
          # Run tests
          scripts/test-satellite-events-e2e.sh full
          
          # Generate report
          python3 scripts/redfish-event-listener.py --report
```

## Success Criteria

### Unit/Integration Tests Pass

```bash
cd build
meson test -k "SatelliteEvent" -v
# Expected: All tests pass or skip with clear explanations
```

### End-to-End Tests Complete

```bash
./test-satellite-events-e2e.sh full
# Expected: All operations succeed, no connection errors
```

### Events Delivered

```bash
python3 scripts/redfish-event-listener.py --report
# Expected: Events received, correct counts by type/severity
```

### No Warnings or Errors

All tests complete without:
- Connection timeouts
- JSON parsing errors
- Missing required fields
- Invalid message formats

## Next Steps (STEP C)

Once integration tests pass, STEP C will:

1. **Remove GTEST_SKIP()** from placeholder tests
2. **Implement sendEvent() calls** in:
   - `handleAggregationSourceCollectionPost()` (ResourceCreated)
   - `handleAggregationSourcePatch()` (ResourceChanged)
   - `handleAggregationSourceDelete()` (ResourceRemoved)
   - `addSatelliteConfig()` (AggregationSourceDiscovered)
   - `forwardRequest()` (ResourceStatusChanged*)

3. **Fill in test assertions** to verify:
   - Event structure is valid
   - MessageId matches expected type
   - OriginOfCondition points to correct resource
   - Events reach subscribers matching filter criteria

4. **Implement per-satellite health tracking** for:
   - Failure thresholds (emit Critical after N failures)
   - Recovery detection (emit OK after success)
   - Optional periodic health polling

## References

- [Redfish Event Model](https://redfish.dmtf.org/schemas/Event.v1_4_0.json)
- [Redfish AggregationService](https://redfish.dmtf.org/schemas/AggregationService.v1_3_0.json)
- [Redfish Event Listener](https://github.com/DMTF/Redfish-Event-Listener)
- [bmcweb EventService](../redfish-core/lib/event_service.hpp)

## Support and Questions

For issues or questions:
1. Check this guide's troubleshooting section
2. Review test comments and documentation
3. Check bmcweb build logs
4. Open an issue on the project repository
