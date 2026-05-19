# Coredump Verification Guide

## What the Logs Show

The logs you're seeing indicate the Python script is successfully reproducing the coredump conditions:

```
May 19 16:05:46 p11bmc bmcwebd[1283]: [http_connection.hpp:248] 0x10785b0 SSL handshake failed
May 19 16:05:46 p11bmc bmcwebd[1283]: [http_connection.hpp:248] 0x10c3890 SSL handshake failed
May 19 16:05:46 p11bmc bmcwebd[1283]: [http_connection.hpp:248] 0x10efaf0 SSL handshake failed
```

**These are EXPECTED behaviors** when the client abruptly closes connections during:
- SSL handshake phase
- Authentication phase
- Active HTTP requests

## Checking for Actual Coredumps

### Method 1: Check systemd coredumps

```bash
# List all coredumps
coredumpctl list

# List only bmcweb coredumps
coredumpctl list bmcweb

# Show details of the most recent coredump
coredumpctl info

# Get backtrace of the most recent coredump
coredumpctl debug
```

### Method 2: Check core dump directory

```bash
# Check for core files
ls -lh /var/lib/systemd/coredump/

# Or traditional location
ls -lh /var/crash/
ls -lh /tmp/core*
```

### Method 3: Monitor BMCWeb process

```bash
# Watch for crashes in real-time
journalctl -u bmcweb -f | grep -i "segmentation\|core\|crash\|signal"

# Check if bmcweb process died
systemctl status bmcweb
```

### Method 4: Check dmesg for segfaults

```bash
# Look for segmentation faults
dmesg | grep -i "segfault\|general protection"

# Or with timestamps
dmesg -T | grep -i "bmcweb"
```

## Expected Coredump Indicators

### Before Fix Applied

If the coredump issue exists, you should see:

1. **Process crash:**
   ```
   bmcwebd[1283]: Segmentation fault (core dumped)
   ```

2. **Signal 11 (SIGSEGV):**
   ```
   bmcwebd[1283]: Process 1283 (bmcweb) of user 0 dumped core.
   ```

3. **Coredump file created:**
   ```bash
   $ coredumpctl list
   TIME                            PID   UID   GID SIG COREFILE  EXE
   Mon 2026-05-19 16:05:47 EDT    1283     0     0  11 present   /usr/sbin/bmcweb
   ```

4. **Stack trace showing use-after-free:**
   ```
   #0  crow::ConnectionPool::sendNext()
   #1  crow::ConnectionPool::afterSendData()
   #2  boost::asio::detail::binder1::operator()
   ```

### After Fix Applied

If the fix is working correctly:

1. **No crashes** - bmcweb continues running
2. **Clean error handling:**
   ```
   [DEBUG] Connection shutting down, skipping callback
   [DEBUG] ConnectionPool destroyed, skipping callback and sendNext
   ```
3. **No coredump files created**
4. **Process remains alive:**
   ```bash
   $ systemctl status bmcweb
   ● bmcweb.service - BMC Web Server
        Active: active (running)
   ```

## Running with Enhanced Detection

### Option 1: Run with AddressSanitizer (Recommended)

ASan will detect use-after-free immediately without waiting for a crash:

```bash
# Build with ASan
cd /path/to/bmcweb
meson setup build-asan -Db_sanitize=address -Db_lundef=false
ninja -C build-asan

# Stop production bmcweb
systemctl stop bmcweb

# Run ASan build
./build-asan/bmcweb

# In another terminal, run the Python script
python3 test_http_client_coredump.py --host localhost --port 443
```

**Expected ASan output (if bug exists):**
```
=================================================================
==12345==ERROR: AddressSanitizer: heap-use-after-free on address 0x60300000eff0
READ of size 8 at 0x60300000eff0 thread T1
    #0 0x7f8b4c2d1234 in crow::ConnectionPool::sendNext() http/http_client.hpp:705
    #1 0x7f8b4c2d1567 in crow::ConnectionPool::afterSendData() http/http_client.hpp:834
    
0x60300000eff0 is located 0 bytes inside of 128-byte region [0x60300000eff0,0x60300000f070)
freed by thread T2 here:
    #0 0x7f8b4d123456 in operator delete(void*)
    #1 0x7f8b4c2d1890 in std::shared_ptr<crow::ConnectionPool>::~shared_ptr()
```

### Option 2: Run with Valgrind

