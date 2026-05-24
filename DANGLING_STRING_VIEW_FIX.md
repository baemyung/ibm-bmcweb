# Dangling string_view Bug in async_resolve()

## Critical Discovery

After fixing all 9 use-after-free bugs in http_client.hpp, the test still crashed. Investigation revealed a **separate critical bug** in [`async_resolve.hpp`](include/async_resolve.hpp:1).

## Root Cause

The `async_resolve()` function signature:
```cpp
void async_resolve(std::string_view host, std::string_view port,
                   ResolveHandler&& handler)
```

Was called from [`http_client.hpp`](http/http_client.hpp:169) like this:
```cpp
resolver.async_resolve(host.encoded_host_address(), host.port(),
                       std::bind_front(&ConnectionInfo::afterResolve,
                                       shared_from_this()));
```

**The Problem:**
- `host.encoded_host_address()` returns a **temporary** `string_view`
- `host.port()` returns a **temporary** `string_view`
- These temporaries are destroyed immediately after the `async_resolve()` call
- But `async_resolve()` tries to use them in `BMCWEB_LOG_DEBUG` at line 67
- **Result**: Accessing freed memory → Segmentation fault

## Stack Trace Evidence

```
[DEBUG async_resolve.hpp:67] Trying to resolve: localhost:9999
Segmentation fault (core dumped)
```

The crash happened at line 67 when trying to format the dangling `string_view` parameters.

## The Fix

Changed [`async_resolve.hpp`](include/async_resolve.hpp:64) to copy the string_views immediately:

```cpp
void async_resolve(std::string_view host, std::string_view port,
                   ResolveHandler&& handler)
{
    // Copy string_views to strings immediately to avoid dangling references
    std::string hostStr(host);
    std::string portStr(port);
    
    BMCWEB_LOG_DEBUG("Trying to resolve: {}:{}", hostStr, portStr);

    uint16_t portNum = 0;

    auto it = std::from_chars(&*portStr.begin(), &*portStr.end(), portNum);
    // ... rest of function uses hostStr and portStr
    
    // Move hostStr into lambda capture (instead of copying again)
    crow::connections::systemBus->async_method_call(
        [host{std::move(hostStr)}, portNum,
         handler = std::forward<ResolveHandler>(handler)](...) {
            // Lambda body
        }, ...);
}
```

## Why This Matters

This bug affected **ALL HTTP client connections** because:
1. Every connection starts with DNS resolution
2. DNS resolution immediately crashes due to dangling string_views
3. No connection could ever succeed

## Lesson Learned

**String_view parameters are dangerous when:**
- They point to temporary objects
- The function needs to use them after the call returns (even in logging)
- The function stores them for async operations

**Solution**: Copy to `std::string` immediately if the data needs to outlive the function call.

## Related Bugs

This is separate from but related to the 9 use-after-free bugs in http_client.hpp:
- Those bugs: Raw `this` pointers in async callbacks
- This bug: Dangling `string_view` references to temporary objects

Both are lifetime management issues, but with different root causes.