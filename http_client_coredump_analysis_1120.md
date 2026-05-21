# HTTP Client Coredump Analysis - Branch 1120

**Branch**: 1120  
**File**: [`http/http_client.hpp`](http/http_client.hpp)

---

## Executive Summary

Branch 1120 contains HTTP client coredump vulnerabilities that can lead to segmentation faults during rapid connection destruction. Analysis reveals critical race conditions in async callback execution when ConnectionPool objects are destroyed.

---

## Vulnerability Analysis

### 1. Out-of-Bounds Vector Access (Line 705)

**Code:**
```cpp
void sendNext(bool keepAlive, uint32_t connId)
{
    auto conn = connections[connId];  // Line 705 - NO BOUNDS CHECK
```

**Problem:**
- Direct vector access without validating `connId < connections.size()`
- If `connId` is corrupted or out of bounds → immediate segmentation fault
- Race condition: connection may be removed from vector while `connId` is still referenced

**Crash Scenario:**
1. Async operation holds `connId = 5`
2. ConnectionPool shrinks, `connections.size()` becomes 3
3. Callback fires with `connId = 5`
4. `connections[5]` → **SEGFAULT**

---

### 2. Callback Execution Before Pool Validation (Line 824)

**Code:**
```cpp
void afterSendData(const std::function<void(Response&)>& resHandler,
                  bool keepAlive, uint32_t connId, Response& res)
{
    // Allow provided callback to perform additional processing of the
    // request
    resHandler(res);  // Line 824 - CALLED FIRST

    // If requests remain in the queue then we want to reuse this
    // connection to send the next request
    std::shared_ptr<ConnectionPool> self = weakSelf.lock();  // Line 828
    if (!self)
    {
        BMCWEB_LOG_CRITICAL("{} Failed to capture connection",
                            logPtr(self.get()));
        return;
    }

    self->sendNext(keepAlive, connId);  // Line 836
}
```

**Problem:**
- `resHandler(res)` executes **before** checking if ConnectionPool still exists
- If ConnectionPool is destroyed between callback start and pool validation
- Callback may access freed memory or trigger cascading failures

**Crash Scenario:**
1. HTTP request completes, `afterSendData()` called
2. `resHandler(res)` executes (line 824)
3. During callback execution, ConnectionPool is destroyed
4. `weakSelf.lock()` returns null (line 828)
5. But damage may already be done if callback accessed pool members
6. **POTENTIAL COREDUMP** depending on what callback does

---

### 3. Null Pointer Dereference Risk

**Code:**
```cpp
auto conn = connections[connId];  // Line 705

// No null check here!
conn->callback = nullptr;  // Line 710 - IMMEDIATE DEREFERENCE
```

**Problem:**
- After vector access, `conn` is immediately dereferenced without null check
- If `connections[connId]` returns null (e.g., connection was removed)
- Direct crash on `conn->callback`

---

## Root Cause

The fundamental issue is **object lifecycle management** in async operations:

1. **Async callbacks** hold references to connection IDs and weak pointers
2. **ConnectionPool destruction** can occur while callbacks are pending
3. **No defensive checks** prevent accessing destroyed or invalid objects
4. **Race conditions** between object destruction and callback execution

---

## Reproduction Methods

### Method 1: C++ Unit Test (Most Reliable)

```cpp
#include "http/http_client.hpp"
#include <boost/asio/io_context.hpp>
#include <thread>
#include <chrono>

void reproduceCoredump1120() {
    boost::asio::io_context ioc;
    std::thread ioThread([&ioc]() { ioc.run(); });
    
    for (int i = 0; i < 100; i++) {
        auto policy = std::make_shared<crow::ConnectionPolicy>();
        auto client = std::make_unique<crow::HttpClient>(ioc, policy);
        
        // Send request to non-routable IP
        boost::beast::http::fields headers;
        client->sendData(
            "test",
            boost::urls::url_view("http://192.0.2.1:8080/test"),
            crow::ensuressl::VerifyCertificate::Verify,
            headers,
            boost::beast::http::verb::post
        );
        
        // Destroy client while async operations pending
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        client.reset(); // COREDUMP HERE
    }
    
    ioc.stop();
    ioThread.join();
}
```

### Method 2: BMCWeb Shutdown During Aggregation

1. Configure Redfish aggregation with satellites
2. Start aggregation operations
3. Immediately: `systemctl restart bmcweb`
4. Shutdown destroys HttpClient while connections active

### Method 3: Python Event Subscription Test (Limited)

Use the provided Python scripts:
- `test_http_client_coredump.py`
- `test_http_client_coredump_server_side.py`

**Note:** Python scripts have limited effectiveness because they cannot control BMCWeb's internal object lifecycle.

---

## Required Fixes

Three defensive fixes are needed:

### Fix 1: Add Bounds Checking (Line 705)

```cpp
void sendNext(bool keepAlive, uint32_t connId)
{
    // Defensive check: Validate connId is within bounds
    if (connId >= connections.size())
    {
        BMCWEB_LOG_ERROR("Invalid connId: {} (size: {})", connId,
                         connections.size());
        return;
    }

    auto conn = connections[connId];

    // Defensive check: Validate connection pointer is not null
    if (!conn)
    {
        BMCWEB_LOG_ERROR("Connection at index {} is null", connId);
        return;
    }
    
    // ... rest of function
}
```

### Fix 2: Reorder afterSendData() (Line 824)

```cpp
void afterSendData(const std::function<void(Response&)>& resHandler,
                  bool keepAlive, uint32_t connId, Response& res)
{
    // Validate pool exists BEFORE executing callback
    std::shared_ptr<ConnectionPool> self = weakSelf.lock();
    if (!self)
    {
        BMCWEB_LOG_CRITICAL("Failed to capture connection");
        return;
    }

    // Now safe to execute callback
    resHandler(res);

    self->sendNext(keepAlive, connId);
}
```

---

## Verification

### Before Fix (Expected)
```bash
# Run C++ unit test
meson test -C build http_client_coredump_test

# Check for coredump
dmesg | tail -20
# Should show: bmcweb[PID]: segfault at ADDRESS
```

### After Fix (Expected)
```bash
# Run same test
meson test -C build http_client_coredump_test

# Check logs for defensive messages
journalctl -u bmcweb | grep -E "Invalid connId|Connection at index.*is null|Failed to capture"

# No coredumps
dmesg | grep segfault
# Should be empty
```

---

## Conclusion

Branch 1120 has critical HTTP client coredump vulnerabilities that require defensive fixes. The same three fixes applied to branch 1110 are needed here.

The fix branch `1120-fix-http-client-coredump-opt-bandaid-approach` (commit 041597dc) contains the necessary defensive checks to address these vulnerabilities.