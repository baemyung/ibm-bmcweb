# The Actual Bug: Raw `this` Pointer in Async Callbacks

## Summary
The coredump is caused by using raw `this` pointers in `std::bind_front()` for async operations. When the `ConnectionInfo` object is destroyed (e.g., when `HttpClient` is destroyed during a callback), the `this` pointer becomes dangling, but async operations still try to invoke callbacks on it.

## The Bug Pattern

### ❌ WRONG (Current 1110 Code)
```cpp
resolver.async_resolve(host.encoded_host_address(), host.port(),
                       std::bind_front(&ConnectionInfo::afterResolve,
                                       this, shared_from_this()));
```

### ✅ CORRECT
```cpp
resolver.async_resolve(host.encoded_host_address(), host.port(),
                       std::bind_front(&ConnectionInfo::afterResolve,
                                       shared_from_this()));
```

## Why This Causes Coredumps

1. **Async operation starts**: `async_resolve()` is called with a callback bound to `this`
2. **Object destroyed**: `HttpClient` (and its `ConnectionPool`) is destroyed
3. **`this` becomes dangling**: The raw pointer in the callback now points to freed memory
4. **Callback invoked**: When DNS resolution completes, it tries to call `afterResolve()` on the dangling `this`
5. **SEGFAULT**: Accessing freed memory causes segmentation fault

## Affected Locations in http/http_client.hpp (1110 branch)

### 1. Line 169-170: `doResolve()`
```cpp
resolver.async_resolve(host.encoded_host_address(), host.port(),
                       std::bind_front(&ConnectionInfo::afterResolve,
                                       this, shared_from_this()));  // ❌ BUG
```

### 2. Line 194-195: `afterResolve()`
```cpp
boost::asio::async_connect(
    conn, endpointList,
    std::bind_front(&ConnectionInfo::afterConnect, this,
                    shared_from_this()));  // ❌ BUG
```

### 3. Line 242-243: `doSslHandshake()`
```cpp
sslConn->async_handshake(
    boost::asio::ssl::stream_base::client,
    std::bind_front(&ConnectionInfo::afterSslHandshake, this,
                    shared_from_this()));  // ❌ BUG
```

### 4. Line 335-336: `sendMessage()` - SSL read
```cpp
boost::beast::http::async_read(
    *sslConn, buffer, thisParser,
    std::bind_front(&ConnectionInfo::afterRead, this,
                    shared_from_this()));  // ❌ BUG
```

### 5. Line 342-343: `sendMessage()` - non-SSL read
```cpp
boost::beast::http::async_read(
    conn, buffer, thisParser,
    std::bind_front(&ConnectionInfo::afterRead, this,
                    shared_from_this()));  // ❌ BUG
```

## Why `shared_from_this()` Alone is Sufficient

When you use:
```cpp
std::bind_front(&ConnectionInfo::method, shared_from_this())
```

The `shared_from_this()` is evaluated **at bind time** and stored in the bound function object. This keeps the `ConnectionInfo` alive as long as the callback exists. When the callback is invoked, `std::bind_front` automatically passes the stored `shared_ptr` as the implicit `this` parameter.

## Test Reproduction

The test `http-client-pool-destruction-test` successfully reproduces this bug:

```
./http-client-pool-destruction-test
=== ConnectionPool Destruction During Callback Test ===
Creating HTTP client...
Sending request...
Segmentation fault (core dumped)
```

The crash happens immediately during DNS resolution because:
1. Test creates `HttpClient` and calls `sendDataWithCallback()`
2. `ConnectionPool::sendData()` creates a `ConnectionInfo` and calls `doResolve()`
3. `doResolve()` starts async DNS resolution with callback bound to raw `this`
4. Test's callback destroys `globalClient` (and its `ConnectionPool`)
5. `ConnectionInfo` is destroyed, `this` becomes dangling
6. DNS resolution completes and tries to call `afterResolve()` on dangling `this`
7. **SEGFAULT**

## The Fix

Remove the raw `this` pointer from all FIVE `std::bind_front()` calls:

```cpp
// Fix 1: doResolve()
resolver.async_resolve(host.encoded_host_address(), host.port(),
                       std::bind_front(&ConnectionInfo::afterResolve,
                                       shared_from_this()));

// Fix 2: afterResolve()
boost::asio::async_connect(
    conn, endpointList,
    std::bind_front(&ConnectionInfo::afterConnect,
                    shared_from_this()));

// Fix 3: doSslHandshake()
sslConn->async_handshake(
    boost::asio::ssl::stream_base::client,
    std::bind_front(&ConnectionInfo::afterSslHandshake,
                    shared_from_this()));

// Fix 4: sendMessage() - SSL read
boost::beast::http::async_read(
    *sslConn, buffer, thisParser,
    std::bind_front(&ConnectionInfo::afterRead,
                    shared_from_this()));

// Fix 5: sendMessage() - non-SSL read
boost::beast::http::async_read(
    conn, buffer, thisParser,
    std::bind_front(&ConnectionInfo::afterRead,
                    shared_from_this()));
```

## Why Previous Analysis Was Wrong

The previous analysis focused on `ConnectionPool::afterSendData()` which correctly uses `weak_ptr`. However, the bug is actually in the **earlier stages** of connection establishment (DNS resolution, TCP connect, SSL handshake) where raw `this` pointers are used.

The `weak_ptr` protection in `afterSendData()` is correct and necessary, but it doesn't help if the object is already destroyed before we even get to the send phase.

## Conclusion

This is a **use-after-free bug** caused by binding raw `this` pointers to async callbacks. The fix is simple: remove the raw `this` and let `shared_from_this()` handle object lifetime correctly.