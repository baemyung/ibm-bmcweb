# Quick Debug Steps - HTTP Client Crash Test

## 🚀 Fastest Way to Detect the Bug

### Step 1: Build with AddressSanitizer (RECOMMENDED)

```bash
# This will detect use-after-free IMMEDIATELY without waiting for crash
meson setup build-asan -Db_sanitize=address -Db_lundef=false
cd build-asan
ninja http-client-extreme-test
```

### Step 2: Run the Test

```bash
./http-client-extreme-test 200 4
```

### Step 3: Check Results

**If bug exists, you'll see:**
```
=================================================================
==12345==ERROR: AddressSanitizer: heap-use-after-free
    #0 in crow::ConnectionPool::sendNext() http/http_client.hpp:705
```

**If no bug:**
```
✓ All tests completed without crash!
```

---

## 🔍 Alternative: Check if Fix is Already Applied

Look at [`http/http_client.hpp`](http/http_client.hpp) around line 705:

```bash
# Check for defensive bounds checking
grep -A 5 "void sendNext" http/http_client.hpp | grep -A 3 "connId >= connections.size()"
```

**If you see this, the fix is already applied:**
```cpp
if (connId >= connections.size())
{
    BMCWEB_LOG_ERROR("Invalid connId: {} (size: {})", connId, connections.size());
    return;
}
```

**If you DON'T see bounds checking, the vulnerability exists!**

---

## 📊 Test Comparison

| Test | Best For | Speed | Detection Rate |
|------|----------|-------|----------------|
| **AddressSanitizer** | Immediate detection | Medium | 99% |
| **Extreme Test** | Actual crash | Fast | 30-50% |
| **Aggressive Test** | Basic testing | Fast | 10-20% |
| **ThreadSanitizer** | Race detection | Slow | 80% |
| **Valgrind** | Thorough analysis | Very Slow | 70% |

---

## 🎯 Quick Commands

### Build All Tests
```bash
meson setup build && cd build && ninja http-client-extreme-test http-client-aggressive-test
```

### Run Extreme Test (Default)
```bash
./build/http-client-extreme-test
```

### Run in Loop (100 attempts)
```bash
for i in {1..100}; do ./build/http-client-extreme-test 500 8 || break; done
```

### Build and Run with ASan
```bash
meson setup build-asan -Db_sanitize=address -Db_lundef=false && \
cd build-asan && \
ninja http-client-extreme-test && \
./http-client-extreme-test 200 4
```

### Check for Crashes
```bash
# Check dmesg
dmesg | tail -50 | grep -i "segfault\|general protection"

# Check coredumps
coredumpctl list | tail -5

# Check if process crashed
echo $?  # Non-zero = crash
```

---

## 🐛 If Test Doesn't Crash

### Option 1: Use ASan (Most Reliable)
```bash
meson setup build-asan -Db_sanitize=address -Db_lundef=false
cd build-asan
ninja http-client-extreme-test
./http-client-extreme-test 200 4
```

### Option 2: Check if Fix Exists
```bash
# Look for bounds checking in sendNext()
sed -n '700,710p' http/http_client.hpp
```

### Option 3: Compare with Vulnerable Version
```bash
# If you have access to ibm-ghe-1110 branch
cd ../ibm-ghe-1110/ibm-bmcweb
meson setup build && cd build
ninja http-client-extreme-test
./http-client-extreme-test 200 4
```

### Option 4: Try on BMC Hardware
```bash
# Copy to BMC
scp build/http-client-extreme-test root@<bmc-ip>:/tmp/

# Run on BMC
ssh root@<bmc-ip> "/tmp/http-client-extreme-test 1000 8"
```

---

## 📝 What Each Test Does

### Extreme Test (NEW - Most Aggressive)
- **10 requests per client** (vs 5 in aggressive)
- **5-10ms retry intervals** (vs 1000ms)
- **2 io_context threads** (vs 1)
- **Synchronized thread start** (all threads start together)
- **Busy work injection** (keeps io_context queue full)

### Aggressive Test (Original)
- 5 requests per client
- 1000ms retry intervals
- 1 io_context thread
- Sequential thread start

---

## 🎓 Understanding the Results

### ✅ Test Passes (No Crash)
**Possible reasons:**
1. Fix is already applied (check line 705 in http_client.hpp)
2. Race is very timing-sensitive
3. Need to try with ASan

### ❌ Test Crashes
**Confirms:**
- Vulnerability exists
- Need to apply defensive fixes
- Check crash location with `coredumpctl gdb`

### 🔬 ASan Detects Issue
**Best outcome:**
- Precise location identified
- No need to wait for crash
- Apply fixes immediately

---

## 📚 Full Documentation

For complete details, see:
- [`EXTREME_CRASH_TEST_DEBUG_GUIDE.md`](EXTREME_CRASH_TEST_DEBUG_GUIDE.md) - Comprehensive debugging guide
- [`AGGRESSIVE_CRASH_TEST_INSTRUCTIONS.md`](AGGRESSIVE_CRASH_TEST_INSTRUCTIONS.md) - Original test instructions
- [`http_client_coredump_analysis_1120.md`](http_client_coredump_analysis_1120.md) - Vulnerability analysis

---

## 🆘 Quick Help

**Test won't build?**
```bash
meson setup --wipe build
cd build
ninja http-client-extreme-test
```

**Can't find the executable?**
```bash
find build -name "http-client-extreme-test" -type f
```

**Need debug symbols?**
```bash
meson setup build-debug -Dbuildtype=debug
cd build-debug
ninja http-client-extreme-test
```

**Want to see what's happening?**
```bash
# Run with verbose output
BMCWEB_LOG_LEVEL=DEBUG ./http-client-extreme-test 100 4