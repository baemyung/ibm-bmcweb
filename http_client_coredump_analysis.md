# HTTP Client Coredump Analysis

**Defect**: https://jazz07.rchland.ibm.com:13443/jazz/web/projects/FPS%20Collaboration#action=com.ibm.team.workitem.viewWorkItem&id=778179

**File**: [`http/http_client.hpp`](../http/http_client.hpp)

**Date**: 2026-04-28

---

## Executive Summary

Analysis of the `http_client.hpp` file reveals multiple potential causes for coredumps, with the most critical being use-after-free vulnerabilities in callback execution when connection pools are destroyed during active async operations.

---

## Potential Coredump Causes

### 1. **Use-After-Free in Callback Execution** ⚠️ CRITICAL

**Locations**: Lines 442, 490, 824

**Code Examples**:

Line 442 in `afterRead()`:
```cpp
callback(parser->keep_alive(), connId, res);
```

Line 490 in `waitAndRetry()`:
```cpp
callback(false, connId, res);
```

Line 824 in `afterSendData()`:
```cpp
resHandler(res);
```

**Problem**: 
- The callback member variable (line 154) holds a `std::function` that may capture references to objects in the `ConnectionPool` or `HttpClient`
- If the `ConnectionPool` or `HttpClient` is destroyed while async operations are pending, the callback may reference destroyed objects
- The `shared_from_this()` pattern keeps `ConnectionInfo` alive but doesn't protect objects captured by the callback

**Crash Scenario**:
1. HTTP request initiated, creating `ConnectionInfo` object
2. `ConnectionPool` or `HttpClient` destroyed (e.g., during shutdown)
3. Async operation completes and invokes callback
4. Callback accesses destroyed `ConnectionPool` members
5. **COREDUMP**

---

### 2. **Parser Dereferencing Without Complete Validation**

**Location**: Line 402

**Code**:
```cpp
BMCWEB_LOG_DEBUG("recvMessage() data: {}", parser->get().body().str());
```

**Problem**:
- Parser is dereferenced at line 402 for logging
- The check for `parser->is_done()` doesn't occur until line 410
- If parser is in an invalid state, dereferencing could crash

**Context**:
```cpp
398 | if (!parser)
399 | {
400 |     return;
401 | }
402 | BMCWEB_LOG_DEBUG("recvMessage() data: {}", parser->get().body().str());
403 | 
404 | unsigned int respCode = parser->get().result_int();
405 | BMCWEB_LOG_DEBUG("recvMessage() Header Response Code: {}", respCode);
406 | 
407 | // Handle the case of stream_truncated...
410 | if (!parser->is_done())
411 | {
412 |     state = ConnState::recvFailed;
413 |     waitAndRetry();
414 |     return;
415 | }
```

---

### 3. **SSL Connection Optional Dereferencing**

**Locations**: Lines 267, 310-311, 362-363

**Code Examples**:

Line 267 in `doSslHandshake()`:
```cpp
auto& ssl = *sslConn;
```

Lines 310-311 in `sendMessage()`:
```cpp
boost::beast::http::async_write(
    *sslConn, req,
    std::bind_front(&ConnectionInfo::afterWrite, this, shared_from_this()));
```

Lines 362-363 in `recvMessage()`:
```cpp
boost::beast::http::async_read(
    *sslConn, buffer, thisParser,
    std::bind_front(&ConnectionInfo::afterRead, this, shared_from_this()));
```

**Problem**:
- Direct dereferencing of `sslConn` optional without complete safety checks
- While there are checks before these operations (e.g., line 252, 308, 360), race conditions or state inconsistencies could lead to dereferencing an empty optional
- The optional is defined at line 164-165:
  ```cpp
  std::optional<boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>> sslConn;
  ```

---

### 4. **Connection Vector Access Without Bounds Check** ⚠️ HIGH RISK

**Location**: Line 705

**Code**:
```cpp
void sendNext(bool keepAlive, uint32_t connId)
{
    auto conn = connections[connId];
    // ...
}
```

**Problem**:
- No validation that `connId < connections.size()`
- If `connId` is corrupted or out of bounds, this causes undefined behavior
- The `connections` vector is defined at line 672:
  ```cpp
  std::vector<std::shared_ptr<ConnectionInfo>> connections;
  ```

**Risk**: Direct vector access without bounds checking can cause immediate segmentation fault

---

### 5. **Weak Pointer Lock Failure Handling**

**Locations**: Lines 461-465, 828-834

**Code Example** (lines 461-465 in `onTimeout()`):
```cpp
std::shared_ptr<ConnectionInfo> self = weakSelf.lock();
if (self == nullptr)
{
    return;
}
self->waitAndRetry();
```

**Code Example** (lines 828-834 in `afterSendData()`):
```cpp
std::shared_ptr<ConnectionPool> self = weakSelf.lock();
if (!self)
{
    BMCWEB_LOG_CRITICAL("{} Failed to capture connection", logPtr(self.get()));
    return;
}

self->sendNext(keepAlive, connId);
```

**Problem**:
- While the code checks for null after lock, there's a potential TOCTOU (Time-Of-Check-Time-Of-Use) issue
- If the object is destroyed between the lock check and method call, accessing member variables could crash
- The null check at line 831 uses `self.get()` which is already null, potentially causing issues in logging

---

### 6. **Timer Cancel Race Condition**

**Locations**: Lines 239, 286, 334, 387

**Code Examples**:
```cpp
timer.cancel();
```

**Problem**:
- Multiple locations cancel timers after async operations complete
- If the timer callback is already executing when `cancel()` is called, there could be a race condition
- Both the timeout handler and the success handler might try to access the same object state simultaneously

