# Database Sharding

> High-Level System Design | Databases & Storage

---

### 1. What is it?

Sharding is horizontally partitioning a large database table into smaller chunks (called **shards**) and distributing them across **multiple independent physical database servers**.
* **Replication vs. Sharding:**
  * **Replication:** Copies the *entire* database to multiple machines (scales **reads**).
  * **Sharding:** Splits the database so each machine holds only a *fraction* of the rows (scales **writes and storage**).
```mermaid
flowchart TD
    App[Application Servers] --> Router{ Shard Router / Proxy\nhash(user_id) % 3}
    Router -->|user_id hash: 0| Shard1[(Shard 1: Users 0 - 3M\nIndependent Disk & CPU)]
    Router -->|user_id hash: 1| Shard2[(Shard 2: Users 3M - 6M\nIndependent Disk & CPU)]
    Router -->|user_id hash: 2| Shard3[(Shard 3: Users 6M - 9M\nIndependent Disk & CPU)]
```

---

### 2. Sharding Strategies Compared

```mermaid
flowchart LR
    subgraph RangeBased [1. Range-Based Sharding]
        direction TB
        R1[A - H: Shard 1]
        R2[I - P: Shard 2]
        R3[Q - Z: Shard 3]
        NoteR["Simple, but causes severe hotspots! (Letters like S get 10x traffic)"]
    end
    subgraph HashBased [2. Hash-Based Sharding (Standard)]
        direction TB
        H1["hash(user_id) % N"]
        H2["Even distribution across all nodes"]
        NoteH["Uniform spread, but resharding when adding nodes is painful"]
    end
    subgraph ConsistentHash [3. Consistent Hashing]
        direction TB
        C1["Virtual Ring (0 to 2^32-1)"]
        C2["Adding Shard 4 only moves 1/4 of keys!"]
    end
```
| Strategy | How it Works | Pros | Cons |
|---|---|---|---|
| **Range-Based** | Shard by ID ranges (e.g. 1–1M $\to$ Shard 1; 1M–2M $\to$ Shard 2) or timestamps | Easy range queries (`WHERE created_at BETWEEN...`) | **Severe hotspots** (All current writes slam the latest date shard) |
| **Hash-Based** | `hash(key) % Number_Of_Shards` | Perfectly even distribution across all servers | Adding new shard servers requires migrating almost all data |
| **Directory-Based** | Lookup service maintains a map of which entity lives on which shard | Highly flexible; can move individual users easily | The lookup table becomes a single point of failure and bottleneck |

---

### 3. Detailed Pros & Cons of Sharding

#### Advantages (The Pros)

* [+] **Unlimited Horizontal Write Scaling:** If 1 database maxes out at 10,000 writes/sec, 10 shards can handle **100,000 writes/sec**.
* [+] **Unlimited Disk Storage:** You can store Petabytes of data by simply adding more commodity servers.
* [+] **Smaller B-Tree Indexes:** A table with 1 billion rows has a massive index that spills out of RAM into slow disk. Ten shards of 100M rows each easily fit their indexes in high-speed RAM.
* [+] **Blast Radius Isolation:** If Shard 2 experiences a hardware failure, **only 33% of your users** are impacted. Users on Shards 1 and 3 experience zero downtime.

#### Disadvantages (The Cons)

* [-] **Cross-Shard Joins are Impossible / Slow:** You can no longer run `SELECT * FROM orders JOIN users ON...` if the user is on Shard 1 and the order is on Shard 3. You must execute joins manually in application code.
* [-] **Distributed Transactions are Painful:** If a transaction modifies data on two shards, you must use **Two-Phase Commit (2PC)** or a Saga pattern, introducing high network latency and locking risks.
* [-] **Re-sharding Complexity:** When your traffic outgrows 4 shards and you need 8 shards, redistributing existing data across machines without downtime is one of the hardest operations in software engineering.
* [-] **The Hotspot (Celebrity) Problem:** If a celebrity (e.g. Elon Musk or Cristiano Ronaldo) is assigned to Shard 2, millions of likes and replies will overwhelm Shard 2 while other shards sit idle.

---

### 4. How to Choose a Great Shard Key

The **Shard Key** is the single most important decision in your database architecture.
```mermaid
graph TD
    Rule{Is the Candidate Shard Key Good?}
    Rule -->|High Cardinality e.g. user_id, order_id| Good[[+] Great Key: Evenly distributes millions of distinct values]
    Rule -->|Low Cardinality e.g. country, gender| Bad[[-] Terrible Key: Only 2 or 100 values; creates massive unbalanced shards]
    Rule -->|Monotonically Increasing e.g. auto_increment id, timestamp| Hotspot[[-] Terrible Key: All new writes hit the highest shard]
```
* **High Cardinality:** Millions of unique possible values (`user_id`, `UUID`).
* **Query Alignment:** The shard key should be present in the `WHERE` clause of 90%+ of your queries so the router sends the request to **exactly one shard** (Single-Shard Query) rather than querying all shards (Scatter-Gather Query).

---

### 5. Real-World Case Study: Instagram

* **The Problem:** Instagram needed to shard their PostgreSQL databases across dozens of servers, but needed 64-bit unique IDs that were chronologically sortable without coordination.
* **The Solution:** Custom 64-bit ID generation:
  * 41 bits = Timestamp in milliseconds (gives 69 years of IDs).
  * 13 bits = **Logical Shard ID** (supports up to 8,192 shards).
  * 10 bits = Auto-incrementing sequence number (1,024 IDs per shard per millisecond).
* **Result:** Every ID inherently tells the router which shard to query without looking up a central index!

---

### 6. The Production Golden Rule

> Note: **Do NOT shard until you have exhausted:**
> 1. Hardware vertical scaling (large NVMe SSDs, 512GB RAM).
> 2. Database read replicas (to offload 90% read traffic).
> 3. Caching hot queries in Redis.
>
> Sharding adds 10x operational complexity. Treat it as your **last resort** when write volume or storage limits demand it.
