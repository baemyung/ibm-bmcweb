# HTTP Client Race Condition - Root Cause Analysis

## Executive Summary

The coredump occurs due to a race condition where `ConnectionInfo::callback` is invoked after the `ConnectionPool::connections` vector has been destroyed or is in an invalid state during HttpClient destruction.

## The Race Condition Flow

### Normal Flow (No Race)
1. HttpClient creates ConnectionPool with shared_ptr
2. ConnectionPool creates ConnectionInfo objects stored in `connections` vector
3. ConnectionInfo starts async operations with `shared_from_this()` 
4. Async operation completes → callback invoked → `afterSendData` → `sendNext`
5. `sendNext` accesses `connections[connId]` safely
6. Eventually all operations complete and objects are destroyed cleanly

### Race Condition Flow (Crash)
1. HttpClient is destroyed while async operations are pending
2. `connectionPools` map is destroyed
3. `shared_ptr<ConnectionPool>` refcount decreases
4. **BUT** ConnectionInfo objects are still alive (kept by `shared_from_this()` in async handlers)
5. **AND** ConnectionInfo holds callback with captured `weak_ptr<ConnectionPool>`
6. Async operation completes (e.g., timer, socket read)
7. Callback is invoked: `callback(keepAlive, connId, res)` at line 413
8. This calls `afterSendData` (line 812-832)
9. `weakSelf.lock()` at line 818 **SUCCEEDS** because:
   - Other ConnectionInfo objects may still hold references
   - The callback itself may be keeping the pool alive temporarily
10. `self->sendNext(keepAlive, connId)` is called at line 831
11. `sendNext` accesses `connections[connId]` at line 692
12. **CRASH**: `connections` vector is destroyed or in invalid state

## Code Locations

### Where callback is created (with weak_ptr protection)
```cpp
// Line 755-756 in http_client.hpp
auto cb = std::bind_front(&ConnectionPool::afterSendData,
                          weak_from_this(), resHandler);
```

### Where callback is stored in ConnectionInfo
```cpp
// Line 766, 791, 798 in http_client.hpp
conn->callback = std::move(cb);
```

### Where callback is invoked from ConnectionInfo
```cpp
// Line 413 in http_client.hpp (in recvMessage)
callback(parser->keep_alive(), connId, res);
```

### Where weak_ptr is checked (insufficient protection)
```cpp
// Line 812-832 in http_client.hpp
static void afterSendData(const std::weak_ptr<ConnectionPool>& weakSelf,
                          const std::function<void(Response&)>& resHandler,
                          bool keepAlive, uint32_t connId, Response& res)
{
    std::shared_ptr<ConnectionPool> self = weakSelf.lock();
    if (!self)
    {
        BMCWEB_LOG_CRITICAL("Failed to capture connection");
        return;
    }
    resHandler(res);
    self->sendNext(keepAlive, connId);  // ← PROBLEM: accesses connections vector
}
```

### Where the crash occurs
```cpp
// Line 682-699 in http_client.hpp
void sendNext(bool keepAlive, uint32_t connId)
{
    if (connId >= connections.size())  // ← May pass this check
    {
        BMCWEB_LOG_ERROR("Invalid connId: {} (size: {})", connId,
                         connections.size());
        return;
    }
    
    auto conn = connections[connId];  // ← CRASH HERE: vector destroyed
    
    if (!conn)  // ← Or crash here if conn is invalid
    {
        BMCWEB_LOG_ERROR("Connection at index {} is null", connId);
        return;
    }
    // ...
}
```

## Why shared_from_this() Alone Is Not Enough

The current fix uses `shared_from_this()` in all async operations to keep ConnectionInfo alive. This prevents ConnectionInfo from being destroyed while async operations are pending.

**However**, this does NOT prevent the ConnectionPool from being destroyed because:

1. ConnectionInfo is kept alive by `shared_from_this()` in async handlers
2. ConnectionInfo holds a callback that captures `weak_ptr<ConnectionPool>`
3. When the async operation completes, the callback is invoked
4. The callback checks if ConnectionPool exists via `weak_ptr::lock()`
5. **The check may succeed** if other ConnectionInfo objects are keeping the pool alive
6. But the `connections` vector may already be destroyed or in an invalid state
7. Accessing `connections[connId]` causes undefined behavior

## The Missing Protection

The issue is that `sendNext()` assumes the `connections` vector is valid when called. The defensive checks at lines 684-699 are insufficient because:

1. `connId >= connections.size()` check may pass if vector is partially destroyed
2. `connections[connId]` access may return garbage if vector memory is freed
3. The null check `if (!conn)` happens AFTER the potentially invalid access

## Solution Options

### Option 1: Add State Flag to ConnectionPool (Recommended)
Add a boolean flag to track if ConnectionPool is being destroyed:

```cpp
class ConnectionPool : public std::enable_shared_from_this<ConnectionPool>
{
private:
    std::atomic<bool> isShuttingDown{false};
    
public:
    ~ConnectionPool()
    {
        isShuttingDown = true;
        // Existing cleanup
    }
    
    void sendNext(bool keepAlive, uint32_t connId)
    {
        if (isShuttingDown)
        {
            BMCWEB_LOG_DEBUG("Pool shutting down, ignoring sendNext");
            return;
        }
        // Rest of existing code
    }
};
```

### Option 2: Make connections Access Thread-Safe
Use mutex to protect connections vector access:

```cpp
class ConnectionPool
{
private:
    std::mutex connectionsMutex;
    std::vector<std::shared_ptr<ConnectionInfo>> connections;
    
public:
    void sendNext(bool keepAlive, uint32_t connId)
    {
        std::lock_guard<std::mutex> lock(connectionsMutex);
        if (connId >= connections.size())
        {
            return;
        }
        auto conn = connections[connId];
        // ...
    }
};
```

### Option 3: Store weak_ptr in ConnectionInfo callback
Instead of storing the callback directly, store a weak_ptr to the pool and validate before each use:

```cpp
// In ConnectionInfo
std::weak_ptr<ConnectionPool> poolRef;
std::function<void(Response&)> userCallback;

// In recvMessage
if (auto pool = poolRef.lock())
{
    pool->handleResponse(keepAlive, connId, res, userCallback);
}
```

## Recommended Fix

**Option 1** is the simplest and most effective:
1. Add `std::atomic<bool> isShuttingDown` to ConnectionPool
2. Set it to `true` in destructor
3. Check it at the start of `sendNext()` and return early if true
4. This prevents any access to `connections` during/after destruction

## Testing Strategy

To verify the fix:
1. Run `http_client_aggressive_test` which rapidly creates/destroys HttpClient
2. Use AddressSanitizer (ASAN) to detect use-after-free
3. Use ThreadSanitizer (TSAN) to detect data races
4. Run under high load with many concurrent connections
5. Verify no crashes occur after 10,000+ iterations

## Related Files

- `http/http_client.hpp` - Main implementation
- `src/http_client_aggressive_test.cpp` - Reproduction test
- `test/http/http_client_race_test.cpp` - Unit test for race conditions