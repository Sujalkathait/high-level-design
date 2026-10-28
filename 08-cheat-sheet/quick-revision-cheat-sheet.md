# 01. Quick Revision Cheat Sheet & Top 20 Interview Questions

[🏠 Home](../README.md) > [08. Cheat Sheets](./README.md) > **Quick Revision Cheat Sheet**

---

## 1. Quick Revision Cheat Sheet

### Architecture

| Concept | Key Idea |
|---|---|
| **Monolithic Architecture** | Application functionality is packaged and deployed as a single unified unit. |
| **N-Tier Architecture** | Application responsibilities are separated into distinct logical layers (Presentation, Business, Data). |
| **Vertical Scaling** | Increasing the physical resources (CPU, RAM, NVMe disk) of an existing machine. |
| **Horizontal Scaling** | Adding more machines or application instances behind a load balancer. |
| **Latency** | The time taken to complete a single operation from start to finish. |
| **Throughput** | The total amount of work or requests completed per unit of time (QPS / TPS). |

### Networking & API Communication

| Technology | Key Idea |
|---|---|
| **REST** | Resource-oriented API style utilizing standard HTTP methods and conventions. |
| **RPC** | Remote Procedure Call; invoking a function on a remote machine as if it were local. |
| **gRPC** | High-performance RPC framework using HTTP/2, multiplexing, and Protocol Buffers. |
| **GraphQL** | Query language letting clients specify the exact schema fields they require. |
| **WebSockets** | Persistent, bidirectional, full-duplex TCP communication stream. |

### Load Balancing & Traffic Routing

| Concept | Key Idea |
|---|---|
| **Layer 4 Load Balancing** | Routes packets using transport-level data (IP addresses and TCP/UDP ports). |
| **Layer 7 Load Balancing** | Routes requests using application-level data (HTTP URLs, cookies, headers). |
| **Round Robin** | Distributes requests sequentially across a server pool in strict rotation. |
| **Least Connections** | Selects the server currently handling the fewest active connections. |
| **Consistent Hashing** | Minimizes key reassignments when server membership changes on a hash ring. |

### Caching & In-Memory Acceleration

| Concept | Key Idea |
|---|---|
| **Cache-Aside (Lazy)** | Application checks cache first and explicitly fetches from database on miss. |
| **Read-Through** | The cache provider layer automatically loads missing data on a miss. |
| **Write-Through** | Cache and backing store are updated synchronously as part of the write flow. |
| **Write-Back (Write-Behind)** | Writes hit cache first; updates to backing store occur asynchronously in batches. |
| **Write-Around** | Writes bypass the cache directly to persistent disk storage. |
| **LRU** | Evicts the item that has not been accessed for the longest time. |
| **LFU** | Evicts the item with the lowest total access frequency counter. |
| **Redis** | High-performance in-memory store supporting rich data structures, persistence, and clustering. |
| **Memcached** | Simple, natively multi-threaded in-memory key-value cache for raw objects. |

### Databases & Distributed Storage

| Concept | Key Idea |
|---|---|
| **ACID** | Transaction properties (Atomicity, Consistency, Isolation, Durability) for exact data correctness. |
| **BASE** | Distributed philosophy (Basically Available, Soft State, Eventual Consistency) prioritizing scale. |
| **CAP Theorem** | During a network partition, a distributed system must trade off consistency and availability. |
| **Database Replication** | Maintains identical copies of data across multiple database nodes to scale reads. |
| **Database Sharding** | Horizontally divides a dataset into partitions across multiple independent database nodes. |

---

## 2. Top 20 System Design Interview Questions & Answers

### Architecture
1. **What is the difference between monolithic and N-tier architecture?**
   * *Answer:* A monolith describes deployment packaging (all code compiled and shipped as one binary), while N-tier architecture describes separation of responsibilities into layers (UI, business logic, database). A single monolithic deployment can contain multiple tiers.
2. **What is the difference between vertical and horizontal scaling?**
   * *Answer:* Vertical scaling upgrades an existing machine with bigger hardware (more CPU/RAM). Horizontal scaling adds more machines or application instances across a network.
3. **Can one server handle thousands of requests?**
   * *Answer:* Yes. Capacity depends on workload type, I/O multiplexing model (epoll/kqueue), CPU, memory, network bandwidth, database query latency, and required response times.
4. **What is the difference between latency and throughput?**
   * *Answer:* Latency measures how long an operation takes from invocation to response. Throughput measures how much work is completed per unit of time (e.g., requests per second).

### Networking
5. **When should you use REST instead of gRPC?**
   * *Answer:* REST is ideal for public, browser-facing, resource-oriented HTTP APIs where broad third-party compatibility is required. gRPC is preferred for internal microservice communication and real-time streaming where binary serialization and speed are critical.
