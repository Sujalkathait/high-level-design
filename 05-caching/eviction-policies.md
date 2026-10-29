# 03. Cache Eviction Policies (LRU vs. LFU)

[🏠 Home](../README.md) > [05. Caching](./README.md) > **Cache Eviction Policies**

---

## 1. What is an Eviction Policy?

An **Eviction Policy** is the algorithm that determines **which items should be removed when a cache reaches its maximum capacity or memory limit**.

Because RAM is finite and expensive, an in-memory cache cannot expand indefinitely. When capacity is saturated, the cache must choose which existing records to sacrifice to accommodate new writes.

> [!IMPORTANT]
> **Expiration vs. Eviction: The Core Difference**
> * **Expiration (TTL):** Removes items passively or actively when their configured **time-to-live lifespan expires** (e.g., `EXPIRE key 300`).
> * **Eviction:** Forcibly purges valid, non-expired keys when **capacity is exhausted** based on access patterns.

---

## 2. LRU: Least Recently Used

**LRU** evicts the item that has not been accessed for the longest period of time.

* **Core Premise:** Temporal locality—data accessed recently is highly likely to be accessed again in the near future.
* **Standard Implementation:** A **Hash Map** combined with a **Doubly Linked List** to achieve strict $O(1)$ lookup, update, and eviction operations.

```mermaid
flowchart LR
    Head["Head: Most Recently Used"] <--> A["Item A"]
    A <--> C["Item C"]
    C <--> D["Item D"]
    D <--> Tail["Tail: Least Recently Used"]
    Tail -.->|"Memory Full: Purged First"| Evicted["Eviction Candidate (D)"]
```

### Step-by-Step LRU Walkthrough Example

Assume a cache with **Capacity = 3**:

| Step | Operation | Cache Contents (Most $\to$ Least Recent) | Evicted Item | Notes |
|:---:|---|:---:|:---:|---|
| 1 | **Access A** | `[A]` | None | Insert A at head |
| 2 | **Access B** | `[B, A]` | None | Insert B at head |
| 3 | **Access C** | `[C, B, A]` | None | Cache reaches max capacity (3/3) |
| 4 | **Access A** | `[A, C, B]` | None | A is moved back to head (B becomes least recent) |
| 5 | **Insert D** | `[D, A, C]` | **B** | Cache is full! **B is evicted** because it was accessed longest ago |

**Final Cache State:** `[D, A, C]`

---

## 3. LFU: Least Frequently Used

**LFU** evicts the item that has the **lowest total access frequency counter**. When two or more items have identical frequencies, a tie-breaking rule (such as age or recency) is applied.

* **Core Premise:** Items that are popular historically will remain popular in the future.
* **Standard Implementation:** Hash map of keys coupled with frequency buckets or a min-heap.

### Step-by-Step LFU Walkthrough Example

Assume a cache with existing access counts:
* **Item A:** Accessed $10$ times
* **Item B:** Accessed $2$ times
* **Item C:** Accessed $5$ times

```mermaid
flowchart LR
    subgraph FrequencyCounters ["LFU Access Frequency Table"]
        A["Item A: 10 Hits"]
        C["Item C: 5 Hits"]
        B["Item B: 2 Hits (Lowest Frequency)"]
    end
    B -.->|"Capacity Full"| Evict["Item B Evicted"]
```

When a **New Item** arrives and the cache is full:
* **Candidate for Eviction:** **Item B** (Lowest frequency count: 2).
* **Aging Mechanisms:** Production LFU systems (like Redis LFU) implement **frequency decay / aging** so that historically popular items that are no longer accessed gradually lose their counts over time and do not dominate memory forever.

---

## 4. Head-to-Head Comparison: LRU vs. LFU

| Feature | LRU (Least Recently Used) | LFU (Least Frequently Used) |
|---|---|---|
| **Eviction Rule** | Least recently accessed item | Least frequently accessed item |
| **What It Tracks** | Access recency (timestamp / list position) | Access frequency counter |
| **Implementation** | Hash Map + Doubly Linked List | Frequency counters, frequency buckets, or Min-Heap |
| **Core Strength** | Adapts rapidly to shifting access trends & bursts | Preserves persistently popular hot items |
| **Main Weakness** | A sudden one-time table scan can evict useful hot items | Historic popular items may remain too long; new items risk instant eviction |
| **Best Suited For** | Workloads with strong **temporal locality** (social feeds, blogs) | Workloads with stable, persistent popularity patterns |

---

## 5. Other Eviction Policies

1. **FIFO (First-In, First-Out):**
   * Evicts the oldest inserted item regardless of how many times it was accessed.
   * *Trade-off:* Trivial to implement, but frequently evicts foundational cached data.

   ```mermaid
   flowchart LR
       In["New Item Enters"] --> Q["FIFO Queue: [ Item 4 -> Item 3 -> Item 2 -> Item 1 ]"]
       Q --> Out["Oldest Item 1 Evicted First"]
   ```
2. **Random Eviction:**
   * Evicts a randomly selected item uniformly.
   * *Trade-off:* Zero metadata tracking overhead, but risks evicting ultra-hot records.
3. **TTL-Based Eviction (Volatile-TTL):**
   * Selects and evicts the key with the shortest remaining Time-To-Live.
4. **TinyLFU:**
   * Uses space-efficient Count-Min Sketches to maintain frequency estimates for millions of keys, making highly accurate admission and eviction decisions with minimal memory overhead (utilized in modern caches like Caffeine).
5. **ARC (Adaptive Replacement Cache):**
   * Dynamically and automatically balances between recency (LRU) and frequency (LFU) by maintaining two tracked lists and tuning their sizes in real time based on cache hit history.

---

## 6. Comprehensive Dry Runs & State Transition Tables

