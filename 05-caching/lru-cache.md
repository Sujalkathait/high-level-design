# 🚀 LRU Cache in C++ — Complete Beginner-Friendly Explanation

[🏠 Home](../README.md) > [05. Caching](./README.md) > **LRU Cache in C++**

---

**Problem:** Design and implement an LRU (Least Recently Used) Cache.

**Difficulty:** Medium  
**Approach:** Hash Map + Doubly Linked List  
**Time Complexity:** $O(1)$ average for `get()` and `put()`  
**Space Complexity:** $O(\text{capacity})$  
**Executable Source Code:** [`code/lru_cache.cpp`](./code/lru_cache.cpp)

---

## 📚 Table of Contents

1. [🧠 1. Understand What an LRU Cache Is](#1--understand-what-an-lru-cache-is)
2. [💡 2. Understand the Problem with a Real-Life Example](#2--understand-the-problem-with-a-real-life-example)
3. [🔍 3. Dry Run the Step-by-Step Example](#3--dry-run-the-given-example)
4. [🛠️ 4. Understand Which Data Structures We Need](#4-️-understand-which-data-structures-we-need)
5. [💻 5. Complete C++ Code Implementation](#5--write-the-complete-c-code)
6. [📖 6. Line-by-Line Code Explanation](#6--understand-the-code-line-by-line)
7. [⏱️ 7. Time and Space Complexity Analysis](#7-️-analyze-time-and-space-complexity)
8. [🎯 8. Final Revision Notes for Interviews](#8--final-revision-notes-for-interviews)

---

# 1. 🧠 Understand What an LRU Cache Is

## What does LRU mean?

**LRU = Least Recently Used**

It is a cache replacement and eviction technique.

In simple English:

> When the cache becomes full, remove the item that has not been used for the longest time.

Before understanding LRU, let's understand what a cache is.

### What is a cache?

A cache is a small, ultra-fast storage area used to keep data that we may need again soon.

For example, when you open a website, your browser may save some images and stylesheets in its cache. The next time you open the same website, the browser reuses the saved data instead of downloading everything across the network again. This makes operations substantially faster.

### The Problem: Cache Memory is Finite

Suppose a cache can store only **three items**. What happens when we want to insert a fourth item?

We must remove an existing item. But which one?

That's where **LRU** comes in.

### How does LRU decide what to remove?

LRU removes the item that was accessed the longest time ago.

For example:

```text
Cache Capacity = 3

Cache = [A, B, C]
```

Assume:
* `A` = Least Recently Used (LRU)
* `C` = Most Recently Used (MRU)

Now we access `A`. Since `A` was just accessed, it becomes the most recently used item:

```text
Before accessing A:
[A, B, C]
 ↑     ↑
LRU   MRU

After accessing A:
[B, C, A]
 ↑     ↑
LRU   MRU
```

Now insert `D`. The cache is already full. Which item should we remove?

**Answer: B**, because `B` is now the least recently used element at the front.

```text
Before inserting D:
[B, C, A]

Remove B:
[C, A]

Insert D:
[C, A, D]
 ↑     ↑
LRU   MRU
```

### ⭐ Important Rule

Whenever we access or update an existing key, that key becomes the **Most Recently Used (MRU)**.

This happens during both:
* `get(key);`
* `put(key, value);`

---

# 2. 💡 Understand the Problem with a Real-Life Example

Imagine you have a study table. Your table has space for only **three books**.

You have five books:
1. Math
2. Physics
3. Chemistry
4. English
5. Computer Science

Initially, you place:

```text
LRU                                         MRU
 ↓                                           ↓
[Math] <=======> [Physics] <=======> [Chemistry]
```

Assume the leftmost book (`Math`) was used the longest time ago.

Now you study Math. Since you just used Math, it becomes the most recently used book and moves to the right end:

```text
LRU                                         MRU
 ↓                                           ↓
[Physics] <=======> [Chemistry] <=======> [Math]
```

Now you want to study English. But the table is full (3/3 capacity). Which book should you remove from the table?

**Answer: Physics.**

Why? Because Physics has not been used for the longest time.

After removing Physics and placing English on the table:

```text
LRU                                         MRU
 ↓                                           ↓
[Chemistry] <=======> [Math] <=======> [English]
```

This is the exact idea behind an LRU Cache.

---

## Understanding the Three Functions

We need to implement the following class interface:

```cpp
class LRUCache {
public:
    LRUCache(int capacity) {

    }

    int get(int key) {

    }

    void put(int key, int value) {

    }
};
```

### Function 1: Constructor

```cpp
LRUCache(int capacity)
```

Initializes the cache with a positive maximum capacity.

Example:
```cpp
LRUCache cache(2);
// Maximum capacity = 2, Cache = []
```

### Function 2: `get(key)`

```cpp
int get(int key)
```

Searches for a key in the cache.

* **Case 1: Key exists** $\implies$ Return its value and mark it as recently used (move to MRU).
* **Case 2: Key does not exist** $\implies$ Return `-1`.

### Function 3: `put(key, value)`

```cpp
void put(int key, int value)
```

Inserts or updates a key-value pair. There are three possible situations:

* **Case 1: Key already exists** $\implies$ Update its value and move it to the MRU position.
* **Case 2: New key and space available** $\implies$ Insert the new pair at the MRU position.
* **Case 3: New key and cache is full** $\implies$ Evict the least recently used key from the LRU position, then insert the new key at the MRU position.

---

# 3. 🔍 Dry Run the Given Example

Let's dry run the classic sequence step by step.

### Input Sequence

```cpp
LRUCache cache(2);

cache.put(1, 10);
cache.get(1);
cache.put(2, 20);
cache.put(3, 30);
cache.get(2);
cache.get(1);
```

> **Convention:**  
> **LEFT (Front)** = Least Recently Used (LRU)  
> **RIGHT (Back)** = Most Recently Used (MRU)

---

### Step 1: Create Cache
```cpp
LRUCache cache(2);
```
* `capacity = 2`
* `cache = []` (Initially empty)

---

### Step 2: `put(1, 10)`
```cpp
cache.put(1, 10);
```
* Insert `(1, 10)`.
* Cache: `[(1, 10)]`
* Only one element exists $\implies$ it is both LRU and MRU.

---

### Step 3: `get(1)`
```cpp
cache.get(1);
```
* Search for key `1`. Key `1` exists.
* Return `10`.
* Key `1` was accessed, so it stays at the MRU position: `[(1, 10)]`.

---

### Step 4: `put(2, 20)`
```cpp
cache.put(2, 20);
```
* Key `2` does not exist. Space is available (`size = 1 < capacity = 2`).
* Insert `(2, 20)` at the back (MRU):
```text
LRU (Front)                MRU (Back)
     ↓                          ↓
  [(1, 10)] <=============> [(2, 20)]
```
* Cache is now full (`size = 2 / 2`).

---

### Step 5: `put(3, 30)`
```cpp
cache.put(3, 30);
```
* We want to insert key `3`, but the cache is full!
* Must evict the LRU key at the front: **Key 1**.
* Remove `(1, 10)`.
* Insert `(3, 30)` at the back (MRU).
```text
LRU (Front)                MRU (Back)
     ↓                          ↓
  [(2, 20)] <=============> [(3, 30)]
```
* Key `1` has been evicted permanently.

---

### Step 6: `get(2)`
```cpp
cache.get(2);
```
* Key `2` exists. Its value is `20`.
* Return `20`.
* **Important:** Since key `2` was accessed, it must move to the back (MRU)!
```text
Before:  [(2, 20)] <===> [(3, 30)]
After:   [(3, 30)] <===> [(2, 20)]
           ↑               ↑
      LRU (Front)     MRU (Back)
```

---

### Step 7: `get(1)`
```cpp
cache.get(1);
```
* Search for key `1`. Key `1` was evicted in Step 5!
* Return `-1`.
* Cache remains unchanged: `[(3, 30), (2, 20)]`.

---

## 📊 Complete Dry Run State Transition Table

| Step | Operation | Cache State (Front = LRU $\to$ Back = MRU) | Evicted Key | Return Value | Notes |
|:---:|:---|:---:|:---:|:---:|---|
| **1** | `LRUCache(2)` | `[]` | *None* | `null` | Capacity initialized to 2 |
| **2** | `put(1, 10)` | `[(1, 10)]` | *None* | `null` | Inserted at back |
| **3** | `get(1)` | `[(1, 10)]` | *None* | `10` | Hit; recency refreshed |
| **4** | `put(2, 20)` | `[(1, 10), (2, 20)]` | *None* | `null` | Inserted at back (MRU) |
| **5** | `put(3, 30)` | `[(2, 20), (3, 30)]` | **Key 1** | `null` | Cache full; Front (Key 1) evicted |
| **6** | `get(2)` | `[(3, 30), (2, 20)]` | *None* | `20` | Hit; Key 2 spliced to back |
| **7** | `get(1)` | `[(3, 30), (2, 20)]` | *None* | `-1` | Miss; Key 1 does not exist |

### Final Output Array
```text
[null, null, 10, null, null, 20, -1]
```

---

# 4. 🛠️ Understand Which Data Structures We Need

The requirement is strict:
> Both `get()` and `put()` must run in **$O(1)$ average time complexity**.

We need two core capabilities:
1. **Find a key quickly ($O(1)$ lookup)**.
2. **Maintain and reorder recently used items in $O(1)$ time**.

### Evaluation of Candidate Data Structures

| Candidate | Lookup | Order Tracking / Reordering | Eviction of Oldest | Verdict |
|---|---|---|---|---|
| **Vector / Array** | $O(n)$ linear scan | $O(n)$ shifting elements | $O(n)$ erase from front | ❌ Too slow |
| **Hash Map only** | $O(1)$ average | ❌ No order preserved | ❌ Cannot find LRU key in $O(1)$ | ❌ Insufficient |
| **Doubly Linked List only** | ❌ $O(n)$ traversal | $O(1)$ node splicing | $O(1)$ remove from front | ❌ Lookup too slow |
| **Hash Map + Doubly Linked List** | **$O(1)$** via map | **$O(1)$** via `splice()` | **$O(1)$** via `pop_front()` | **✅ Optimal Solution** |

### The Winning Combination

```text
                  HASH MAP (unordered_map)
          ┌──────────────────────────────────────┐
          │ Key 1  ──>  Iterator to Node (1, 10) │
          │ Key 2  ──>  Iterator to Node (2, 20) │
          │ Key 3  ──>  Iterator to Node (3, 30) │
          └──────────────────────────────────────┘
                             │
                             ▼
               DOUBLY LINKED LIST (std::list)

  LRU (Front)                                          MRU (Back)
       ↓                                                    ↓
   [(1, 10)]   <=========>   [(2, 20)]   <=========>   [(3, 30)]
```

* **`std::list<pair<int, int>> cacheList`:** Maintains usage order. Front = LRU, Back = MRU.
* **`std::unordered_map<int, std::list<pair<int, int>>::iterator> cacheMap`:** Maps each key directly to its node's iterator in the linked list.

### Why Store Iterators?

When `get(key)` is invoked:
1. `cacheMap[key]` directly gives us the list iterator in $O(1)$ average time.
2. We read the value from `node->second`.
3. We move the node to the back of the list in $O(1)$ using `cacheList.splice()`.
4. **Crucial insight:** `std::list::splice` transfers the existing node without reallocating memory and **does not invalidate iterators** pointing to that node.

---

# 5. 💻 Write the Complete C++ Code

Here is the clean, complete C++ solution using standard, portable C++ headers:

```cpp
#include <iostream>
#include <list>
#include <unordered_map>
#include <utility>
#include <iterator>

class LRUCache {
private:
    int capacity;

    // Stores {key, value} pairs
    // Front = Least Recently Used (LRU)
    // Back  = Most Recently Used (MRU)
    std::list<std::pair<int, int>> cacheList;

    // Maps each key to its iterator position in the list
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> cacheMap;

public:
    // Initialize cache capacity
    explicit LRUCache(int cap) : capacity(cap < 0 ? 0 : cap) {}

    int get(int key) {
        // Return -1 if key does not exist
        auto it = cacheMap.find(key);
        if (it == cacheMap.end()) {
            return -1;
        }

        // Get the node iterator of the requested key
        auto node = it->second;

        // Extract its value
        int value = node->second;

        // Move the node to the back (most recently used) in O(1)
        cacheList.splice(cacheList.end(), cacheList, node);

        return value;
    }

    void put(int key, int value) {
        // Handle zero or negative capacity cache
        if (capacity <= 0) {
            return;
        }

        // Case 1: Key already exists
        auto it = cacheMap.find(key);
        if (it != cacheMap.end()) {
            auto node = it->second;

            // Update its value
            node->second = value;

            // Move the updated node to the back (MRU)
            cacheList.splice(cacheList.end(), cacheList, node);
            return;
        }

        // Case 2: Key does not exist
        // If cache is full, evict the least recently used key from the front
        if (static_cast<int>(cacheList.size()) >= capacity) {
            // Get the key at the front (LRU)
            int leastUsedKey = cacheList.front().first;

            // Remove the key from the map
            cacheMap.erase(leastUsedKey);

            // Remove the node from the list
            cacheList.pop_front();
        }

        // Insert the new key-value pair at the back (MRU)
        cacheList.push_back({key, value});

        // Store the new node's iterator in the map
        cacheMap[key] = std::prev(cacheList.end());
    }
};
```

---

# 6. 📖 Understand the Code Line by Line

## Part 1: Portable Headers
```cpp
#include <iostream>
#include <list>
#include <unordered_map>
#include <utility>
#include <iterator>
```
* `<list>`: Provides `std::list` (doubly linked list).
* `<unordered_map>`: Provides hash table lookup in $O(1)$ average time.
* `<utility>`: Provides `std::pair`.
* `<iterator>`: Provides `std::prev`.

---

## Part 2: Member Variables
```cpp
private:
    int capacity;
    std::list<std::pair<int, int>> cacheList;
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> cacheMap;
```
* `capacity`: Maximum allowed elements.
* `cacheList`: Doubly linked list storing `std::pair<int, int>` `{key, value}`.
  * **Front:** Least Recently Used (first to be evicted).
  * **Back:** Most Recently Used.
* `cacheMap`: Hash table storing `key` $\to$ `iterator pointing to the list node`.

---

## Part 3: Constructor
```cpp
explicit LRUCache(int cap) : capacity(cap < 0 ? 0 : cap) {}
```
Initializes the capacity. Guards against negative inputs by clamping to 0.

---

## Part 4: `get(key)` Function

```cpp
int get(int key) {
    auto it = cacheMap.find(key);
    if (it == cacheMap.end()) {
        return -1;
    }

    auto node = it->second;
    int value = node->second;

    cacheList.splice(cacheList.end(), cacheList, node);

    return value;
}
```

1. **`cacheMap.find(key)`**: Checks if the key is present in $O(1)$ average time.
2. If `it == cacheMap.end()`, the key is missing $\implies$ return `-1`.
3. `auto node = it->second`: Retrieves the iterator pointing to the node in `cacheList`.
4. `int value = node->second`: Reads the cached value.
5. **`cacheList.splice(cacheList.end(), cacheList, node)`**:
   * Moves `node` to immediately before `cacheList.end()` (i.e., to the back of the list).
   * **Why is `splice()` so powerful?** It simply unlinks the node's pointers (`prev` and `next`) and relinks them at the new destination in $O(1)$ time. No heap allocation, no copy, and existing iterators remain valid!
6. `return value`: Returns the stored value.

---

## Part 5: `put(key, value)` Function

```cpp
void put(int key, int value) {
    if (capacity <= 0) {
        return;
    }

    auto it = cacheMap.find(key);
    if (it != cacheMap.end()) {
        auto node = it->second;
        node->second = value;
        cacheList.splice(cacheList.end(), cacheList, node);
        return;
    }

    if (static_cast<int>(cacheList.size()) >= capacity) {
        int leastUsedKey = cacheList.front().first;
        cacheMap.erase(leastUsedKey);
        cacheList.pop_front();
    }

    cacheList.push_back({key, value});
    cacheMap[key] = std::prev(cacheList.end());
}
```

### Case 1: Key already exists
* Find the node via `cacheMap`.
* Update `node->second = value`.
* Move the node to the back (MRU) with `splice()`.
* Return immediately.

### Case 2: Key does not exist
1. **Eviction Check:** If `cacheList.size() >= capacity`:
   * The least recently used element is at `cacheList.front()`.
   * Extract `leastUsedKey = cacheList.front().first`.
   * Erase `leastUsedKey` from `cacheMap`.
   * Call `cacheList.pop_front()` to free the LRU node.
2. **Insertion:**
   * Call `cacheList.push_back({key, value})` to place the new item at the back (MRU).
   * Get an iterator to the newly inserted node: `std::prev(cacheList.end())` (since `end()` points past the last element, `prev(end())` points to the last element).
   * Save this iterator in the map: `cacheMap[key] = std::prev(cacheList.end());`.

---

# 7. ⏱️ Analyze Time and Space Complexity

## Time Complexity

| Operation | Action | Time Complexity |
|---|---|:---:|
| `get(key)` | Map lookup | $O(1)$ average |
| | Read value from iterator | $O(1)$ |
| | `cacheList.splice()` | $O(1)$ |
| **Total `get()`** | | **$O(1)$ average** |
| `put(key, value)` | Map lookup | $O(1)$ average |
| | Node update or list insertion | $O(1)$ |
| | Eviction (`pop_front` + `map.erase`) | $O(1)$ |
| **Total `put()`** | | **$O(1)$ average** |

> [!NOTE]
> Hash map lookups and deletions are $O(1)$ on average. In adversarial hash collision scenarios, they degrade to $O(n)$, but with standard uniform hashing distributions, they achieve true constant time.

---

## Space Complexity

* `cacheList` stores at most $C$ elements (where $C = \text{capacity}$).
* `cacheMap` stores at most $C$ key-iterator entries.
* **Total Space Complexity:** $O(C) = O(\text{capacity})$.

---

# 8. 🎯 Final Revision Notes for Interviews

### ⭐ Five Most Important Lines to Remember

1. **Check if key exists in $O(1)$:**
   ```cpp
   if (cacheMap.find(key) != cacheMap.end())
   ```
2. **Move an existing node to the MRU position without copying or invalidating iterators:**
   ```cpp
   cacheList.splice(cacheList.end(), cacheList, node);
   ```
3. **Retrieve and erase the LRU key when full:**
   ```cpp
   int leastUsedKey = cacheList.front().first;
   cacheMap.erase(leastUsedKey);
   cacheList.pop_front();
   ```
4. **Insert the new MRU element at the back:**
   ```cpp
   cacheList.push_back({key, value});
   ```
5. **Save the newly inserted node's iterator in the map:**
   ```cpp
   cacheMap[key] = std::prev(cacheList.end());
   ```

---

## Summary Comparison

| Property | LRU Cache Specification |
|---|---|
| **Primary Data Structure** | Doubly Linked List (`std::list<pair<int, int>>`) |
| **Secondary Data Structure**| Hash Map (`std::unordered_map<int, list<...>::iterator>`) |
| **Front of List** | **Least Recently Used (LRU)** — Eviction candidate |
| **Back of List** | **Most Recently Used (MRU)** — Latest insert/access |
| **Lookup Time** | $O(1)$ average |
| **Insert / Update Time** | $O(1)$ average |
| **Eviction Time** | $O(1)$ |
| **Space Overhead** | $O(\text{capacity})$ |