6. **What problem does GraphQL solve?**
   * *Answer:* GraphQL lets clients request only the specific fields they need, eliminating over-fetching and under-fetching while consolidating multiple REST network round-trips into a single query.
7. **Why use WebSockets instead of polling?**
   * *Answer:* WebSockets establish a persistent, bidirectional TCP connection that allows servers to push messages to clients instantly without the repeated HTTP handshake and header overhead of short or long polling.

### Load Balancing
8. **What is the difference between Layer 4 and Layer 7 load balancing?**
   * *Answer:* Layer 4 operates at the transport layer (TCP/UDP), routing packets without inspecting message payloads. Layer 7 operates at the application layer, parsing HTTP headers, cookies, and URLs to make intelligent routing decisions.
9. **When should you use Round Robin?**
   * *Answer:* Round Robin works best when backend servers have identical hardware specifications and incoming requests have relatively uniform computational costs.
10. **When is Least Connections useful?**
    * *Answer:* Least Connections is useful when request durations vary significantly (e.g., file downloads or long-lived database transactions), provided connection counts accurately reflect server workload.
11. **Why use consistent hashing?**
    * *Answer:* Consistent hashing maps keys and servers to a virtual ring ($0 \text{ to } 2^{32}-1$), ensuring that when a server is added or removed, only $K/N$ keys need to be relocated, preventing catastrophic cache stampedes and minimizing data migration.

### Caching
12. **What is the difference between cache-aside and read-through?**
    * *Answer:* In cache-aside, the application code explicitly checks the cache and queries the database on a miss. In read-through, the cache layer or provider transparently handles cache misses and loads data from the database.
13. **What is the difference between write-through and write-back?**
    * *Answer:* Write-through updates the cache and backing store synchronously as part of the write flow. Write-back acknowledges the write immediately in cache RAM and updates the backing store asynchronously in batches.
14. **What is a cache stampede?**
    * *Answer:* A cache stampede occurs when an ultra-popular cache entry expires, causing thousands of concurrent requests to miss simultaneously and query the primary database all at once.
15. **What is the difference between LRU and LFU?**
    * *Answer:* LRU (Least Recently Used) evicts the item that has not been accessed for the longest period of time. LFU (Least Frequently Used) evicts the item with the lowest total access frequency counter.
16. **When should you choose Redis over Memcached?**
    * *Answer:* Choose Redis when you need complex data structures (Hashes, Sorted Sets, Lists), persistence to disk, replication, Sentinel failover, or pub/sub. Choose Memcached for straightforward, high-throughput string/object caching on large multi-core machines.

### Databases
17. **What is the difference between replication and sharding?**
    * *Answer:* Replication creates duplicate copies of the same dataset across multiple nodes to scale reads and provide high availability. Sharding distributes different subsets of a dataset across multiple nodes to scale write throughput and total disk capacity.
18. **What is the CAP theorem?**
    * *Answer:* During an unavoidable network partition, a distributed data system cannot guarantee both linearizable consistency and availability for every request; it must trade off between returning errors (CP) or returning stale data (AP).
19. **What is the difference between ACID and BASE?**
    * *Answer:* ACID describes strict transactional correctness properties (Atomicity, Consistency, Isolation, Durability) for relational databases. BASE describes a distributed design philosophy (Basically Available, Soft State, Eventual Consistency) prioritizing availability and scale.
20. **When should a database be sharded?**
    * *Answer:* A database should be sharded only when vertical scaling, query optimization, indexing, caching, and read replicas have been fully exhausted and write volume or storage capacity exceeds single-server limits.

---

## 3. Engineering Conclusion

High-Level Design is about **understanding trade-offs rather than choosing technologies in isolation**.

A well-designed system evolves according to measurable requirements:
1. **Start with a simple architecture.**
2. **Measure latency, throughput, error rates, and resource utilization.**
3. **Optimize inefficient application code and database queries first.**
4. **Introduce caching when repeated reads justify it.**
5. **Add load balancing and application instances when capacity or redundancy is needed.**
6. **Scale the database according to its actual bottlenecks (read replicas first, sharding last).**
7. **Introduce distributed components only when their operational benefits justify their complexity.**

> [!TIP]
> **The Golden Rule:** The objective is not to build the most complicated architecture. It is to build the simplest architecture that reliably satisfies the system's functional, performance, reliability, and cost requirements.

---

| ⬅️ Previous Module | 🏠 Index |
|:--- |:---:|
| [07. System Design Examples](../07-system-design-examples/README.md) | [System Design Hub](../README.md) |