```bash
# Stop production bmcweb
systemctl stop bmcweb

# Run with Valgrind
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind.log \
         ./build/bmcweb

# In another terminal, run the Python script
python3 test_http_client_coredump.py --host localhost --port 443

# Check the log
cat valgrind.log | grep -A 10 "Invalid read\|Invalid write"
```

### Option 3: Run with GDB

```bash
# Stop production bmcweb
systemctl stop bmcweb

# Run with GDB
gdb ./build/bmcweb

# In GDB, set breakpoints
(gdb) break crow::ConnectionInfo::~ConnectionInfo
(gdb) break crow::ConnectionPool::afterSendData
(gdb) break crow::ConnectionPool::sendNext
(gdb) run

# In another terminal, run the Python script
python3 test_http_client_coredump.py --host localhost --port 443

# When it crashes
(gdb) bt full
(gdb) info threads
(gdb) thread apply all bt
```

## Understanding the Logs

### SSL Handshake Failures

```
[http_connection.hpp:248] 0x10785b0 SSL handshake failed
```

**What this means:**
- Client initiated SSL connection
- Client abruptly closed connection during handshake
- BMCWeb's HTTP connection handler detected the failure
- This is EXPECTED when testing rapid disconnection

**This is NOT a coredump** - it's normal error handling.

### Authentication Failures

```
pam_unix(webserver:auth): authentication failure
password check failed for user (root)
```

**What this means:**
- Client sent HTTP request with credentials
- Client closed connection before auth completed
- PAM authentication failed due to disconnection
- This is EXPECTED during the tests

**This is NOT a coredump** - it's normal auth failure logging.

### What to Look For

The coredump happens AFTER these errors, when:

1. **Async callback executes** after ConnectionPool is destroyed
2. **Timer fires** after ConnectionInfo is destroyed
3. **sendNext() is called** with invalid connId

**Key indicators:**
- Process terminates unexpectedly
- "Segmentation fault" message
- Core file created
- Process needs to be restarted

## Test Results Interpretation

### Scenario 1: No Coredump Detected

**Possible reasons:**
1. ✅ **Fix is already applied** - The code properly handles destruction
2. ⚠️ **Timing issue** - Need to run more iterations or adjust timing
3. ⚠️ **Different code path** - The specific vulnerable path wasn't triggered

**Next steps:**
- Run tests multiple times
- Increase iteration counts in the script
- Try with ASan for better detection

### Scenario 2: Coredump Detected

**Confirms:**
- ❌ The use-after-free vulnerability exists
- ❌ Async operations access destroyed objects
- ❌ Fix needs to be applied

**Next steps:**
- Analyze the coredump with `coredumpctl debug`
- Get stack trace to identify exact location
- Apply the fixes from the analysis document

### Scenario 3: ASan Detects Issue

**Best case scenario:**
- ✅ Issue detected without crash
- ✅ Exact location identified
- ✅ Stack trace shows the problem

**Example:**
```
==12345==ERROR: AddressSanitizer: heap-use-after-free
    #0 in crow::ConnectionPool::sendNext() http/http_client.hpp:705
```

This confirms the vulnerability at line 705 in http_client.hpp.

## Continuous Testing

### Automated Test Script

```bash
#!/bin/bash
# continuous_test.sh

echo "Starting continuous coredump testing..."

for i in {1..10}; do
    echo "=== Test iteration $i ==="
    
    # Run the Python script
    python3 test_http_client_coredump.py --host localhost --port 443
    
    # Check for coredumps
    if coredumpctl list bmcweb 2>/dev/null | tail -1 | grep -q "$(date +%Y-%m-%d)"; then
        echo "❌ COREDUMP DETECTED in iteration $i"
        coredumpctl info
        exit 1
    fi
    
    # Check if bmcweb is still running
    if ! systemctl is-active --quiet bmcweb; then
        echo "❌ BMCWEB CRASHED in iteration $i"
        systemctl status bmcweb
        exit 1
    fi
    
    echo "✅ Iteration $i completed successfully"
    sleep 2
done

echo "✅ All 10 iterations completed without crashes"
```

## Summary

The logs you're seeing (SSL handshake failures, auth failures) are **expected behaviors** during the test. They indicate the script is working correctly by:

1. Creating connections
2. Abruptly closing them
3. Triggering error conditions in BMCWeb

To confirm if the **actual coredump** occurs, check:
- `coredumpctl list` for core files
- `systemctl status bmcweb` for process crashes
- `dmesg` for segmentation faults
- Run with ASan for immediate detection

The coredump happens when BMCWeb's async callbacks try to access destroyed ConnectionPool objects, which may occur after the connection errors you're seeing.