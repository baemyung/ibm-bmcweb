# HTTP Client Coredump Reproduction - Python Client Script

## Overview

This document explains how to use the Python client-side script [`test_http_client_coredump.py`](test_http_client_coredump.py) to reproduce the HTTP client coredump issue documented in [http_client_coredump_analysis.md](http_client_coredump_analysis.md).

## Prerequisites

- Python 3.7 or higher
- `requests` library
- `urllib3` library
- Access to a running BMCWeb instance

## Installation

```bash
# Install required Python packages
pip3 install requests urllib3

# Or using a virtual environment (recommended)
python3 -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
pip install requests urllib3
```

## Usage

### Basic Usage

Run all tests against a BMCWeb instance:

```bash
python3 test_http_client_coredump.py --host <bmcweb_ip> --port <port>
```

**Example:**
```bash
python3 test_http_client_coredump.py --host 192.168.1.100 --port 443
```

### Run Specific Test

Run only a specific test (1-5):

```bash
python3 test_http_client_coredump.py --host 192.168.1.100 --port 443 --test 3
```

### Use HTTP Instead of HTTPS

```bash
python3 test_http_client_coredump.py --host 192.168.1.100 --port 80 --no-ssl
```

### Command-Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `--host` | BMCWeb host IP address (required) | - |
| `--port` | BMCWeb port number | 443 |
| `--no-ssl` | Use HTTP instead of HTTPS | False (uses HTTPS) |
| `--test` | Run specific test only (1-5) | All tests |

## Test Descriptions

### Test 1: Rapid Client Destruction

**Purpose:** Creates and destroys HTTP sessions rapidly while requests are in flight.

**What it does:**
- Creates 50 new sessions
- Sends a POST request to create an event subscription
- Immediately closes the session (10ms delay)
- Destroys the session object

**Expected behavior (before fix):**
- BMCWeb may crash when callbacks try to access destroyed ConnectionPool
- Segmentation fault in `afterSendData()` or `sendNext()`

**Command:**
```bash
python3 test_http_client_coredump.py --host 192.168.1.100 --test 1
```

### Test 2: Shutdown During Active Requests

**Purpose:** Sends multiple concurrent requests and abruptly closes all connections.

**What it does:**
- Creates a single session
- Sends 30 concurrent requests to various Redfish endpoints
- Waits 50ms for requests to be in various states
- Abruptly closes the session while requests are pending

**Expected behavior (before fix):**
- Multiple segmentation faults as different async operations complete
- Crashes in various callback functions

**Command:**
```bash
python3 test_http_client_coredump.py --host 192.168.1.100 --test 2
```

### Test 3: Connection Pool Stress Test

**Purpose:** Overwhelms the connection pool with many concurrent requests.

**What it does:**
- Creates a session with limited connection pool (10 connections)
- Submits 50 requests concurrently using ThreadPoolExecutor
- Destroys session after 20ms while pool is active
- Attempts to collect results (most will fail)

**Expected behavior (before fix):**
- Crash in `sendNext()` with out-of-bounds `connId`
- Crash when accessing destroyed connection pool

**Command:**
```bash
python3 test_http_client_coredump.py --host 192.168.1.100 --test 3
```

### Test 4: Timer Race Condition

**Purpose:** Triggers timeout while destroying connection.

**What it does:**
- Sends request with 2-second timeout to non-routable IP
- Waits 1.9 seconds (just before timeout)
- Destroys session just before timeout fires
- Waits for timeout to occur

**Expected behavior (before fix):**
- Race condition between timer callback and destruction
- Crash in `onTimeout()` when accessing destroyed object

**Command:**
```bash
python3 test_http_client_coredump.py --host 192.168.1.100 --test 4
```

### Test 5: Event Subscription Lifecycle

**Purpose:** Simulates real-world scenario of creating and deleting event subscriptions.

**What it does:**
- Creates 10 event subscriptions
- Immediately deletes each subscription after creation
- Abruptly closes session after each cycle
- Simulates rapid subscription management

**Expected behavior (before fix):**
- Callback executes after client destruction
- Access to destroyed ConnectionPool
- Segmentation fault in callback

**Command:**
```bash
python3 test_http_client_coredump.py --host 192.168.1.100 --test 5
```

## Monitoring for Coredumps

### On BMCWeb Side

1. **Enable core dumps:**
   ```bash
   ulimit -c unlimited
   ```

2. **Run BMCWeb with debugging:**
   ```bash
   ./bmcweb --verbose
   ```

3. **Monitor logs:**
   ```bash
   journalctl -u bmcweb -f
   ```

