# The Real Bug: std::bind_front with `this` and `shared_from_this()`

## The Actual Crash Location

The crash was happening in `async_resolve.hpp:67` during DNS resolution, NOT in the callback handling code we were trying to fix.

## Root Cause

All async operations in ConnectionInfo were using this pattern:

```cpp
std::bind_front(&ConnectionInfo::afterResolve, this, shared_from_this())
```

**This is WRONG!**

### Why This Crashes

When `std::bind_front` is used with a member function pointer:
1. The FIRST argument (`this`) is used to invoke the member function
2. The SECOND argument (`shared_from_this()`) is passed as a parameter

The problem:
- `this` is a **raw pointer**
- When ConnectionInfo is destroyed, `this` becomes a **dangling pointer**
- Even though `shared_from_this()` keeps the object alive, `std::bind_front` uses `this` FIRST
- Accessing `this` on a destroyed object = **CRASH**

### The Sequence of Events

1. HttpClient destroyed → ConnectionPool destroyed
2. ConnectionPool's `connections` vector destroyed
3. ConnectionInfo's `shared_ptr` refcount goes to 0
4. **BUT** async operation is still queued in io_context
5. Async operation completes (e.g., DNS resolution)
6. Callback tries to invoke: `this->afterResolve(...)`
7. **CRASH**: `this` is a dangling pointer!

The `shared_from_this()` parameter never gets a chance to prevent the crash because `this` is dereferenced first.

## The Correct Pattern

Instead of:
```cpp
std::bind_front(&ConnectionInfo::afterResolve, this, shared_from_this())
```

Use a lambda that captures the shared_ptr:
```cpp
auto self = shared_from_this();
[self](auto&&... args) {
    self->afterResolve(self, std::forward<decltype(args)>(args)...);
}
```

This ensures:
1. The lambda captures `self` (a `shared_ptr`)
2. The object is kept alive as long as the lambda exists
3. No raw `this` pointer is used
4. Safe to call even if the original object would have been destroyed

## Why Our Previous Fixes Didn't Work

1. **Atomic shutdown flag**: Irrelevant - crash happened before any callback was invoked
2. **weak_ptr in afterSendData**: Correct, but the crash was in async_resolve, not in callbacks
3. **shared_from_this() in async handlers**: We HAD this, but used it WRONG with std::bind_front

## All Affected Locations Fixed

1. `doResolve()` - async_resolve
2. `doConnect()` - async_connect  
3. `doSslHandshake()` - async_handshake
4. `sendMessage()` - async_write (SSL and non-SSL)
5. `recvMessage()` - async_read (SSL and non-SSL)
6. `waitAndRetry()` - timer.async_wait
7. `afterSslHandshake()` - async_shutdown

All now use lambdas that capture `shared_from_this()` instead of binding `this`.

## Lesson Learned

**Never use `std::bind_front` with both a raw `this` pointer and `shared_from_this()`.**

The correct patterns are:
1. Lambda capturing shared_ptr: `[self = shared_from_this()](auto&&... args) { self->method(...); }`
2. Static function with weak_ptr: `std::bind_front(&Class::staticMethod, weak_from_this())`

## Why This Bug Was Hard to Find

1. The crash location (async_resolve) was far from where we were looking (callbacks)
2. The pattern `std::bind_front(..., this, shared_from_this())` LOOKS correct
3. It only crashes when the object is destroyed while async operations are pending
4. The timing is very sensitive - depends on when io_context processes the callback

## Verification

After this fix:
- No more crashes in async_resolve
- No more dangling `this` pointers
- All async operations properly keep ConnectionInfo alive
- Safe destruction even with pending operations

The atomic shutdown flag and other defensive checks are still useful for defense-in-depth, but the real bug was the incorrect use of `std::bind_front`.