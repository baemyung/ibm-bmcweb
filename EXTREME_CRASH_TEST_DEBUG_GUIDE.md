# HTTP Client Extreme Crash Test - Debugging Guide

## Why the Aggressive Test Might Not Crash

The aggressive crash test may not reproduce the coredump for several reasons:

### 1. **The Fix May Already Be Applied**
If the current codebase already has defensive checks in [`http/http_client.hpp`](http/http_client.hpp), the crash won't occur. Check for:
- Bounds checking at line ~705 in `sendNext()`
- Null pointer validation after vector access
- Pool validation before callback execution at line ~824 in `afterSendData()`

### 2. **Race Window is Too Narrow**
The race condition requires:
- Async operations (DNS resolve, connect, timers) to be queued
- Client destruction to occur while callbacks are pending
- Callbacks to fire AFTER destruction but BEFORE cleanup

This timing window can be microseconds wide.

### 3. **Async Operations Complete Too Quickly**
If DNS resolution or connection attempts fail immediately (before destruction), no callbacks are queued, so no race occurs.

---

## New Extreme Test Improvements

The [`src/http_client_extreme_test.cpp`](src/http_client_extreme_test.cpp) test adds:

1. **Shorter retry intervals** (5-10ms vs 1000ms) - callbacks fire faster
2. **More requests per client** (8-10 vs 5) - more async operations
3. **Multiple io_context threads** (2 threads) - more contention
4. **Synchronized thread start** - all threads start simultaneously
5. **Busy work injection** - keeps io_context queue full
6. **Signal handlers** - detects crashes immediately

---

## Building and Running

### Build the Extreme Test

```bash
# From bmcweb directory
meson setup build
cd build
ninja http-client-extreme-test
```

### Run with Default Settings

```bash
./http-client-extreme-test
```

### Run with Custom Parameters

```bash
# Syntax: ./http-client-extreme-test [iterations] [threads]
./http-client-extreme-test 1000 8
```

### Run in a Loop (Increase Probability)

```bash
for i in {1..100}; do
    echo "=== Run $i ==="
    ./http-client-extreme-test 500 8
    if [ $? -ne 0 ]; then
        echo "CRASH DETECTED in run $i"
        break
    fi
done
```

---

## Advanced Debugging Techniques

### Method 1: AddressSanitizer (HIGHLY RECOMMENDED)

ASan detects use-after-free **immediately** without waiting for a crash:

```bash
# Build with ASan
meson setup build-asan -Db_sanitize=address -Db_lundef=false
cd build-asan
ninja http-client-extreme-test

# Run the test
./http-client-extreme-test 200 4
```

**Expected output if bug exists:**
```
=================================================================
==12345==ERROR: AddressSanitizer: heap-use-after-free on address 0x...
READ of size 8 at 0x... thread T1
    #0 in crow::ConnectionPool::sendNext() http/http_client.hpp:705
    #1 in crow::ConnectionPool::afterSendData() http/http_client.hpp:836
    
0x... is located 0 bytes inside of 128-byte region
freed by thread T2 here:
    #0 in operator delete(void*)
    #1 in std::shared_ptr<crow::ConnectionPool>::~shared_ptr()
```

### Method 2: ThreadSanitizer

TSan detects race conditions:

```bash
# Build with TSan
meson setup build-tsan -Db_sanitize=thread -Db_lundef=false
cd build-tsan
ninja http-client-extreme-test

# Run the test
./http-client-extreme-test 200 4
```

**Expected output if race exists:**
```
==================
WARNING: ThreadSanitizer: data race (pid=12345)
  Write of size 8 at 0x... by thread T1:
    #0 crow::ConnectionPool::~ConnectionPool()
    
  Previous read of size 8 at 0x... by thread T2:
    #0 crow::ConnectionPool::sendNext()
```

### Method 3: Valgrind (Slower but Thorough)

```bash
# Build without optimizations
meson setup build-debug -Dbuildtype=debug
cd build-debug
ninja http-client-extreme-test

# Run with Valgrind
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind.log \
         ./http-client-extreme-test 100 2

# Check for errors
grep -A 10 "Invalid read\|Invalid write\|use after free" valgrind.log
```

### Method 4: GDB with Breakpoints

```bash
# Build with debug symbols
meson setup build-debug -Dbuildtype=debug
cd build-debug
ninja http-client-extreme-test

# Run with GDB
gdb ./http-client-extreme-test

# In GDB, set breakpoints
(gdb) break crow::ConnectionInfo::~ConnectionInfo
(gdb) break crow::ConnectionPool::afterSendData
(gdb) break crow::ConnectionPool::sendNext
(gdb) run 100 4

# When it crashes
(gdb) bt full
(gdb) info threads
(gdb) thread apply all bt
```

### Method 5: Core Dump Analysis

Enable core dumps:
```bash
# Set unlimited core dump size
ulimit -c unlimited

# Run the test
./http-client-extreme-test 1000 8

# If it crashes, analyze the core
gdb ./http-client-extreme-test core
(gdb) bt full
(gdb) info threads
(gdb) thread apply all bt
```

---

## Interpreting Results

### Scenario 1: No Crash, No ASan Errors

