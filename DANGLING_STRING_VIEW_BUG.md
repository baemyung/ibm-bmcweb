# The Dangling String_View Bug in async_resolve

## The Crash Location

```
[DEBUG async_resolve.hpp:67] Trying to resolve: 192.0.2.1:8080
Segmentation fault (core dumped)
```

The crash was happening at line 67 in `async_resolve.hpp` during the logging statement.

## Root Cause

### The Problem

In `http_client.hpp`, the `doResolve()` method calls:

```cpp
resolver.async_resolve(
    host.encoded_host_address(), host.port(),
    [self](const boost::system::error_code& ec,
           const Resolver::results_type& endpointList) {
        self->afterResolve(self, ec, endpointList);
    });
```

**The Issue:**
1. `host.encoded_host_address()` returns a **temporary** `boost::urls::pct_string_view`
2. `host.port()` returns a **temporary** `boost::core::string_view`
3. These temporaries are implicitly converted to `std::string_view` parameters in `async_resolve()`
4. **The temporaries are destroyed immediately after the function call**
5. The `std::string_view` parameters now point to **deallocated memory**

### In async_resolve.hpp

```cpp
void async_resolve(std::string_view host, std::string_view port,
                   ResolveHandler&& handler)
{
    BMCWEB_LOG_DEBUG("Trying to resolve: {}:{}", host, port);  // CRASH HERE!
    
    uint16_t portNum = 0;
    auto it = std::from_chars(&*port.begin(), &*port.end(), portNum);  // Would also crash
    // ...
}
```

When line 67 tries to access `host` and `port` for logging, they're **dangling pointers** pointing to the destroyed temporary objects.

## The Sequence of Events

1. `doResolve()` calls `resolver.async_resolve(host.encoded_host_address(), host.port(), ...)`
2. Temporary objects are created for `encoded_host_address()` and `port()`
3. These are passed as `std::string_view` parameters to `async_resolve()`
4. **The temporary objects are destroyed when the expression completes**
5. Inside `async_resolve()`, the `host` and `port` string_views now point to freed memory
6. Line 67 tries to log them → **SEGFAULT**

## The Fix

Convert the `std::string_view` parameters to `std::string` immediately at the start of `async_resolve()`:

```cpp
void async_resolve(std::string_view host, std::string_view port,
                   ResolveHandler&& handler)
{
    // Convert string_views to strings immediately to avoid dangling references
    // when the caller's temporary objects (like boost::urls::url members) are destroyed
    std::string hostStr(host);
    std::string portStr(port);
    
    BMCWEB_LOG_DEBUG("Trying to resolve: {}:{}", hostStr, portStr);  // Safe now!

    uint16_t portNum = 0;
    auto it = std::from_chars(portStr.data(), portStr.data() + portStr.size(), portNum);
    // ...
    
    crow::connections::systemBus->async_method_call(
        [hostStr, portNum,  // Capture the string, not the view
         handler = std::forward<ResolveHandler>(handler)](...) {
            // ...
        },
        "org.freedesktop.resolve1", "/org/freedesktop/resolve1",
        "org.freedesktop.resolve1.Manager", "ResolveHostname", 0, hostStr,
        AF_UNSPEC, flag);
}
```

## Why This Wasn't Caught Earlier

1. **The bug is subtle**: String views are designed to be lightweight references, but they don't own the data
2. **Temporaries are destroyed immediately**: The lifetime issue happens at the call site, not in the async callback
3. **The crash happens synchronously**: Unlike the previous async callback issues, this crashes during the initial function call
4. **Implicit conversions hide the problem**: The conversion from `boost::urls::pct_string_view` to `std::string_view` is implicit

## Key Lesson

**Never accept `std::string_view` parameters if the underlying data might be a temporary object that will be destroyed before you're done using the view.**

Either:
1. Accept `std::string` by value or const reference
2. Convert `std::string_view` to `std::string` immediately at the start of the function
3. Document that the caller must ensure the data outlives the function call

In this case, option 2 is the best solution since we need to maintain API compatibility with boost::asio::ip::tcp::resolver.

## Related Issues

This is a **different bug** from the `std::bind_front` issue fixed earlier. That bug was about keeping `ConnectionInfo` alive during async callbacks. This bug is about the lifetime of the string data being passed to `async_resolve()`.

Both bugs needed to be fixed for the code to work correctly.