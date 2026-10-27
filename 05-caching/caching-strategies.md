# Caching Strategies

> High-Level System Design | Caching & Data Acceleration

---

### 1. What is it?
A Caching Strategy is the design pattern that dictates **how and when data moves between the application, the fast in-memory cache (Redis / Memcached), and the durable database**. Choosing the right strategy determines data freshness, latency, and resilience during database outages.

---

### 2. Core Caching Patterns Compared

```mermaid
flowchart TD
    subgraph CacheAside [1. Cache-Aside / Lazy Loading (Standard)]
        direction TB
        App1[Application] -->|1. Check Cache| C1[(Cache)]
        App1 -->|2. On Miss: Read DB| DB1[(Database)]
        App1 -->|3. Populate Cache| C1
    end

    subgraph WriteThrough [2. Write-Through (Consistent)]
        direction TB
        App2[Application] -->|1. Write Data| C2[(Cache)]
        C2 -->|2. Synchronously Writes| DB2[(Database)]
    end

    subgraph WriteBack [3. Write-Back / Write-Behind (High Write Volume)]
        direction TB
        App3[Application] -->|1. Fast Write| C3[(Cache)]
        C3 -.->|2. Async Batched Flush| DB3[(Database)]
    end
```

---

### 3. Detailed Pattern Breakdown

| Strategy | Read Path | Write Path | Pros | Cons |
|---|---|---|---|---|
| **Cache-Aside (Lazy)** | Check cache $\to$ If miss, query DB and backfill cache | Write directly to DB $\to$ Evict/invalidate cache key | [+] Only caches data actually requested; cache failure does not stop writes | [-] Cache misses cost 3 network hops; risk of stale reads |
| **Read-Through** | App requests from cache; cache automatically queries DB on miss | (Same as write strategy) | [+] Clean application code; simplifies caching logic | [-] Cache library must support custom DB loaders |
| **Write-Through** | Read from cache | App writes to cache $\to$ Cache synchronously writes to DB | [+] Cache is always 100% in sync with DB | [-] Higher write latency (writes to two systems synchronously) |
| **Write-Back (Write-Behind)** | Read from cache | App writes to cache $\to$ Cache asynchronously batches writes to DB | [+] **Blazing fast writes**; absorbs massive write spikes | [-] **Risk of data loss** if cache node crashes before flush |
| **Write-Around** | Read from cache | App writes directly to DB, bypassing cache | [+] Prevents one-off batch writes from polluting cache | [-] Immediate reads after writes result in cache misses |

---

### 4. Cache Invalidation: The 3 Classic Production Gotchas

1. **Cache Stampede (Thundering Herd):** A popular key expires. 10,000 concurrent requests miss simultaneously and slam the primary database.
   * *Fix:* Use distributed locks (e.g. Redlock) so only 1 worker queries DB, or implement probabilistic early expiration (XFetch).
2. **Cache Penetration:** Malicious requests query non-existent keys (e.g. `user_id = -999`), bypassing cache and hitting DB every time.
   * *Fix:* Cache null values (`user:-999 = null`, TTL 60s) or put a **Bloom Filter** in front of the cache.
3. **Cache Breakdown:** An ultra-hot key expires.
   * *Fix:* Set background cron jobs to refresh the key asynchronously before its TTL expires.