### 6.1 LRU Cache Complete Dry Run (Capacity = 3)

We initialize an LRU cache with **Capacity = 3** and execute the following sequential operations:
1. `put(1, 10)`
2. `put(2, 20)`
3. `put(3, 30)`
4. `get(1)`
5. `put(4, 40)`

#### State Transition Table:
| Step | Operation | Cache Contents (Most $\to$ Least Recent) | MRU Key | LRU Key | Evicted Key | Detailed Internal Explanation |
|:---:|---|:---:|:---:|:---:|:---:|---|
| **1** | `put(1, 10)` | `[1:10]` | `1` | `1` | *None* | Cache empty. Node `1` inserted at Head. Map stores pointer to Node `1`. |
| **2** | `put(2, 20)` | `[2:20, 1:10]` | `2` | `1` | *None* | Node `2` inserted at Head. Node `1` pushed down. Capacity = 2/3. |
| **3** | `put(3, 30)` | `[3:30, 2:20, 1:10]` | `3` | `1` | *None* | Node `3` inserted at Head. Cache reaches full capacity (3/3). |
| **4** | `get(1)` $\to$ `10` | `[1:10, 3:30, 2:20]` | `1` | `2` | *None* | Cache Hit! Node `1` spliced from tail and moved to Head (now MRU). **Key 2 is now LRU!** |
| **5** | `put(4, 40)` | `[4:40, 1:10, 3:30]` | `4` | `3` | **Key 2** | Capacity full (3/3). **Key 2 (LRU at tail) is evicted**. Key 4 inserted at Head. |

#### Key Architectural Insights from Dry Run:
* **Why was Key 2 evicted instead of Key 1?** Although Key 1 was inserted earlier than Key 2, `get(1)` at step 4 refreshed Key 1's recency, promoting it to the Head. Key 2 became the least recently used item.
* **Time Complexity:** Both `get()` and `put()` run in strictly **$O(1)$** time because the `unordered_map` provides instant node lookup, and the `std::list` allows $O(1)$ node splicing without pointer loops.

---

### 6.2 LFU Cache Complete Dry Run (Capacity = 2)

We initialize an LFU cache with **Capacity = 2** and execute:
1. `put(1, 100)`
2. `put(2, 200)`
3. `get(1)`
4. `put(3, 300)`

#### State Transition Table:
| Step | Operation | Frequency Buckets | `minFreq` | Evicted Key | Detailed Internal Explanation |
|:---:|---|---|:---:|:---:|---|
| **1** | `put(1, 100)` | `Freq 1: [1]` | `1` | *None* | Key 1 inserted with freq=1. Placed in bucket 1. |
| **2** | `put(2, 200)` | `Freq 1: [2, 1]` | `1` | *None* | Key 2 inserted with freq=1. Cache at capacity (2/2). |
| **3** | `get(1)` $\to$ `100` | `Freq 1: [2]`, `Freq 2: [1]` | `1` | *None* | Key 1 frequency increments to 2 ($1 \to 2$). Moved from bucket 1 to bucket 2. |
| **4** | `put(3, 300)` | `Freq 1: [3]`, `Freq 2: [1]` | `1` | **Key 2** | Capacity full. Evicts from `minFreq` bucket (`Freq 1`). **Key 2 evicted**! Key 3 inserted into `Freq 1`. |

---

### 6.3 FIFO Cache Complete Dry Run (Capacity = 3)

We initialize a FIFO cache with **Capacity = 3** and execute:
1. `put(A)`
2. `put(B)`
3. `put(C)`
4. `get(A)` (accessed again!)
5. `put(D)`

#### State Transition Table:
| Step | Operation | FIFO Queue Contents | Oldest Item | Evicted Item | Detailed Internal Explanation |
|:---:|---|:---:|:---:|:---:|---|
| **1** | `put(A)` | `[A]` | `A` | *None* | A enters queue |
| **2** | `put(B)` | `[A, B]` | `A` | *None* | B enters queue |
| **3** | `put(C)` | `[A, B, C]` | `A` | *None* | Capacity full (3/3) |
| **4** | `get(A)` | `[A, B, C]` | `A` | *None* | Cache hit on A! In FIFO, **accesses do NOT change queue order** |
| **5** | `put(D)` | `[B, C, D]` | `B` | **Key A** | Despite being accessed in step 4, **A is evicted because it arrived first** |

> [!TIP]
> **Working C++ Implementations Available:**
> Check the dedicated code directory for clean C++17 implementations:
> * [`code/lru_cache.cpp`](./code/lru_cache.cpp) — $O(1)$ LRU Cache using `std::list` + `std::unordered_map`.
> * [`code/lfu_cache.cpp`](./code/lfu_cache.cpp) — $O(1)$ LFU Cache using frequency bucket lists + `minFreq`.
> * [`code/fifo_cache.cpp`](./code/fifo_cache.cpp) — $O(1)$ FIFO Cache using `std::queue` + `std::unordered_map`.

---

## 7. Related Topics & Next Steps

* [Cache Fundamentals](./cache-fundamentals.md) — Hits, misses, and the 80/20 Pareto sizing rule.
* [Caching Strategies](./caching-strategies.md) — Cache-Aside, Write-Through, Write-Back, and Write-Around.
* [Redis vs. Memcached](./redis-vs-memcached.md) — Comparing distributed cache engines.
* [Common Caching Problems](./common-caching-problems.md) — Mitigating stampedes and avalanches.

---

| ⬅️ Previous Topic | 🏠 Index | Next Topic ➡️ |
|:--- |:---:| ---:|
| [Caching Strategies](./caching-strategies.md) | [05. Caching Index](./README.md) | [Redis vs. Memcached ➡️](./redis-vs-memcached.md) |