4. **Check for core dumps:**
   ```bash
   ls -lh /var/lib/systemd/coredump/
   # or
   coredumpctl list
   ```

### With AddressSanitizer

Build BMCWeb with ASan to detect use-after-free:

```bash
meson setup build-asan -Db_sanitize=address -Db_lundef=false
ninja -C build-asan
./build-asan/bmcweb
```

Then run the Python script. ASan will report any memory errors immediately.

### With Valgrind

Run BMCWeb under Valgrind:

```bash
valgrind --leak-check=full --show-leak-kinds=all \
         --track-origins=yes --verbose \
         ./build/bmcweb
```

Then run the Python script and check Valgrind output for memory errors.

## Expected Output

### Successful Test Run (After Fix)

```
============================================================
HTTP Client Coredump Reproduction Tests
Target: https://192.168.1.100:443
============================================================

[TEST 1] Rapid Client Destruction (50 iterations)
============================================================
  Completed 10/50 iterations
  Completed 20/50 iterations
  Completed 30/50 iterations
  Completed 40/50 iterations
  Completed 50/50 iterations
✓ Test 1 complete

[TEST 2] Shutdown During Active Requests (30 requests)
============================================================
  Closing session while requests are in flight...
✓ Test 2 complete

[TEST 3] Connection Pool Stress Test (50 requests)
============================================================
  Destroying session while pool is active...
  Completed: 0, Failed: 50/50
✓ Test 3 complete

[TEST 4] Timer Race Condition
============================================================
  Timeout occurred (expected)
  Destroying session near timeout...
✓ Test 4 complete

[TEST 5] Event Subscription Lifecycle (10 cycles)
============================================================
  Completed 5/10 cycles
  Completed 10/10 cycles
✓ Test 5 complete

============================================================
All tests completed!
Check BMCWeb logs for crashes or coredumps
============================================================
```

### Failed Test Run (Before Fix)

BMCWeb will crash with messages like:

```
Segmentation fault (core dumped)
```

Or with ASan:

```
==12345==ERROR: AddressSanitizer: heap-use-after-free
READ of size 8 at 0x60300000eff0 thread T1
    #0 in crow::ConnectionPool::sendNext()
    #1 in crow::ConnectionPool::afterSendData()
```

## Troubleshooting

### Connection Refused

**Error:** `requests.exceptions.ConnectionError: Connection refused`

**Solution:** 
- Verify BMCWeb is running
- Check the host IP and port are correct
- Ensure firewall allows connections

### SSL Certificate Errors

**Error:** `SSLError: certificate verify failed`

**Solution:** The script already disables SSL verification for testing. If you still see this error, use `--no-ssl` flag.

### Authentication Errors

**Error:** `401 Unauthorized`

**Solution:** Test 5 uses default credentials (`root`/`0penBmc`). Update the script if your BMCWeb uses different credentials:

```python
auth=('your_username', 'your_password')
```

### No Coredump Detected

If tests complete but no coredump is detected:

1. The fix may already be applied
2. Try running with ASan or Valgrind for better detection
3. Increase iteration counts in the script
4. Try running tests multiple times

## Integration with CI/CD

### Example GitHub Actions Workflow

```yaml
name: Test HTTP Client Coredump

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      
      - name: Build BMCWeb with ASan
        run: |
          meson setup build-asan -Db_sanitize=address -Db_lundef=false
          ninja -C build-asan
      
      - name: Start BMCWeb
        run: |
          ./build-asan/bmcweb &
          sleep 5
      
      - name: Run Coredump Tests
        run: |
          pip3 install requests urllib3
          python3 test_http_client_coredump.py --host localhost --port 443
      
      - name: Check for crashes
        run: |
          if coredumpctl list | grep bmcweb; then
            echo "Coredump detected!"
            exit 1
          fi
```

## Related Documentation

- [http_client_coredump_analysis.md](http_client_coredump_analysis.md) - Detailed analysis of the coredump causes
- [http_client_coredump_reproduction.md](http_client_coredump_reproduction.md) - C++ reproduction methods
- [`http/http_client.hpp`](http/http_client.hpp) - Source code with the issue

## Summary

This Python script provides a **client-side** method to reproduce the HTTP client coredump issue by:

1. Creating rapid connection/disconnection cycles
2. Destroying sessions while async operations are pending
3. Overwhelming connection pools
4. Triggering timeout race conditions
5. Simulating real-world event subscription scenarios

The key to reproduction is **timing** - destroying the client while async operations are in flight but before they complete. The script uses various techniques (non-routable IPs, short timeouts, rapid destruction) to reliably trigger the use-after-free conditions.

**After the fix is applied**, all tests should complete cleanly without crashes or memory errors.