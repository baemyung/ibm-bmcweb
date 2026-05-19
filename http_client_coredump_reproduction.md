# HTTP Client Coredump - Reproduction Guide

**Defect**: https://jazz07.rchland.ibm.com:13443/jazz/web/projects/FPS%20Collaboration#action=com.ibm.team.workitem.viewWorkItem&id=778179

**File**: [`http/http_client.hpp`](../ibm-bmcweb/http/http_client.hpp)

---

## Overview

The coredump occurs due to use-after-free vulnerabilities when `HttpClient` or `ConnectionPool` objects are destroyed while async operations are still pending. This document provides several methods to reproduce the issue.

---

## Reproduction Method 1: Rapid Client Destruction

### Description
Create and destroy `HttpClient` instances rapidly while requests are in flight.

### Test Code

```cpp
#include "http/http_client.hpp"
#include <boost/asio/io_context.hpp>
#include <thread>
#include <chrono>

void reproduceRapidDestruction() {
    boost::asio::io_context ioc;
    
    // Start io_context in a separate thread
    std::thread ioThread([&ioc]() {
        ioc.run();
    });
    
    for (int i = 0; i < 100; i++) {
        auto policy = std::make_shared<crow::ConnectionPolicy>();
        policy->maxRetryAttempts = 3;
        
        // Create HttpClient
        auto client = std::make_unique<crow::HttpClient>(ioc, policy);
        
        // Send a request to a slow/non-existent server
        boost::beast::http::fields headers;
        client->sendData(
            "test data",
            boost::urls::url_view("http://192.0.2.1:8080/test"), // Non-routable IP
            crow::ensuressl::VerifyCertificate::Verify,
            headers,
            boost::beast::http::verb::post
        );
        
        // Destroy client immediately (while DNS resolution or connection is pending)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        client.reset(); // COREDUMP likely here
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    ioc.stop();
    ioThread.join();
}
```

### Expected Behavior (Before Fix)
- Segmentation fault when callback tries to access destroyed `ConnectionPool`
- Crash in `afterSendData()` when `weakSelf.lock()` returns null but code continues
- Crash in `sendNext()` with invalid `connId`

### Expected Behavior (After Fix)
- Clean shutdown without crashes
- Callbacks detect destroyed pool and return early
- All pending operations cancelled properly

---

## Reproduction Method 2: Shutdown During Active Requests

### Description
Destroy the client while multiple requests are in various stages (resolving, connecting, sending, receiving).

### Test Code

```cpp
void reproduceShutdownDuringRequests() {
    boost::asio::io_context ioc;
    std::thread ioThread([&ioc]() { ioc.run(); });
    
    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxConnections = 5;
    
    auto client = std::make_unique<crow::HttpClient>(ioc, policy);
    
    // Send multiple requests to different slow servers
    std::vector<std::string> urls = {
        "http://192.0.2.1:8080/test1",  // Non-routable
        "http://192.0.2.2:8080/test2",
        "http://192.0.2.3:8080/test3",
        "http://192.0.2.4:8080/test4",
        "http://192.0.2.5:8080/test5"
    };
    
    boost::beast::http::fields headers;
    for (const auto& url : urls) {
        client->sendData(
            "test data",
            boost::urls::url_view(url),
            crow::ensuressl::VerifyCertificate::Verify,
            headers,
            boost::beast::http::verb::post
        );
    }
    
    // Wait a bit for requests to be in different states
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Destroy client while requests are pending
    client.reset(); // COREDUMP likely here
    
    ioc.stop();
    ioThread.join();
}
```

### Expected Behavior (Before Fix)
- Multiple segmentation faults as different async operations complete
- Crashes in various callback functions
- Use-after-free detected by AddressSanitizer

---

## Reproduction Method 3: Connection Pool Stress Test

### Description
Stress test the connection pool with high concurrency and rapid destruction.

### Test Code

```cpp
void reproduceConnectionPoolStress() {
    boost::asio::io_context ioc;
    std::thread ioThread([&ioc]() { ioc.run(); });
    
    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxConnections = 10;
    policy->maxRetryAttempts = 5;
    
    auto client = std::make_unique<crow::HttpClient>(ioc, policy);
    
    // Queue many requests to same destination
    boost::beast::http::fields headers;
    for (int i = 0; i < 50; i++) {
        client->sendData(
            std::format("request {}", i),
            boost::urls::url_view("http://192.0.2.1:8080/test"),
            crow::ensuressl::VerifyCertificate::Verify,
            headers,
            boost::beast::http::verb::post
        );
    }
    
    // Destroy while queue is full and connections are being created
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    client.reset(); // COREDUMP likely here
    
    ioc.stop();
    ioThread.join();
}
```

