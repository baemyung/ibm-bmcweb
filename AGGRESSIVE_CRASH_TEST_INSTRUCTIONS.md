# Aggressive HTTP Client Crash Test Instructions

## Overview

The aggressive crash test (`http-client-aggressive-test`) is designed to maximize the probability of triggering race conditions in the HTTP client by:

1. **Creating multiple concurrent async operations** - Each client sends 5 requests to different unreachable IPs
2. **Immediate destruction** - Clients are destroyed with NO delays while async operations are in flight
3. **High iteration count** - Default 1000 iterations to increase probability
4. **Multi-threaded execution** - Concurrent threads creating/destroying clients simultaneously

## Why Test 2 Shows Multiple "Maximum retries" Messages

When you run `--test 2` and see journal entries like:
```
May 21 14:28:33 p11bmc bmcwebd[1247]: [http_client.hpp:474] Maximum number of retries reached. http://192.0.2.1:8080/events
May 21 14:28:33 p11bmc bmcwebd[1247]: [http_client.hpp:474] Maximum number of retries reached. http://192.0.2.1:8080/events
...
```

This is **EXPECTED** and **NOT** evidence of retries. Here's why:

- Test 2 creates **10 concurrent HttpClient instances**
- Each client has `maxRetryAttempts = 0` (fail immediately, no retries)
- Each client tries to connect to an unreachable IP
- Each connection fails and logs "Maximum number of retries reached" at line 474
- The **7 messages** are from **7 different client instances**, not 7 retries

The test is working correctly - there are no actual retries happening.

## Building the Test

```bash
# From the bmcweb directory
meson setup build
cd build
ninja http-client-aggressive-test
```

## Running the Test

### Basic Usage
```bash
# Run with defaults (1000 iterations, 4 threads)
./http-client-aggressive-test

# Custom iterations
./http-client-aggressive-test 5000

# Custom iterations and threads
./http-client-aggressive-test 5000 8
```

### On BMC
```bash
# Copy to BMC
scp build/http-client-aggressive-test root@<bmc-ip>:/tmp/

# SSH to BMC
ssh root@<bmc-ip>

# Run the test
/tmp/http-client-aggressive-test 2000 4
```

## What the Test Does

### Phase 1: Single-Threaded Aggressive Test
- Creates HttpClient instances in rapid succession
- Each client sends 5 requests to different unreachable IPs (192.0.2.x)
- Immediately destroys the client (no sleep/delay)
- This creates a race between:
  - Async DNS resolution operations
  - Async connection attempts
  - Async timer callbacks
  - Client destruction

### Phase 2: Multi-Threaded Test
- Multiple threads simultaneously create/destroy clients
- Increases contention on the io_context
- Maximizes the chance of callbacks firing after destruction

## Expected Behavior

### If NO Crash Occurs
This could mean:
1. **The race condition is already fixed** in this codebase
2. **The race is very timing-sensitive** and requires specific conditions
3. **The test needs even more aggressive parameters**

### If a Crash Occurs
You'll see:
```bash
Segmentation fault (core dumped)
```

Check system logs:
```bash
# Check for segfault
dmesg | tail -50

# Check journal
journalctl -n 100 | grep -i 'segfault\|core\|crash'

# Find core dump
coredumpctl list
coredumpctl info <PID>
coredumpctl gdb <PID>
```

## Understanding the Race Condition

The race condition occurs in this sequence:

1. **HttpClient created** → Creates ConnectionPool → Creates ConnectionInfo objects
2. **sendData() called** → Starts async operations:
   - DNS resolution (`async_resolve`)
   - Connection attempts (`async_connect`)
   - Retry timers (`timer.async_wait`)
3. **HttpClient destroyed** → Destroys ConnectionPool → Destroys ConnectionInfo shared_ptrs
4. **Async callbacks fire** → Try to lock `weak_ptr<ConnectionInfo>`

The crash happens when:
- The `shared_ptr<ConnectionInfo>` is destroyed (step 3)
- But async callbacks are still queued in io_context (step 2)
- Callbacks fire and try to access the destroyed ConnectionInfo (step 4)

## Key Differences from Original Test

| Aspect | Original Test | Aggressive Test |
|--------|--------------|-----------------|
| Requests per client | 1 | 5 |
| Delay after send | 100μs | 0 (none) |
| Delay after destroy | 500μs | 0 (none) |
| Threading | Single | Multi-threaded |
| Iterations | 50 | 1000 |
| Retry attempts | 0 | 3 (keeps operations alive longer) |

## Troubleshooting

### No Crash After Many Iterations
Try increasing aggressiveness:
```bash
# More iterations
./http-client-aggressive-test 10000 8

# Run in a loop
for i in {1..100}; do
    echo "Run $i"
    ./http-client-aggressive-test 1000 4
done
```

### System Becomes Unresponsive
The test creates many async operations. If the system becomes slow:
- Reduce iterations: `./http-client-aggressive-test 500 2`
- Monitor system resources: `top`, `free -h`

### Build Errors
Ensure all dependencies are available:
```bash
# Check meson configuration
meson configure build

# Rebuild
cd build
ninja clean
ninja http-client-aggressive-test
```

## Comparison with Test 2

**Test 2** (`--test 2` in original reproducer):
- Creates 10 clients
- Sends 1 request per client
- Destroys all clients at once
- Good for testing concurrent destruction

**Aggressive Test**:
- Creates 1000+ clients sequentially
- Sends 5 requests per client
- Destroys each immediately
- Also runs multi-threaded phase
- Better for maximizing race condition probability

## Next Steps

If this test doesn't reproduce the crash:
1. The current codebase may already have the fix
2. Compare with the vulnerable version (ibm-ghe-1110)
3. Check if there are additional conditions needed (specific timing, system load, etc.)
4. Consider using tools like ThreadSanitizer or AddressSanitizer

## Related Files

- [`src/http_client_aggressive_test.cpp`](src/http_client_aggressive_test.cpp) - Test implementation
- [`src/http_client_coredump_reproducer.cpp`](src/http_client_coredump_reproducer.cpp) - Original test suite
- [`http/http_client.hpp`](http/http_client.hpp) - HTTP client implementation
- [`meson.build`](meson.build) - Build configuration