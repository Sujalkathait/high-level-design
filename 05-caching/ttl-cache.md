# 🚀 TTL Cache in C++ — Complete Beginner-Friendly Explanation

[🏠 Home](../README.md) > [05. Caching](./README.md) > **TTL Cache in C++**

---

**Problem:** Design and implement a Time-To-Live (TTL) Cache supporting both Lazy Eviction and Active Background Sweeping.

**Difficulty:** Medium  
**Approach:** Monotonic Clocks (`std::chrono::steady_clock`) + Hash Map  
**Time Complexity:** $O(1)$ average for `set()` and `get()`; $O(N)$ for periodic sweep  
**Space Complexity:** $O(N)$ entries  
**Executable Source Code:** [`code/ttl_cache.cpp`](./code/ttl_cache.cpp)

---

## 📚 Table of Contents

1. [🧠 1. Understand What a TTL Cache Is](#1--understand-what-a-ttl-cache-is)
2. [💡 2. Understand the Problem with a Real-Life Example](#2--understand-the-problem-with-a-real-life-example)
3. [🔍 3. Dry Run the Step-by-Step Example](#3--dry-run-the-given-example)
4. [🛠️ 4. Understand Which Data Structures We Need](#4-️-understand-which-data-structures-we-need)
5. [💻 5. Complete C++ Code Implementation](#5--write-the-complete-c-code)
6. [📖 6. Line-by-Line Code Explanation](#6--understand-the-code-line-by-line)
7. [⏱️ 7. Time and Space Complexity Analysis](#7-️-analyze-time-and-space-complexity)
8. [🎯 8. Final Revision Notes for Interviews](#8--final-revision-notes-for-interviews)

---

# 1. 🧠 Understand What a TTL Cache Is

## What does TTL mean?

**TTL = Time-To-Live**

It is a time-based expiration mechanism where each cached item is assigned a lifespan (e.g., 5 minutes, 2 hours). Once that lifespan elapses, the data is considered expired and stale.

### The Two Deletion Strategies

A production TTL cache must handle expired keys using two complementary mechanisms:

1. **Lazy Eviction (Passive):** When a client requests a key using `get(key)`, the cache checks: `current_time > expiry_time?`. If expired, it deletes the key on the spot and returns a Cache Miss.
2. **Active Periodic Sweep (Proactive):** What if a key expires, but the client **never** calls `get()` on it again? (An "abandoned" session). Without an active background cleaner, that expired key would linger in RAM forever, causing a **silent memory leak**! The active sweep periodically scans the cache and frees expired records.

---

# 2. 💡 Understand the Problem with a Real-Life Example

## The Supermarket Milk Carton

Imagine a grocery store shelf holding milk cartons:

```text
[ Carton A: Expires Oct 30 ]
[ Carton B: Expires Oct 28 ]  <--- Expired!
[ Carton C: Expires Nov 05 ]
```

### 1. Lazy Eviction (The Shopper)
A customer reaches for **Carton B** on Oct 29. They check the printed expiration date: *"Oh, it expired yesterday!"* The customer throws it in the trash instead of buying it. The expired item was detected and removed **only when accessed**.

### 2. Active Sweep (The Night Cleaner)
What if **Carton B** sits in the back of the shelf and no customer ever touches it for 6 months? It would rot and take up valuable shelf space!  
Every night at midnight, a supermarket employee walks the aisles, inspects every carton, and throws away all expired items. This is the **Active Background Sweep**.

---

# 3. 🔍 Dry Run the Given Example

Let's trace a concrete timeline:
* $t = 0\text{ ms}$: `set("session", "token_123", TTL = 200ms)` $\implies$ Expiry = $200\text{ms}$
* $t = 0\text{ ms}$: `set("abandoned", "temp_data", TTL = 100ms)` $\implies$ Expiry = $100\text{ms}$

### Timeline Trace:

| Timestamp ($t$) | Action | Checked Key | Evaluated Condition | Result / Action Taken |
|:---:|:---|:---:|:---|:---|
| **$t = 50\text{ ms}$** | `get("session")` | `"session"` | $50\text{ ms} < 200\text{ ms}$ | **Cache HIT** (`"token_123"`) |
| **$t = 250\text{ ms}$** | `get("session")` | `"session"` | $250\text{ ms} > 200\text{ ms}$ | **LAZY EVICTION:** Key expired! Erased from memory $\implies$ **Cache MISS** |
| **$t = 300\text{ ms}$** | `cleanupExpired()` | *All keys* | `"abandoned"`: $300\text{ ms} > 100\text{ ms}$ | **ACTIVE SWEEP:** Erased `"abandoned"` from RAM (Memory freed without any client `get` call!) |

---

# 4. 🛠️ Understand Which Data Structures We Need

### 1. Why `std::chrono::steady_clock` instead of `system_clock`?
* `system_clock` can jump forwards or backwards (e.g., NTP server synchronization, daylight saving time adjustments). If the clock jumps back 1 hour, a 60-second TTL key could live for 61 minutes!
* `steady_clock` is a **monotonic clock** that never jumps backwards. It guarantees steady, reliable tick intervals.

### 2. The Data Structure
```cpp
struct TTLEntry {
    std::string value;
    std::chrono::steady_clock::time_point expiryTime;
};

std::unordered_map<std::string, TTLEntry> store;
```

---

# 5. 💻 Write the Complete C++ Code

```cpp
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <string>
#include <vector>
#include <thread>

struct TTLEntry {
    std::string value;
    std::chrono::steady_clock::time_point expiryTime;
};

class TTLCache {
private:
    std::unordered_map<std::string, TTLEntry> store;

public:
    // Store key with an explicit millisecond TTL
    void set(const std::string& key, const std::string& val, std::chrono::milliseconds ttl) {
        auto expiry = std::chrono::steady_clock::now() + ttl;
        store[key] = {val, expiry};
        std::cout << "[SET] '" << key << "' stored with TTL " << ttl.count() << "ms\n";
    }

    // Lazy Eviction upon get()
    std::string get(const std::string& key) {
        auto it = store.find(key);
        if (it == store.end()) {
            std::cout << "[GET] '" << key << "' -> Cache MISS (Not found)\n";
            return "";
        }

        auto now = std::chrono::steady_clock::now();
        if (now > it->second.expiryTime) {
            // Lazy eviction: remove expired entry on access
            store.erase(it);
            std::cout << "[GET] '" << key << "' -> EXPIRED & EVICTED (Treated as Cache MISS)\n";
            return "";
        }

        std::cout << "[GET] '" << key << "' -> Cache HIT (Value: " << it->second.value << ")\n";
        return it->second.value;
    }

    // Active Periodic Memory Sweep (Prevents memory leaks from unread keys)
    int cleanupExpired() {
        auto now = std::chrono::steady_clock::now();
        std::vector<std::string> expiredKeys;

        for (const auto& [k, entry] : store) {
            if (now > entry.expiryTime) {
                expiredKeys.push_back(k);
            }
        }

        for (const auto& k : expiredKeys) {
            store.erase(k);
        }

        if (!expiredKeys.empty()) {
            std::cout << "[ACTIVE CLEANUP] Swept and evicted " << expiredKeys.size() << " expired keys.\n";
        }
        return static_cast<int>(expiredKeys.size());
    }

    size_t size() const { return store.size(); }
};
```

---

# 6. 📖 Understand the Code Line by Line

## 1. Setting Expiration with Monotonic Time
```cpp
auto expiry = std::chrono::steady_clock::now() + ttl;
store[key] = {val, expiry};
```
Computes absolute future time point.

## 2. Lazy Eviction on `get()`
```cpp
if (now > it->second.expiryTime) {
    store.erase(it);
    return "";
}
```
If current timestamp exceeds stored deadline, the entry is invalidated immediately and purged from RAM.

## 3. Active Background Sweep
```cpp
for (const auto& [k, entry] : store) {
    if (now > entry.expiryTime) expiredKeys.push_back(k);
}
for (const auto& k : expiredKeys) store.erase(k);
```
Collects expired keys first before deleting them, avoiding undefined iterator invalidation during map traversal.

---

# 7. ⏱️ Analyze Time and Space Complexity

| Operation | Time Complexity | Auxiliary Space |
|---|:---:|:---:|
| `set(key, val, ttl)` | **$O(1)$ average** | $O(1)$ |
| `get(key)` | **$O(1)$ average** | $O(1)$ |
| `cleanupExpired()` | **$O(N)$ linear sweep** | $O(\text{expired})$ |
| **Total Cache** | — | **$O(N)$ entries** |

---

# 8. 🎯 Final Revision Notes for Interviews

* **Production Tip (Cache Avalanche Prevention):** If $100{,}000$ keys are set at midnight with the exact same 24-hour TTL, they will all expire simultaneously at 12:00:00 AM tomorrow, causing a catastrophic database stampede. Always add random **TTL jitter** (e.g., $\text{TTL} = 24\text{h} \pm \text{rand}(0, 300\text{s})$).