### Expected Behavior (Before Fix)
- Crash in `sendNext()` with out-of-bounds `connId`
- Crash when accessing destroyed connection pool
- Memory corruption detected

---

## Reproduction Method 4: Timer Race Condition

### Description
Trigger timeout while destroying the connection.

### Test Code

```cpp
void reproduceTimerRaceCondition() {
    boost::asio::io_context ioc;
    std::thread ioThread([&ioc]() { ioc.run(); });
    
    auto policy = std::make_shared<crow::ConnectionPolicy>();
    policy->maxRetryAttempts = 1;
    
    auto client = std::make_unique<crow::HttpClient>(ioc, policy);
    
    // Send request that will timeout
    boost::beast::http::fields headers;
    client->sendData(
        "test data",
        boost::urls::url_view("http://192.0.2.1:8080/test"),
        crow::ensuressl::VerifyCertificate::Verify,
        headers,
        boost::beast::http::verb::post
    );
    
    // Wait for timeout to be close to expiring (30 seconds default)
    std::this_thread::sleep_for(std::chrono::seconds(29));
    
    // Destroy just before timeout fires
    client.reset(); // COREDUMP likely here or when timeout fires
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    ioc.stop();
    ioThread.join();
}
```

### Expected Behavior (Before Fix)
- Race condition between timer callback and destruction
- Crash in `onTimeout()` when accessing destroyed object
- Double-free or use-after-free

---

## Reproduction Method 5: Callback Capture Issue

### Description
Demonstrate the callback capturing references to destroyed objects.

### Test Code

```cpp
void reproduceCallbackCaptureIssue() {
    boost::asio::io_context ioc;
    std::thread ioThread([&ioc]() { ioc.run(); });
    
    auto policy = std::make_shared<crow::ConnectionPolicy>();
    
    {
        crow::HttpClient client(ioc, policy);
        
        // Send request with callback that captures local references
        boost::beast::http::fields headers;
        client.sendDataWithCallback(
            "test data",
            boost::urls::url_view("http://httpbin.org/delay/2"), // 2 second delay
            crow::ensuressl::VerifyCertificate::Verify,
            headers,
            boost::beast::http::verb::post,
            [](crow::Response& res) {
                // This callback may execute after client is destroyed
                BMCWEB_LOG_DEBUG("Response: {}", res.resultInt());
            }
        );
        
        // Let request start
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
    } // client destroyed here, but callback may still execute
    
    // Wait for response
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    ioc.stop();
    ioThread.join();
}
```

### Expected Behavior (Before Fix)
- Callback executes after client destruction
- Access to destroyed ConnectionPool in `afterSendData()`
- Segmentation fault in callback

---

## Running with Memory Sanitizers

### AddressSanitizer (ASan)

```bash
# Compile with ASan
meson setup build-asan -Db_sanitize=address -Db_lundef=false
ninja -C build-asan

# Run test
./build-asan/bmcweb
```

**Expected ASan Output (Before Fix):**
```
==12345==ERROR: AddressSanitizer: heap-use-after-free
READ of size 8 at 0x60300000eff0 thread T1
    #0 in crow::ConnectionPool::sendNext()
    #1 in crow::ConnectionPool::afterSendData()
    ...
```

### ThreadSanitizer (TSan)

```bash
# Compile with TSan
meson setup build-tsan -Db_sanitize=thread
ninja -C build-tsan

# Run test
./build-tsan/bmcweb
```

**Expected TSan Output (Before Fix):**
```
WARNING: ThreadSanitizer: data race
  Write of size 8 at 0x7b0400000000 by thread T1:
    #0 crow::ConnectionInfo::~ConnectionInfo()
  Previous read of size 8 at 0x7b0400000000 by thread T2:
    #0 crow::ConnectionInfo::afterRead()
```

### Valgrind

```bash
# Run with Valgrind
valgrind --leak-check=full --show-leak-kinds=all \
         --track-origins=yes --verbose \
         ./build/bmcweb

# Or with memcheck
valgrind --tool=memcheck --leak-check=full \
         --show-reachable=yes ./build/bmcweb
```

**Expected Valgrind Output (Before Fix):**
```
==12345== Invalid read of size 8
==12345==    at 0x4C2FB3F: crow::ConnectionPool::sendNext()
==12345==  Address 0x5b7c040 is 0 bytes inside a block of size 128 free'd
==12345==    at 0x4C2EDEB: operator delete(void*)
==12345==    by 0x4E8A2F1: std::shared_ptr<crow::ConnectionPool>::~shared_ptr()
```