**Context**:
- Timer is defined at line 167: `boost::asio::steady_timer timer;`
- Timeout handler is at lines 446-467 (`onTimeout()`)
- Timer is set at lines 219, 269, 305, 356

---

## Root Cause Analysis

### Most Likely Cause: Use-After-Free in Callback Execution

**Detailed Scenario**:

1. **Setup Phase**:
   - `HttpClient` creates a `ConnectionPool` for a destination
   - `ConnectionPool` creates `ConnectionInfo` objects
   - Request is queued with a callback that captures references to pool/client objects

2. **Destruction Phase**:
   - Application shutdown or subscription deletion occurs
   - `HttpClient` or `ConnectionPool` destructor is called
   - Async operations are still pending (DNS resolution, TCP connect, SSL handshake, HTTP read/write)

3. **Crash Phase**:
   - Async operation completes
   - Callback is invoked (lines 442, 490, or 824)
   - Callback tries to access destroyed `ConnectionPool` or `HttpClient` members
   - **Segmentation fault / coredump**

**Why `shared_from_this()` Doesn't Prevent This**:
- `shared_from_this()` keeps the `ConnectionInfo` object alive
- However, the callback function object may capture raw pointers or references to the `ConnectionPool`
- When `ConnectionPool` is destroyed, these captured references become dangling

---

## Recommended Fixes

### Priority 1: Critical Fixes

1. **Add Bounds Checking for Connection Vector Access** (Line 705)
   ```cpp
   void sendNext(bool keepAlive, uint32_t connId)
   {
       if (connId >= connections.size())
       {
           BMCWEB_LOG_ERROR("Invalid connId: {} (size: {})", connId, connections.size());
           return;
       }
       auto conn = connections[connId];
       // ...
   }
   ```

2. **Improve Callback Lifetime Management**
   - Use `weak_ptr` to `ConnectionPool` in callbacks
   - Check if pool still exists before invoking user callbacks
   - Example for `afterSendData()`:
   ```cpp
   static void afterSendData(const std::weak_ptr<ConnectionPool>& weakSelf,
                             const std::function<void(Response&)>& resHandler,
                             bool keepAlive, uint32_t connId, Response& res)
   {
       std::shared_ptr<ConnectionPool> self = weakSelf.lock();
       if (!self)
       {
           BMCWEB_LOG_DEBUG("ConnectionPool destroyed, skipping callback");
           return;
       }
       
       resHandler(res);
       self->sendNext(keepAlive, connId);
   }
   ```

3. **Add Proper Cancellation in Destructors**
   - Implement `ConnectionInfo` destructor to cancel pending operations
   - Implement `ConnectionPool` destructor to clean up all connections
   - Set a flag to prevent new operations after destruction starts

### Priority 2: Important Fixes

4. **Validate Parser State Before All Dereferences**
   ```cpp
   if (!parser || !parser->is_done())
   {
       BMCWEB_LOG_ERROR("Parser in invalid state");
       state = ConnState::recvFailed;
       waitAndRetry();
       return;
   }
   BMCWEB_LOG_DEBUG("recvMessage() data: {}", parser->get().body().str());
   ```

5. **Add State Validation Before SSL Operations**
   ```cpp
   void doSslHandshake()
   {
       if (!sslConn || !sslConn->has_value())
       {
           BMCWEB_LOG_ERROR("SSL connection not initialized");
           state = ConnState::sslInitFailed;
           waitAndRetry();
           return;
       }
       // ...
   }
   ```

6. **Implement Proper Timer Cancellation Pattern**
   - Use a flag to track if operation completed
   - Check flag in timeout handler before proceeding
   - Use `try_emplace` or similar patterns to avoid race conditions

### Priority 3: Defensive Programming

7. **Add Connection State Validation**
   - Validate state transitions are legal
   - Add assertions or checks before state-dependent operations

8. **Implement Connection Pool Shutdown Method**
   - Gracefully close all connections
   - Cancel all pending requests
   - Prevent new requests during shutdown

9. **Add Comprehensive Logging**
   - Log object creation/destruction with addresses
   - Log all state transitions
   - Add correlation IDs to track request lifecycle

---

## Testing Recommendations

1. **Stress Testing**:
   - Rapid creation and destruction of `HttpClient` instances
   - High volume of concurrent requests
   - Simulate network delays and timeouts

2. **Shutdown Testing**:
   - Destroy `HttpClient` while requests are pending
   - Test with requests in various states (resolving, connecting, sending, receiving)
   - Verify clean shutdown without crashes

3. **Error Injection**:
   - Simulate DNS resolution failures
   - Simulate connection failures
   - Simulate SSL handshake failures
   - Test with invalid response data

4. **Memory Analysis**:
   - Run with AddressSanitizer (ASan)
   - Run with ThreadSanitizer (TSan)
   - Use Valgrind to detect memory leaks and use-after-free

---

## Additional Notes

- The code uses Boost.Asio async patterns which are generally safe, but require careful lifetime management
- The `shared_from_this()` pattern is used correctly for `ConnectionInfo`, but doesn't extend to captured callback objects
- The connection pooling mechanism adds complexity to lifetime management
- Consider using `std::enable_shared_from_this` for `ConnectionPool` as well

---

## References

- Boost.Asio documentation: https://www.boost.org/doc/libs/release/doc/html/boost_asio.html
- Boost.Beast HTTP client examples: https://www.boost.org/doc/libs/release/libs/beast/doc/html/beast/using_http.html
- C++ Core Guidelines on lifetime management: https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rf-lifetime