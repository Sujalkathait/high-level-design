# ACID vs. BASE in Distributed Databases

> High-Level System Design | Databases & Storage

---

### 1. What is it?

* **ACID (Pessimistic & Strict):** The traditional relational database model (PostgreSQL, MySQL, Oracle) designed for **absolute data accuracy**. Every transaction is guaranteed to either succeed completely or fail completely, with zero half-written data.
* **BASE (Optimistic & Flexible):** The distributed NoSQL model (Cassandra, DynamoDB, Couchbase) designed for **massive scale and high availability**. It accepts that data might be temporarily inconsistent across nodes in exchange for never rejecting a write.
```mermaid
flowchart LR
    subgraph ACID_Model [ACID: Strong Consistency]
        direction TB
        Tx1[Transaction] --> Lock[Acquire Row Locks]
        Lock --> Write[Write to WAL & Disk]
        Write --> Commit[Commit Transaction]
        Commit --> Unlock[Release Locks]
        Note1["Guarantees exact balance, but limits scale!"]
    end
    subgraph BASE_Model [BASE: Eventual Consistency]
        direction TB
        W1[Write Request] --> NodeA[Node A: Saves locally]
        NodeA -->> Client[Instant 200 OK Response]
        NodeA -.->|Async Gossip / Background Sync| NodeB[Node B]
        NodeA -.->|Async Gossip / Background Sync| NodeC[Node C]
        Note2["Never rejects writes, but data syncs over time!"]
    end
```

---

### 2. Deep Dive: ACID Explained

| Principle | Meaning | Real-World Scenario |
|---|---|---|
| **A — Atomicity** | "All or Nothing." If any step fails, the entire transaction rolls back. | Transferring $100 from Alice to Bob: Debit Alice succeeds, credit Bob fails $\to$ Entire transaction aborts; Alice keeps her $100. |
| **C — Consistency** | Enforces all schema constraints, foreign keys, and unique checks. | An account balance column with a `CHECK (balance >= 0)` constraint will immediately abort any transaction that causes overdraft. |
| **I — Isolation** | Concurrent transactions execute without interfering with each other. | If Alice and Bob both attempt to purchase the last concert ticket simultaneously, one transaction wins and the other sees a sold-out error. |
| **D — Durability** | Once committed, data survives crashes, power loss, and reboots. | Written immediately to non-volatile disk via the **Write-Ahead Log (WAL)** before returning success to the client. |

---

### 3. Deep Dive: BASE Explained

| Principle | Meaning | Real-World Scenario |
|---|---|---|
| **BA — Basically Available** | The system guarantees availability for every request, even during node failures. | Even if 2 out of 5 database nodes are down, the remaining nodes accept user requests without error. |
| **S — Soft State** | Data can change over time without user interaction due to background sync. | Replicas asynchronously catch up via gossip protocols; a node's state is "soft" until fully synchronized. |
| **E — Eventual Consistency** | Given enough time with no new updates, all nodes will converge to the exact same value. | After posting an Instagram comment, you see it instantly; your friend across the ocean sees it 500ms later. |

---

### 4. Comprehensive Pros & Cons

#### ACID Pros & Cons

* **Pros:**
  * [+] **Zero Lost Money / Transactions:** Guarantees absolute correctness. Critical for billing, inventory, and banking.
  * [+] **Simple Developer Mental Model:** You don't have to write complex application code to resolve data conflicts.
  * [+] **Rich Query Capabilities:** Supports complex multi-table SQL `JOIN`s, aggregations, and subqueries out of the box.
* **Cons:**
  * [-] **Poor Horizontal Write Scalability:** Scale-up limits write throughput. You cannot easily spread ACID writes across 50 servers.
  * [-] **High Latency Under Contention:** Row locks and table locks cause worker threads to queue up and wait.
  * [-] **Single Point of Failure (Write Master):** A single primary node must serialize all writes; if it fails, failover takes time.

#### BASE Pros & Cons

* **Pros:**
  * [+] **Massive Write Throughput:** Can ingest millions of writes per second across hundreds of commodity servers.
  * [+] **High Availability (99.999%):** Multi-leader or leaderless clusters never go read-only when a node dies.
  * [+] **Multi-Region Active-Active:** Users in Tokyo write to Tokyo nodes; users in London write to London nodes with local latency.
* **Cons:**
  * [-] **Stale / Dirty Reads:** A user might refresh their profile and see old data for a few hundred milliseconds.
  * [-] **Application Conflict Resolution:** You must handle concurrent writes (e.g., using Last-Write-Wins or CRDTs).
  * [-] **No Foreign Keys or Joins:** Data must be denormalized; data consistency must be enforced in application code.

---

### 5. Head-to-Head Decision Matrix

| Dimension | ACID (e.g., PostgreSQL, MySQL) | BASE (e.g., Cassandra, DynamoDB) |
|---|---|---|
| **Data Consistency** | **Strong Consistency** (Immediate) | **Eventual Consistency** (Lagged) |
| **Scalability** | Vertical (Scale Up) + Read Replicas | **Horizontal (Scale Out)** across hundreds of nodes |
| **Transaction Scope** | Multi-row, multi-table transactions | Single-row / single-key atomic operations |
| **Performance** | Slower for high concurrent writes | **Blazing fast writes** (appends to memory/commit log) |
| **Ideal Workloads** | E-commerce checkout, Ledger, Orders, User Auth | Social media feeds, IoT telemetry, Chat history, Clickstream |

---

### 6. Real-World Case Studies

* **Amazon Shopping Cart (BASE):** Amazon engineered DynamoDB so that `Add to Cart` never fails. If a network partition occurs, the system accepts both items and reconciles/merges them later rather than rejecting an eager buyer.
* **Stripe Payment Engine (ACID):** Stripe uses ACID transactions with strict Serializable isolation in PostgreSQL to ensure money is never credited or debited twice under high network concurrency.