**Likely causes:**
1. ✅ **Fix is already applied** - Check [`http/http_client.hpp`](http/http_client.hpp:705) for defensive checks
2. ⚠️ **Race is extremely timing-sensitive** - Try on different hardware
3. ⚠️ **Additional conditions needed** - May require specific network timing

**Next steps:**
- Compare with vulnerable version (ibm-ghe-1110)
- Check if defensive fixes are present
- Try on BMC hardware (different timing characteristics)

### Scenario 2: Crash Detected

**Confirms vulnerability exists!**

Check crash location:
```bash
# Check dmesg
dmesg | tail -50 | grep -i "segfault\|general protection"

# Check coredump
coredumpctl list
coredumpctl info
coredumpctl gdb
```

**In GDB:**
```
(gdb) bt
(gdb) frame 0
(gdb) print connId
(gdb) print connections.size()
(gdb) print conn
```

### Scenario 3: ASan Detects Issue

**Best case - precise detection!**

ASan output shows:
- Exact line where use-after-free occurs
- Stack trace of allocation
- Stack trace of deallocation
- Stack trace of invalid access

This pinpoints the exact vulnerability location.

### Scenario 4: TSan Detects Race

**Race condition confirmed!**

TSan output shows:
- Which threads are racing
- What memory location is being accessed
- Stack traces of both racing operations

---

## Comparing with Vulnerable Version

### Check if Fix is Present

Look for these defensive checks in [`http/http_client.hpp`](http/http_client.hpp):

**Fix 1: Bounds checking (around line 705)**
```cpp
void sendNext(bool keepAlive, uint32_t connId)
{
    // Should have this check:
    if (connId >= connections.size())
    {
        BMCWEB_LOG_ERROR("Invalid connId: {} (size: {})", connId, connections.size());
        return;
    }
    
    auto conn = connections[connId];
    
    // Should have this check:
    if (!conn)
    {
        BMCWEB_LOG_ERROR("Connection at index {} is null", connId);
        return;
    }
    // ...
}
```

**Fix 2: Pool validation before callback (around line 824)**
```cpp
void afterSendData(...)
{
    // Should validate pool FIRST:
    std::shared_ptr<ConnectionPool> self = weakSelf.lock();
    if (!self)
    {
        BMCWEB_LOG_CRITICAL("Failed to capture connection");
        return;
    }
    
    // Then execute callback:
    resHandler(res);
    
    self->sendNext(keepAlive, connId);
}
```

### If Fixes Are Missing

The vulnerability exists. Apply the fixes from the analysis documents:
- [`http_client_coredump_analysis_1120.md`](http_client_coredump_analysis_1120.md)
- [`COREDUMP_REPRODUCTION_LIMITATIONS.md`](COREDUMP_REPRODUCTION_LIMITATIONS.md)

---

## Why This Test is More Aggressive

| Feature | Aggressive Test | Extreme Test |
|---------|----------------|--------------|
| Requests per client | 5 | 8-10 |
| Retry interval | 1000ms | 5-10ms |
| IO threads | 1 | 2 |
| Thread synchronization | No | Yes (simultaneous start) |
| Busy work injection | No | Yes |
| Signal handlers | No | Yes |
| Default iterations | 1000 | 500 (but more intense) |

The extreme test creates **more callbacks in less time**, increasing the probability that callbacks fire after destruction.

---

## On-BMC Testing

The test is most likely to reproduce on actual BMC hardware due to:
- Different CPU timing characteristics
- Real network stack behavior
- System load variations

```bash
# Copy to BMC
scp build/http-client-extreme-test root@<bmc-ip>:/tmp/

# SSH to BMC
ssh root@<bmc-ip>

# Run the test
/tmp/http-client-extreme-test 1000 8

# Check for crashes
dmesg | tail -50
coredumpctl list
```

---

## Monitoring During Test

### Terminal 1: Run the test
```bash
./http-client-extreme-test 1000 8
```

### Terminal 2: Monitor system
```bash
# Watch for segfaults
watch -n 1 'dmesg | tail -20 | grep -i segfault'

# Or monitor coredumps
watch -n 1 'coredumpctl list | tail -5'
```

### Terminal 3: Monitor process
```bash
# Watch the process
watch -n 1 'ps aux | grep http-client-extreme-test'
```

---

## Conclusion

If the extreme test still doesn't crash:

1. **Use AddressSanitizer** - This is the most reliable detection method
2. **Check for existing fixes** - The code may already be patched
3. **Compare with vulnerable version** - Test on ibm-ghe-1110 branch
4. **Try on BMC hardware** - Different timing may trigger the race
5. **Use ThreadSanitizer** - Detects races even without crashes

The absence of crashes combined with ASan showing no errors strongly suggests the fix is already applied.

---

## Related Files

- [`src/http_client_extreme_test.cpp`](src/http_client_extreme_test.cpp) - Extreme test implementation
- [`src/http_client_aggressive_test.cpp`](src/http_client_aggressive_test.cpp) - Original aggressive test
- [`AGGRESSIVE_CRASH_TEST_INSTRUCTIONS.md`](AGGRESSIVE_CRASH_TEST_INSTRUCTIONS.md) - Original test instructions
- [`http/http_client.hpp`](http/http_client.hpp) - HTTP client implementation
- [`http_client_coredump_analysis_1120.md`](http_client_coredump_analysis_1120.md) - Vulnerability analysis