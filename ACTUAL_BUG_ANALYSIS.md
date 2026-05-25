# The Actual Bug - std::bad_function_call in afterRead

## Stack Trace Analysis

The crash occurs at:
```
#8  std::__throw_bad_function_call()
#9  crow::ConnectionInfo::afterRead (this=0x14dc1a0, ec=..., bytesTransferred=<optimized out>) 
    at /usr/src/debug/bmcweb/1.0+git/http/http_client.hpp:390
```

Line 390 is:
```cpp
if (connPolicy->invalidResp(respCode))
```

The `std::bad_function_call` exception means `connPolicy->invalidResp` is an **empty std::function**.

## Root Cause

The issue is NOT that `connPolicy` is null - it's that the ConnectionInfo object (`this`) is **already destroyed** when `afterRead` is called, leading to accessing corrupted memory.

### Why This Happens

1. **Hybrid approach is insufficient**: The code uses:
```cpp
std::bind_front(&ConnectionInfo::afterResolve, this, shared_from_this())
```

2. **The problem**: While `shared_from_this()` is passed as a parameter, `std::bind_front` binds `this` (raw pointer) as the implicit first parameter for the member function call.

3. **The race**: If ConnectionInfo is destroyed before the callback executes:
   - The `shared_from_this()` parameter keeps *a* ConnectionInfo alive
   - But `this` (the raw pointer) is dangling
   - When the callback executes, it calls `this->afterRead(...)` with the dangling `this`

### Memory Corruption Scenario

```
1. ConnectionInfo created, shared_ptr refcount = 1
2. async_read starts with callback: bind_front(&ConnectionInfo::afterRead, this, shared_from_this())
   - Callback captures: raw `this` pointer + shared_ptr (refcount = 2)
3. Original shared_ptr goes out of scope (refcount = 1)
4. Something causes the ConnectionInfo to be destroyed (refcount = 0)
   - But callback still holds a shared_ptr! How?
5. The callback's shared_ptr parameter is the WRONG ConnectionInfo instance!
   - std::bind_front created a COPY of shared_from_this() at bind time
   - If ConnectionInfo was recreated/moved, the callback has the old instance
6. Callback executes: this->afterRead(self, ...)
   - `this` is dangling (points to freed memory)
   - `self` is valid but points to wrong/old instance
   - Accessing `this->connPolicy->invalidResp` reads garbage
   - Garbage looks like empty std::function
   - Calling it throws std::bad_function_call
```

## Why Weak_ptr Protection Doesn't Help

The weak_ptr protection in `afterSendData` only protects against **ConnectionPool** destruction. It doesn't protect against **ConnectionInfo** destruction, which is the actual problem here.

## The Fix

Replace `std::bind_front(this, shared_from_this())` with lambda capture:
```cpp
// BROKEN:
std::bind_front(&ConnectionInfo::afterRead, this, shared_from_this())

// FIXED:
[self = shared_from_this()](const boost::system::error_code& ec, size_t bytes) {
    self->afterRead(self, ec, bytes);
}
```

The lambda captures `shared_from_this()` and uses it for BOTH:
1. Keeping the object alive
2. Calling the member function (no raw `this`)

## Test to Reproduce

The test needs to:
1. Create a ConnectionInfo
2. Start an async operation
3. Destroy all shared_ptrs to ConnectionInfo EXCEPT the one in the callback
4. Let the callback execute
5. It should crash with std::bad_function_call

This is difficult to reproduce in a test because the timing has to be perfect, which is why it only happens in production under load.