# 🚀 FIFO Cache in C++ — Complete Beginner-Friendly Explanation

[🏠 Home](../README.md) > [05. Caching](./README.md) > **FIFO Cache in C++**

---

**Problem:** Design and implement a First-In, First-Out (FIFO) Cache.

**Difficulty:** Easy to Medium  
**Approach:** Queue + Hash Map  
**Time Complexity:** $O(1)$ average for `get()` and `put()`  
**Space Complexity:** $O(\text{capacity})$  
**Executable Source Code:** [`code/fifo_cache.cpp`](./code/fifo_cache.cpp)

---

## 📚 Table of Contents

1. [🧠 1. Understand What a FIFO Cache Is](#1--understand-what-a-fifo-cache-is)
2. [💡 2. Understand the Problem with a Real-Life Example](#2--understand-the-problem-with-a-real-life-example)
3. [🔍 3. Dry Run the Step-by-Step Example](#3--dry-run-the-given-example)
4. [🛠️ 4. Understand Which Data Structures We Need](#4-️-understand-which-data-structures-we-need)
5. [💻 5. Complete C++ Code Implementation](#5--write-the-complete-c-code)
6. [📖 6. Line-by-Line Code Explanation](#6--understand-the-code-line-by-line)
7. [⏱️ 7. Time and Space Complexity Analysis](#7-️-analyze-time-and-space-complexity)
8. [🎯 8. Final Revision Notes for Interviews](#8--final-revision-notes-for-interviews)

---

# 1. 🧠 Understand What a FIFO Cache Is

## What does FIFO mean?

**FIFO = First In, First Out**

It is a cache replacement policy where the oldest inserted item is evicted first when the cache reaches maximum capacity.

In simple English:

> Whichever item was inserted earliest is the first one thrown out when space runs out—regardless of how many times it was accessed or read!

### The Critical Difference: FIFO vs. LRU

* In **LRU (Least Recently Used)**: Accessing an existing item with `get(key)` saves it from eviction by promoting it to the most recently used position.
* In **FIFO (First-In, First-Out)**: Accessing an item with `get(key)` **does not alter its eviction order**. Even if an item is read 1,000 times, it will still be evicted if it was the earliest item inserted into the cache.

---

# 2. 💡 Understand the Problem with a Real-Life Example

## The Cafeteria Food Tray Stack

Imagine a cafeteria with a tray dispenser that holds only **3 trays**.

```text
EVICTED FIRST (Front)                       INSERTED LAST (Back)
        ↓                                           ↓
    [ Tray 1 ]   <=========>   [ Tray 2 ]   <=========>   [ Tray 3 ]
   (Arrived 8:00 AM)         (Arrived 8:05 AM)          (Arrived 8:10 AM)
```

1. **Tray 1** was placed into the dispenser at 8:00 AM.
2. **Tray 2** was placed at 8:05 AM.
3. **Tray 3** was placed at 8:10 AM.

Now a customer wants to inspect **Tray 1** (calling `get(1)`). They look at Tray 1 and put it right back.

At 8:15 AM, the kitchen brings **Tray 4**. The dispenser is full. Which tray gets pushed out?

**Tray 1**, because it was the first tray that entered the queue, even though someone just looked at it!

---

# 3. 🔍 Dry Run the Given Example

Let's trace a concrete example with **Capacity = 3**:

```cpp
FIFOCache cache(3);

cache.put(1, 100);
cache.put(2, 200);
cache.put(3, 300);
cache.get(1);      // Key 1 is accessed!
cache.put(4, 400); // Cache full: which key is evicted?
cache.get(1);      // Check if key 1 survived
cache.get(2);
```

### Step 1: Create Cache
* `capacity = 3`, `Queue = []`, `Map = {}`

### Step 2: `put(1, 100)`
* `Queue = [1]`, `Map = {1: 100}`

### Step 3: `put(2, 200)`
* `Queue = [1, 2]`, `Map = {1: 100, 2: 200}`

### Step 4: `put(3, 300)`
* `Queue = [1, 2, 3]`, `Map = {1: 100, 2: 200, 3: 300}`. Cache is now full (3/3).

### Step 5: `get(1)` $\implies$ Cache HIT (`100`)
* **Crucial Rule:** In FIFO, reads do not alter queue order!
* Queue remains: `[1, 2, 3]`. Key 1 is still at the front!

### Step 6: `put(4, 400)` $\implies$ Eviction!
* The cache is full. Oldest element at `Queue.front()` is **Key 1**.
* Evict Key 1: Pop `1` from Queue, erase `1` from Map.
* Push `4`: `Queue = [2, 3, 4]`, `Map = {2: 200, 3: 300, 4: 400}`.

### Step 7: `get(1)` $\implies$ Cache MISS (`-1`)
* Key 1 was evicted despite being accessed in Step 5!

### Step 8: `get(2)` $\implies$ Cache HIT (`200`)

---

## 📊 Complete Dry Run State Transition Table

| Step | Operation | FIFO Queue (Oldest $\to$ Newest) | Oldest Key | Evicted Key | Return Value | Notes |
|:---:|:---|:---:|:---:|:---:|:---:|---|
| **1** | `FIFOCache(3)` | `[]` | — | *None* | `null` | Capacity initialized to 3 |
| **2** | `put(1, 100)` | `[1]` | 1 | *None* | `null` | Enqueued at back |
| **3** | `put(2, 200)` | `[1, 2]` | 1 | *None* | `null` | Enqueued at back |
| **4** | `put(3, 300)` | `[1, 2, 3]` | 1 | *None* | `null` | Cache reaches full capacity |
| **5** | `get(1)` | `[1, 2, 3]` | 1 | *None* | **100** | Cache HIT; order is NOT changed! |
| **6** | `put(4, 400)` | `[2, 3, 4]` | 2 | **Key 1** | `null` | Full; Key 1 evicted from front |
| **7** | `get(1)` | `[2, 3, 4]` | 2 | *None* | **-1** | Cache MISS; was evicted |
| **8** | `get(2)` | `[2, 3, 4]` | 2 | *None* | **200** | Cache HIT |

---

# 4. 🛠️ Understand Which Data Structures We Need

We need two capabilities:
1. **$O(1)$ lookup and value storage:** Hash Map (`std::unordered_map<int, int>`).
2. **$O(1)$ chronological arrival tracking:** Standard Queue (`std::queue<int>`).

```text
               HASH MAP (unordered_map)
          ┌───────────────────────────────────┐
          │ Key 1 ──> 100                     │
          │ Key 2 ──> 200                     │
          │ Key 3 ──> 300                     │
          └───────────────────────────────────┘
                           │
                           ▼
                 FIFO QUEUE (std::queue)

       FRONT (Oldest)                     BACK (Newest)
             ↓                                 ↓
         [ Key 1 ]    ──>    [ Key 2 ]    ──>    [ Key 3 ]
```

---

# 5. 💻 Write the Complete C++ Code

```cpp
#include <iostream>
#include <queue>
#include <unordered_map>

class FIFOCache {
private:
    int capacity;
    std::queue<int> orderQueue;           // Tracks chronological arrival order
    std::unordered_map<int, int> cacheMap; // Stores {key, value} for O(1) lookups

public:
    explicit FIFOCache(int cap) : capacity(cap < 0 ? 0 : cap) {}

    int get(int key) {
        if (capacity <= 0) return -1;

        auto it = cacheMap.find(key);
        if (it == cacheMap.end()) {
            std::cout << "[GET " << key << "] -> Cache MISS (-1)\n";
            return -1;
        }

        // In FIFO, accessing a key does NOT change its position in the queue!
        std::cout << "[GET " << key << "] -> Cache HIT (Value: " << it->second 
                  << ") [FIFO order unchanged]\n";
        return it->second;
    }

    void put(int key, int value) {
        if (capacity <= 0) {
            std::cout << "[PUT " << key << ", " << value << "] -> Ignored: Capacity is 0.\n";
            return;
        }

        // Case 1: Key already exists -> Update value in-place without altering FIFO order
        if (cacheMap.find(key) != cacheMap.end()) {
            cacheMap[key] = value;
            std::cout << "[PUT " << key << ", " << value << "] -> Updated existing key (Order preserved)\n";
            return;
        }

        // Case 2: Cache is full -> Evict the oldest key from front of queue
        if (static_cast<int>(cacheMap.size()) >= capacity) {
            int oldestKey = orderQueue.front();
            orderQueue.pop();
            cacheMap.erase(oldestKey);
            std::cout << "[EVICT] Cache full! Evicted Oldest Key: " << oldestKey << " from Front\n";
        }

        // Case 3: Insert new key at the back of the queue
        orderQueue.push(key);
        cacheMap[key] = value;
        std::cout << "[PUT " << key << ", " << value << "] -> Inserted at Back (Newest)\n";
    }

    size_t size() const {
        return cacheMap.size();
    }
};
```

---

# 6. 📖 Understand the Code Line by Line

## 1. `get(key)` Does Not Move Items
```cpp
auto it = cacheMap.find(key);
if (it == cacheMap.end()) return -1;
return it->second;
```
Unlike LRU which calls `splice()` on hit, FIFO simply reads `it->second` and returns. The queue is untouched!

## 2. In-Place Update on Existing Key
```cpp
if (cacheMap.find(key) != cacheMap.end()) {
    cacheMap[key] = value;
    return;
}
```
If the key already exists, updating its value does not reset its insertion age. It retains its original position in the queue.

## 3. Eviction from Front
```cpp
int oldestKey = orderQueue.front();
orderQueue.pop();
cacheMap.erase(oldestKey);
```
`orderQueue.front()` gives the oldest key in $O(1)$ time. `pop()` removes it from the queue, and `erase()` removes it from the map.

---

# 7. ⏱️ Analyze Time and Space Complexity

| Operation | Average Time Complexity | Worst Case | Auxiliary Space |
|---|:---:|:---:|:---:|
| `get(key)` | **$O(1)$** | $O(N)$ (hash collision) | $O(1)$ |
| `put(key, value)` | **$O(1)$** | $O(N)$ | $O(1)$ |
| **Total Cache** | — | — | **$O(\text{capacity})$** |

---

# 8. 🎯 Final Revision Notes for Interviews

* **Pros:** Simplest eviction algorithm to implement; minimal memory overhead; zero reordering cost on reads.
* **Cons (Belady's Anomaly):** Frequently evicts foundational, heavily-used configuration or master keys simply because they were loaded at startup.
