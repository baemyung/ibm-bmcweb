# Challenge: Reproducing the HTTP Client Coredump

## Problem

The test program does NOT crash on `origin/1110` branch because this branch already has comprehensive protection mechanisms in place:

### Protections Already Present in origin/1110

1. **ConnectionPool weak_ptr protection** (line 797-816):
```cpp
static void afterSendData(const std::weak_ptr<ConnectionPool>& weakSelf, ...) {
    std::shared_ptr<ConnectionPool> self = weakSelf.lock();
    if (!self) {  // Detects if pool was destroyed
        return;   // Safe exit
    }
    self->sendNext(keepAlive, connId);
}
```

2. **ConnectionInfo hybrid approach** (all async callbacks):
```cpp
resolver.async_resolve(host, port,
    std::bind_front(&ConnectionInfo::afterResolve, this, shared_from_this()));
    //                                              ^^^^  ^^^^^^^^^^^^^^^^^
    //                                              raw   keeps object alive
```

3. **Timer weak_ptr protection** (line 417-435):
```cpp
static void onTimeout(const std::weak_ptr<ConnectionInfo>& weakSelf, ...) {
    std::shared_ptr<ConnectionInfo> self = weakSelf.lock();
    if (self == nullptr) {  // Detects if connection was destroyed
        return;             // Safe exit
    }
    // ... rest of timeout handling
}
```

4. **String copy in async_resolve** (line 83):
```cpp
[host{std::string(host)}, ...](...)  // Copies string_view to std::string
```

## Why the Original Coredump Occurred

The original coredump likely occurred in a version **before** these protections were added. The production system was running an earlier version that had:
- Raw `this` pointers without `shared_from_this()`
- No weak_ptr checks
- Dangling string_view references

## Options to Demonstrate the Bug

### Option 1: Find the Truly Broken Version
Search git history for a commit before these protections were added. This would be before:
- The `shared_from_this()` parameter was added to callbacks
- The weak_ptr checks were added
- The string copy was added to async_resolve

### Option 2: Artificially Remove Protections
Create a "broken" version by removing the protections from `origin/1110`:

1. Remove `shared_from_this()` from all `std::bind_front` calls
2. Remove weak_ptr checks from `afterSendData` and `onTimeout`
3. Remove string copy from async_resolve lambda capture

This would create a truly vulnerable version that the test can crash.

### Option 3: Document the Fixes
Instead of reproducing the crash, document:
- What protections are in place
- How they prevent the crash
- What the code looked like before (if we can find it)
- Why the lambda approach in "fixed-good" is superior

## Recommendation

Since `origin/1110` already has protections, the test demonstrates that **the protections work**, not that there's a crash. The value of the "fixed-good" branch is that it uses a **cleaner, more idiomatic approach** (pure lambdas) compared to the hybrid approach in `origin/1110`.

The comparison should be:
- **origin/1110**: Hybrid approach (works but inconsistent)
- **fixed-good**: Pure lambda approach (works and is cleaner)

Both are safe, but the lambda approach is superior code quality.