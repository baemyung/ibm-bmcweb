# HTTP Client Coredump Test Branches

This document describes two branches created to demonstrate the HTTP client coredump vulnerability and its fix.

## Branch Overview

### 1. `1120-test-reproduction-client-coredump-caused`
**Purpose:** Demonstrates the vulnerable code that causes coredumps

**Key Characteristics:**
- Contains the aggressive HTTP client test program
- Uses the **ORIGINAL** vulnerable `http/http_client.hpp` (without defensive fixes)
- Includes timeout mechanism for clean test exit
- **Expected Behavior:** Will crash with SIGABRT/SIGSEGV when run on BMC hardware

**Base Commit:** 9e7ea8e7 (Fix io_context concurrency issues in aggressive test)
- This is the commit BEFORE the HTTP client fix was applied
- Contains working aggressive test with proper io_context handling
- Does NOT contain the defensive checks in http_client.hpp

**Latest Commit:** 0b494bf6 (Add timeout mechanism for thread cleanup)

### 2. `1120-test-reproduction-client-coredump-fixed`
**Purpose:** Demonstrates the fixed code that prevents coredumps

**Key Characteristics:**
- Contains the aggressive HTTP client test program
- Uses the **FIXED** `http/http_client.hpp` with defensive checks
- Includes timeout mechanism for clean test exit
- **Expected Behavior:** Completes successfully without crashes

**Base Commit:** 0be53126 (Add timeout and detach for stuck IO threads to prevent hang)
- This is the latest commit with ALL fixes applied
- Contains HTTP client defensive fixes (commit 6fcad8be)
- Contains working aggressive test with proper shutdown

## The HTTP Client Fix

The key difference between the two branches is in `http/http_client.hpp`:

### Defensive Checks Added (in -fixed branch only):

1. **Bounds checking in `sendNext()` (~line 705):**
```cpp
if (connId >= connections.size()) {
    BMCWEB_LOG_ERROR("Invalid connId: {} (size: {})", connId, connections.size());
    return;
}
```

2. **Null pointer validation:**
```cpp
if (!conn) {
    BMCWEB_LOG_ERROR("Connection at index {} is null", connId);
    return;
}
```

3. **Reordered validation in `afterSendData()` (~line 824):**
```cpp
std::shared_ptr<ConnectionPool> self = weakSelf.lock();
if (!self) {
    BMCWEB_LOG_CRITICAL("Failed to capture connection");
    return;
}
resHandler(res);  // Now safe after validation
```

## Testing Instructions

### On BMC Hardware:

#### Test the VULNERABLE branch (should crash):
```bash
# Checkout the vulnerable branch
cd ~/Codes/ibm-ghe-1120/ibm-bmcweb
git fetch origin
git checkout 1120-test-reproduction-client-coredump-caused

# Rebuild bmcweb
cd ~/Codes/ibm-ghe-1120/openbmc/build/p10bmc
MACHINE=p10bmc bitbake bmcweb -c cleansstate
MACHINE=p10bmc bitbake bmcweb

# Deploy to BMC and run test
# Expected: SIGABRT or SIGSEGV crashes
```

#### Test the FIXED branch (should complete successfully):
```bash
# Checkout the fixed branch
cd ~/Codes/ibm-ghe-1120/ibm-bmcweb
git fetch origin
git checkout 1120-test-reproduction-client-coredump-fixed

# Rebuild bmcweb
cd ~/Codes/ibm-ghe-1120/openbmc/build/p10bmc
MACHINE=p10bmc bitbake bmcweb -c cleansstate
MACHINE=p10bmc bitbake bmcweb

# Deploy to BMC and run test
# Expected: Clean completion with "created 1000, destroyed 1000 clients"
```

## Commit History Comparison

### Branch: 1120-test-reproduction-client-coredump-caused
```
0b494bf6 Add timeout mechanism for thread cleanup (no HTTP client fix)
9e7ea8e7 Fix io_context concurrency issues in aggressive test
[... earlier commits ...]
```

### Branch: 1120-test-reproduction-client-coredump-fixed
```
0be53126 Add timeout and detach for stuck IO threads to prevent hang
3ac982eb Fix thread hang - use restart/stop pattern to wake blocked threads
5b789c2a Simplify shutdown - call ioc.stop() immediately to cancel pending operations
def1fad5 Fix test hang on exit - ensure all io_context threads stop
9e7ea8e7 Fix io_context concurrency issues in aggressive test
6fcad8be Fix HTTP client coredump vulnerability with defensive checks  <-- KEY FIX
[... earlier commits ...]
```

## Key Files

Both branches contain:
- `src/http_client_aggressive_test.cpp` - The aggressive test program
- `meson.build` - Build configuration for the test
- `AGGRESSIVE_CRASH_TEST_INSTRUCTIONS.md` - Test execution instructions

Only the `-fixed` branch has the defensive fixes in:
- `http/http_client.hpp` - HTTP client implementation with defensive checks

## Summary

These two branches provide a clear before/after comparison:
- **-caused**: Reproduces the vulnerability (crashes expected)
- **-fixed**: Demonstrates the fix (clean completion expected)

Both branches use the same aggressive test program, making it easy to verify that the HTTP client fixes resolve the coredump issue.