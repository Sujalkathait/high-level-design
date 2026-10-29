# 🚀 LFU Cache in C++ — Complete Beginner-Friendly Explanation

[🏠 Home](../README.md) > [05. Caching](./README.md) > **LFU Cache in C++**

---

**Problem:** Design and implement a Least Frequently Used (LFU) Cache.

**Difficulty:** Hard  
**Approach:** Hash Map + Frequency Map + Doubly Linked Lists  
**Time Complexity:** $O(1)$ average for `get()` and `put()`  
**Space Complexity:** $O(\text{capacity})$  
**Executable Source Code:** [`code/lfu_cache.cpp`](./code/lfu_cache.cpp)

---

## 📚 Table of Contents

1. [🧠 1. Understand What an LFU Cache Is](#1--understand-what-an-lfu-cache-is)
2. [💡 2. Understand the Problem with a Real-Life Example](#2--understand-the-problem-with-a-real-life-example)
3. [🔍 3. Dry Run the Step-by-Step Example](#3--dry-run-the-given-example)
4. [🛠️ 4. Understand Which Data Structures We Need](#4-️-understand-which-data-structures-we-need)
5. [💻 5. Complete C++ Code Implementation](#5--write-the-complete-c-code)
6. [📖 6. Line-by-Line Code Explanation](#6--understand-the-code-line-by-line)
7. [⏱️ 7. Time and Space Complexity Analysis](#7-️-analyze-time-and-space-complexity)
8. [🎯 8. Final Revision Notes for Interviews](#8--final-revision-notes-for-interviews)
9. [🧠 9. The Complete LFU Algorithm in Simple English](#9--the-complete-lfu-algorithm-in-simple-english)
10. [🏆 10. Final Summary](#10--final-summary)

---

# 1. 🧠 Understand What an LFU Cache Is

## What does LFU mean?

**LFU = Least Frequently Used**

It is a cache eviction technique that removes the item used the fewest times when the cache becomes full.

In simple English:

> When the cache is full, remove the key that has been accessed the fewest times.

### The Tie-Breaking Rule

**If two or more keys have the same minimum frequency, remove the least recently used (LRU) key among them.**

This is the key distinction between LFU and LRU.

### LRU vs. LFU

| Feature | LRU Cache | LFU Cache |
|---|---|---|
| **Full form** | Least Recently Used | Least Frequently Used |
| **Main rule** | Remove the key used least recently | Remove the key used the fewest times |
| **Tracks** | Recent usage order | Usage frequency + recent usage order |
| **Tie-breaking** | Not needed | Uses LRU among keys with identical frequency |

### Example

Suppose the cache contains these keys:

| Key | Value | Frequency |
|---|---:|---:|
| 1 | 100 | 5 |
| 2 | 200 | 2 |
| 3 | 300 | 4 |

The cache is full, and we want to insert key `4`. Which key should be removed?

**Answer: Key 2**, because its frequency is only `2`.

```text
Key 1 → Frequency 5
Key 2 → Frequency 2  <-- Remove this key (Lowest frequency)
Key 3 → Frequency 4
```

Now suppose the cache looks like this:

| Key | Value | Frequency |
|---|---:|---:|
| 1 | 100 | 3 |
| 2 | 200 | 1 |
| 3 | 300 | 1 |

Keys `2` and `3` have the exact same frequency (`1`). We must remove whichever of those two keys was used less recently. This is LFU's tie-breaking rule.

---

# 2. 💡 Understand the Problem with a Real-Life Example

Imagine you have a study table that can hold only **three books**.

You have three books:
- Mathematics
- Physics
- Chemistry

Every time you study a book, its usage counter increases. Initially, all books have a frequency of `1`.

| Book | Usage Frequency |
|---|---:|
| Mathematics | 1 |
| Physics | 1 |
| Chemistry | 1 |

Now suppose you study Mathematics 3 times, Physics 2 times, and Chemistry 1 time:

| Book | Usage Frequency |
|---|---:|
| Mathematics | 3 |
| Physics | 2 |
| Chemistry | 1 |

Now your table is full, and you want to add an **English** book. Which book should you remove?

**Chemistry**, because it has the lowest usage frequency (`1`).

After removing Chemistry and placing English:

| Book | Usage Frequency |
|---|---:|
| Mathematics | 3 |
| Physics | 2 |
| English | 1 |

English starts with a frequency of `1` because its insertion counts as its first use.

## What if two books have the same frequency?

Suppose the table contains:

| Book | Frequency | Last Used |
|---|---:|---|
| Mathematics | 2 | Recently |
| Physics | 2 | A longer time ago |
| Chemistry | 3 | Recently |

You want to insert another book. Mathematics and Physics share the minimum frequency (`2`). Which one should be removed?

**Physics**, because it was used less recently than Mathematics.

This gives us the **two fundamental rules** of an LFU Cache:
1. **Primary rule:** Remove the key with the lowest frequency.
2. **Tie-breaker:** If frequencies are equal, remove the least recently used (LRU) key among them.

---

## Understanding the Required Functions

```cpp
class LFUCache {
public:
    LFUCache(int capacity) {

    }

    int get(int key) {

    }

    void put(int key, int value) {

    }
};
```

### Function 1: Constructor
```cpp
LFUCache(int capacity)
```
Initializes the cache with the given capacity.
Example: `LFUCache cache(2);` $\implies$ capacity = 2, cache is empty.

### Function 2: `get(key)`
```cpp
int get(int key)
```
* If the key exists:
  * Return its value.
  * Increase its frequency by `1`.
  * Update its recency position within its new frequency group.
* If the key doesn't exist: Return `-1`.

### Function 3: `put(key, value)`
```cpp
void put(int key, int value)
```
* **Case 1: Key already exists:** Update its value and increase its frequency by `1`.
* **Case 2: Key does not exist, space available:** Insert the new key-value pair with initial frequency `1`.
* **Case 3: Key does not exist, cache is full:** Evict the least frequently used key (breaking ties with LRU), then insert the new key-value pair with frequency `1`.

---

# 3. 🔍 Dry Run the Given Example

Let's dry run the standard test sequence step by step.

### Input Sequence
```cpp
LFUCache lfu(2);

lfu.put(1, 1);
lfu.put(2, 2);
lfu.get(1);
lfu.put(3, 3);
lfu.get(2);
lfu.get(3);
lfu.put(4, 4);
lfu.get(1);
lfu.get(3);
lfu.get(4);
```

> **Convention within each frequency group:**  
> **LEFT (Front)** = Least Recently Used (LRU)  
> **RIGHT (Back)** = Most Recently Used (MRU)

---

### Step 1: Create Cache
```cpp
LFUCache lfu(2);
```
* `capacity = 2`
* `cache = {}` (empty)

---

### Step 2: `put(1, 1)`
```cpp
lfu.put(1, 1);
```
* Insert key `1` with value `1`, frequency `1`.
* `Frequency 1: [1]`

---

### Step 3: `put(2, 2)`
```cpp
lfu.put(2, 2);
```
* Insert key `2` with value `2`, frequency `1`.
* `Frequency 1: [1, 2]`
```text
Frequency 1:
LRU                  MRU
 ↓                    ↓
[1]  <-------------> [2]
```
* Cache is now full (`size = 2 / 2`).

---

### Step 4: `get(1)`
```cpp
lfu.get(1);
```
* Key `1` exists $\implies$ return `1`.
* Frequency of key `1` increments: $1 \to 2$.
```text
Frequency 1: [2]
Frequency 2: [1]
```

---

### Step 5: `put(3, 3)`
```cpp
lfu.put(3, 3);
```
* Cache is full. Key `1` has freq 2; Key `2` has freq 1.
* Lowest frequency is `1` $\implies$ **Key 2 is evicted!**
* Insert key `3` with initial frequency `1`.
```text
Frequency 1: [3]
Frequency 2: [1]
```

---

### Step 6: `get(2)`
```cpp
lfu.get(2);
```
* Key `2` was evicted in Step 5 $\implies$ **return `-1`**.
* Cache state remains unchanged.

---

### Step 7: `get(3)`
```cpp
lfu.get(3);
```
* Key `3` exists $\implies$ return `3`.
* Frequency of key `3` increments: $1 \to 2$.
```text
Frequency 2: [1, 3]
              ↑  ↑
             LRU MRU
```

---

### Step 8: `put(4, 4)`
```cpp
lfu.put(4, 4);
```
* Cache is full. Keys in cache: Key `1` (freq 2), Key `3` (freq 2).
* Both have the same minimum frequency (`2`).
* **Tie-break with LRU:** Key `1` is at the front (LRU) $\implies$ **Key 1 is evicted!**
* Insert key `4` with initial frequency `1`.
```text
Frequency 1: [4]
Frequency 2: [3]
```

---

### Step 9: `get(1)`
```cpp
lfu.get(1);
```
* Key `1` was evicted in Step 8 $\implies$ **return `-1`**.

---

### Step 10: `get(3)`
```cpp
lfu.get(3);
```
* Key `3` exists $\implies$ return `3`.
* Frequency increments: $2 \to 3$.
```text
Frequency 1: [4]
Frequency 3: [3]
```

---

### Step 11: `get(4)`
```cpp
lfu.get(4);
```
* Key `4` exists $\implies$ return `4`.
* Frequency increments: $1 \to 2$.
```text
Frequency 2: [4]
Frequency 3: [3]
```

---

## 📊 Complete Dry Run State Transition Table

| Step | Operation | Key 1 Freq | Key 2 Freq | Key 3 Freq | Key 4 Freq | Min Freq | Evicted Key | Return Value |
|:---:|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| **1** | `LFUCache(2)` | — | — | — | — | 0 | *None* | `null` |
| **2** | `put(1, 1)` | 1 | — | — | — | 1 | *None* | `null` |
| **3** | `put(2, 2)` | 1 | 1 | — | — | 1 | *None* | `null` |
| **4** | `get(1)` | 2 | 1 | — | — | 1 | *None* | **1** |
| **5** | `put(3, 3)` | 2 | Evicted | 1 | — | 1 | **Key 2** | `null` |
| **6** | `get(2)` | 2 | Not found | 1 | — | 1 | *None* | **-1** |
| **7** | `get(3)` | 2 | Not found | 2 | — | 2 | *None* | **3** |
| **8** | `put(4, 4)` | Evicted | Not found | 2 | 1 | 1 | **Key 1** | `null` |
| **9** | `get(1)` | Not found | Not found | 2 | 1 | 1 | *None* | **-1** |
| **10** | `get(3)` | Not found | Not found | 3 | 1 | 1 | *None* | **3** |
| **11** | `get(4)` | Not found | Not found | 3 | 2 | 2 | *None* | **4** |

### Final Output Array
```text
[null, null, null, 1, null, -1, 3, null, -1, 3, 4]
```

---

# 4. 🛠️ Understand Which Data Structures We Need

The problem requires both `get()` and `put()` to run in **$O(1)$ average time**.

We must solve three challenges simultaneously:
1. **Find a key's value in $O(1)$**
2. **Find a key's frequency in $O(1)$**
3. **Find the least frequently used key (and its LRU tie-breaker) in $O(1)$**

### The 4 Data Structures + 1 Variable Architecture

```text
                                  LFU CACHE
                                      |
         ┌───────────────────┬────────┴────────────┬───────────────────┐
         │                   │                     │                   │
         ▼                   ▼                     ▼                   ▼
    keyToValue        keyToFrequency        frequencyToKeys      keyToIterator
    Key → Value       Key → Frequency       Freq → list<Key>     Key → iterator
                                                   │
                                                   ▼
                                           minimumFrequency
```

| Structure | Type | Purpose |
|---|---|---|
| `keyToValue` | `unordered_map<int, int>` | Retrieves a key's value in $O(1)$ |
| `keyToFrequency` | `unordered_map<int, int>` | Retrieves a key's frequency in $O(1)$ |
| `frequencyToKeys` | `unordered_map<int, list<int>>` | Groups keys by frequency; maintains LRU order |
| `keyToIterator` | `unordered_map<int, list<int>::iterator>` | Direct pointer to a key's node in its frequency list for $O(1)$ removal |
| `minimumFrequency` | `int` | Tracks the lowest active frequency in the cache |

### Why is `keyToIterator` Essential?
When a key is accessed, its frequency increments from $f$ to $f+1$. We must remove it from the list of frequency $f$ and insert it into frequency $f+1$. Without an iterator map, searching for that key inside `list<int>` would take $O(n)$ time! Storing `list<int>::iterator` enables exact $O(1)$ node removal via `list::erase(iterator)`.

---

# 5. 💻 Write the Complete C++ Code

Here is the clean, beginner-friendly C++ solution using standard, portable headers:

```cpp
#include <iostream>
#include <list>
#include <unordered_map>
#include <iterator>

class LFUCache {
private:
    int capacity;
    int minimumFrequency;

    // Stores each key's value
    std::unordered_map<int, int> keyToValue;

    // Stores each key's usage frequency
    std::unordered_map<int, int> keyToFrequency;

    // Stores keys grouped by frequency:
    // Front = Least Recently Used (LRU)
    // Back  = Most Recently Used (MRU)
    std::unordered_map<int, std::list<int>> frequencyToKeys;

    // Stores each key's position in its frequency list
    std::unordered_map<int, std::list<int>::iterator> keyToIterator;

    // Helper: Increases a key's frequency and moves it to the new frequency list
    void increaseFrequency(int key) {
        int oldFrequency = keyToFrequency[key];

        // 1. Remove the key from its current frequency list in O(1)
        frequencyToKeys[oldFrequency].erase(keyToIterator[key]);

        // 2. If the old frequency list became empty, clean up the bucket
        if (frequencyToKeys[oldFrequency].empty()) {
            frequencyToKeys.erase(oldFrequency);

            // If the minimum frequency bucket is now empty, increment it
            if (minimumFrequency == oldFrequency) {
                minimumFrequency++;
            }
        }

        // 3. Increment the key's frequency
        int newFrequency = oldFrequency + 1;
        keyToFrequency[key] = newFrequency;

        // 4. Append key to the back of its new frequency list (MRU within bucket)
        frequencyToKeys[newFrequency].push_back(key);

        // 5. Store the new iterator position
        keyToIterator[key] = std::prev(frequencyToKeys[newFrequency].end());
    }

public:
    // Initialize the cache with a maximum capacity
    explicit LFUCache(int cap) : capacity(cap < 0 ? 0 : cap), minimumFrequency(0) {}

    int get(int key) {
        // Return -1 if key does not exist
        if (keyToValue.find(key) == keyToValue.end()) {
            return -1;
        }

        // Increase its frequency and update its recency position
        increaseFrequency(key);

        // Return the stored value
        return keyToValue[key];
    }

    void put(int key, int value) {
        // Guard against zero-capacity cache
        if (capacity == 0) {
            return;
        }

        // Case 1: Key already exists
        if (keyToValue.find(key) != keyToValue.end()) {
            keyToValue[key] = value;
            increaseFrequency(key);
            return;
        }

        // Case 2: Cache is full -> Evict the least frequently used key
        if (static_cast<int>(keyToValue.size()) == capacity) {
            // Front of minimumFrequency list is the LRU key among the least frequent
            int leastUsedKey = frequencyToKeys[minimumFrequency].front();

            // Remove from the frequency list
            frequencyToKeys[minimumFrequency].pop_front();

            if (frequencyToKeys[minimumFrequency].empty()) {
                frequencyToKeys.erase(minimumFrequency);
            }

            // Remove from all tracking maps
            keyToValue.erase(leastUsedKey);
            keyToFrequency.erase(leastUsedKey);
            keyToIterator.erase(leastUsedKey);
        }

        // Case 3: Insert new key with frequency 1
        keyToValue[key] = value;
        keyToFrequency[key] = 1;

        // Add to frequency 1 list at back (MRU)
        frequencyToKeys[1].push_back(key);

        // Store its iterator
        keyToIterator[key] = std::prev(frequencyToKeys[1].end());

        // A brand new key always resets the minimum frequency to 1
        minimumFrequency = 1;
    }
};
```

---

# 6. 📖 Understand the Code Line by Line

## The `increaseFrequency(key)` Helper Step-by-Step

```text
Get old frequency
       │
       ▼
Remove key from old list using keyToIterator[key] in O(1)
       │
       ▼
Is old list now empty?
       ├── YES ──> Remove frequency bucket from map
       │           If minimumFrequency == oldFrequency ──> minimumFrequency++
       └── NO  ──> Keep bucket
       │
       ▼
newFrequency = oldFrequency + 1
keyToFrequency[key] = newFrequency
       │
       ▼
frequencyToKeys[newFrequency].push_back(key)
keyToIterator[key] = std::prev(frequencyToKeys[newFrequency].end())
```

### Why does `minimumFrequency++` work?
If the key we just accessed was the *only* key with `minimumFrequency`, that frequency list is now empty. Since the key's frequency became `minimumFrequency + 1`, the new minimum frequency across the cache is guaranteed to be `minimumFrequency + 1`!

---

## The `put(key, value)` Logic Step-by-Step

1. **Zero capacity guard:** If `capacity == 0`, return immediately.
2. **Existing key:** If `keyToValue.find(key) != keyToValue.end()`:
   * Update value: `keyToValue[key] = value`.
   * Increment frequency via `increaseFrequency(key)`.
3. **Full capacity eviction:** If `keyToValue.size() == capacity`:
   * The candidate is `frequencyToKeys[minimumFrequency].front()`.
   * Pop from front: `frequencyToKeys[minimumFrequency].pop_front()`.
   * Clean up empty frequency bucket if necessary.
   * Erase from `keyToValue`, `keyToFrequency`, `keyToIterator`.
4. **Insert new key:**
   * Insert `value` into `keyToValue`.
   * Set `keyToFrequency[key] = 1`.
   * Push to `frequencyToKeys[1].push_back(key)`.
   * Save iterator: `keyToIterator[key] = std::prev(frequencyToKeys[1].end())`.
   * Set `minimumFrequency = 1`.

---

# 7. ⏱️ Analyze Time and Space Complexity

## Time Complexity

| Operation | Internal Operations | Time Complexity |
|---|---|:---:|
| `get(key)` | Map lookup, `list::erase`, `list::push_back`, iterator assignment | **$O(1)$ average** |
| `put(key, value)` (update) | Map lookup, value update, `increaseFrequency` | **$O(1)$ average** |
| `put(key, value)` (evict + insert) | Front element read, `pop_front`, 3 map erasures, `push_back`, iterator assignment | **$O(1)$ average** |

* All hash map operations run in $O(1)$ average time.
* Doubly linked list removals by known iterator and insertions at back run in strict $O(1)$ time.

---

## Space Complexity

For a cache of maximum capacity $C$:
* `keyToValue`: Stores at most $C$ elements $\implies O(C)$.
* `keyToFrequency`: Stores at most $C$ elements $\implies O(C)$.
* `frequencyToKeys`: Across all frequency lists combined, exactly $C$ keys are stored $\implies O(C)$.
* `keyToIterator`: Stores at most $C$ iterators $\implies O(C)$.

**Total Space Complexity:** $O(C) + O(C) + O(C) + O(C) = O(C) = O(\text{capacity})$.

---

# 8. 🎯 Final Revision Notes for Interviews

### ⭐ Five Most Important Lines to Remember

1. **Remove node in $O(1)$ from old frequency group:**
   ```cpp
   frequencyToKeys[oldFreq].erase(keyToIterator[key]);
   ```
2. **Promote `minimumFrequency` when old minimum bucket empties:**
   ```cpp
   if (frequencyToKeys[oldFreq].empty() && minimumFrequency == oldFreq) minimumFrequency++;
   ```
3. **Insert key into new frequency group at back (MRU within frequency):**
   ```cpp
   frequencyToKeys[newFreq].push_back(key);
   keyToIterator[key] = std::prev(frequencyToKeys[newFreq].end());
   ```
4. **Find the exact eviction candidate (lowest freq + LRU):**
   ```cpp
   int leastUsedKey = frequencyToKeys[minimumFrequency].front();
   ```
5. **Reset `minimumFrequency` on new insert:**
   ```cpp
   minimumFrequency = 1;
   ```

---

# 9. 🧠 The Complete LFU Algorithm in Simple English

### When `get(key)` is called:
1. If the key does not exist, return `-1`.
2. Call `increaseFrequency(key)` to move it to the next frequency bucket.
3. Return the stored value.

### When `put(key, value)` is called:
1. If `capacity == 0`, do nothing.
2. If the key already exists, update its value and call `increaseFrequency(key)`.
3. If the cache is full:
   * Evict the front element of `frequencyToKeys[minimumFrequency]`.
   * Erase it from all 3 tracking maps.
4. Insert the new key with frequency `1`, push to `frequencyToKeys[1]`, and set `minimumFrequency = 1`.

---

# 10. 🏆 Final Summary

**LFU Cache = Frequency Buckets + LRU Tie-Breaking**

* **Frequency** identifies the group to evict from.
* **Recency order** inside each bucket decides which key to remove when multiple keys share the same frequency.
* Combining **4 hash maps** with **doubly linked lists** achieves strict **$O(1)$ average time** for both `get()` and `put()`.