---

## Automated Test Script

### Bash Script for Continuous Testing

```bash
#!/bin/bash
# test_coredump.sh

echo "Testing HTTP Client Coredump Issues"
echo "===================================="

# Build with sanitizers
echo "Building with AddressSanitizer..."
meson setup build-asan -Db_sanitize=address -Db_lundef=false
ninja -C build-asan

# Run tests
echo "Running rapid destruction test..."
timeout 60 ./build-asan/bmcweb --test-rapid-destruction

echo "Running shutdown during requests test..."
timeout 60 ./build-asan/bmcweb --test-shutdown-requests

echo "Running connection pool stress test..."
timeout 60 ./build-asan/bmcweb --test-pool-stress

echo "Running timer race condition test..."
timeout 60 ./build-asan/bmcweb --test-timer-race

echo "Running callback capture test..."
timeout 60 ./build-asan/bmcweb --test-callback-capture

echo "===================================="
echo "Tests complete. Check for crashes or sanitizer errors above."
```

---

## Real-World Scenarios

### Scenario 1: Redfish Event Subscription Deletion

```
1. Client subscribes to Redfish events
2. BMCWeb creates HttpClient to send events
3. Events are being sent (async operations in progress)
4. Client deletes subscription
5. BMCWeb destroys HttpClient
6. COREDUMP when pending async operations complete
```

### Scenario 2: BMCWeb Shutdown

```
1. Multiple Redfish aggregation connections active
2. HTTP requests in various stages (DNS, connect, send, receive)
3. BMCWeb receives shutdown signal
4. All HttpClient instances destroyed
5. COREDUMP as async operations complete after destruction
```

### Scenario 3: Network Timeout During Aggregation

```
1. Redfish aggregation to remote BMC
2. Network becomes slow/unresponsive
3. Multiple retry attempts in progress
4. Aggregation times out and cleans up
5. ConnectionPool destroyed
6. COREDUMP when retry timer fires
```

---

## Verification After Fix

### Test Checklist

- [ ] Run all reproduction methods without crashes
- [ ] No AddressSanitizer errors
- [ ] No ThreadSanitizer warnings
- [ ] No Valgrind errors
- [ ] Clean shutdown in all scenarios
- [ ] Proper cleanup of all resources
- [ ] No memory leaks detected

### Expected Logs After Fix

```
[DEBUG] Connection shutting down, skipping callback
[DEBUG] ConnectionPool destroyed, skipping callback and sendNext
[DEBUG] Destroying ConnectionInfo id: 0
[DEBUG] Destroying connection pool for ssl_verify://192.0.2.1:8080
```

---

## Additional Testing Tools

### GDB Debugging

```bash
# Run with GDB
gdb ./build/bmcweb

# Set breakpoints
(gdb) break crow::ConnectionInfo::~ConnectionInfo
(gdb) break crow::ConnectionPool::afterSendData
(gdb) break crow::ConnectionPool::sendNext

# Run and reproduce
(gdb) run

# When crash occurs
(gdb) bt full
(gdb) info threads
(gdb) thread apply all bt
```

### Core Dump Analysis

```bash
# Enable core dumps
ulimit -c unlimited

# Run until crash
./build/bmcweb

# Analyze core dump
gdb ./build/bmcweb core

# In GDB
(gdb) bt full
(gdb) info registers
(gdb) x/10i $pc
```

---

## Performance Testing

### Load Test Script

```python
#!/usr/bin/env python3
import requests
import threading
import time

def stress_test():
    """Send many concurrent requests"""
    def send_request():
        try:
            requests.post('http://localhost:8080/redfish/v1/EventService/Subscriptions',
                         json={'Destination': 'http://192.0.2.1:8080/events'},
                         timeout=1)
        except:
            pass
    
    threads = []
    for i in range(100):
        t = threading.Thread(target=send_request)
        threads.append(t)
        t.start()
    
    # Wait a bit then kill bmcweb
    time.sleep(0.5)
    # Send SIGTERM to bmcweb
    
    for t in threads:
        t.join()

if __name__ == '__main__':
    stress_test()
```

---

## Summary

The coredump is most reliably reproduced by:

1. **Creating async operations** (HTTP requests)
2. **Destroying the client** before operations complete
3. **Waiting for callbacks** to execute after destruction

The key is the timing - the destruction must happen while async operations are pending but before they complete. Using non-routable IPs or slow servers makes this easier to reproduce consistently.

With the fixes applied, all these scenarios should complete cleanly without crashes.