# 01. Introduction to High-Level Design

[🏠 Home](../README.md) > [01. Fundamentals](./README.md) > **Introduction to High-Level Design**

---

## 1. What is High-Level Design (HLD)?

**High-Level Design (HLD)** describes the macro-level architecture of a software system. It explains how major components communicate, where data is persisted, how requests are processed, and how the infrastructure scales as traffic increases.

HLD focuses on the **30,000-foot view** rather than the low-level implementation details of individual classes, data structures, or algorithms.

```mermaid
flowchart TD
    Client(["Client Device: Web / Mobile"]) -->|1. DNS Lookup| DNS[DNS Resolver]
    Client -->|2. HTTPS Traffic| LB[Load Balancer]
    LB -->|3. Route Request| App1[App Server 1]
    LB -->|Route Request| App2[App Server 2]
    App1 <-->|4. Fast Cache Hit| Cache[("In-Memory Cache: Redis")]
    App1 -->|5. Miss / Writes| DB[("Primary Database")]
    App1 -.->|6. Async Events| MQ["Message Queue: Kafka"]
    MQ --> Workers[Background Workers]
```

---

## 2. Key Components of System Design

A modern distributed application is composed of well-defined building blocks. Each component fulfills a specific operational responsibility:

| Component | Primary Responsibility | Common Technologies |
|---|---|---|
| **Client** | Sends user requests and renders UI | Web Browser, iOS/Android App, IoT, CLI |
| **DNS** | Resolves domain names (`api.app.com`) to IP addresses | AWS Route 53, Cloudflare DNS, CoreDNS |
| **CDN** | Caches static assets & media close to users geographically | Cloudflare, CloudFront, Fastly, Akamai |
| **Load Balancer** | Distributes incoming traffic across backend servers | Nginx, HAProxy, AWS ALB/NLB, Envoy |
| **Application Server** | Executes core business logic and API orchestration | Node.js, Go, Java (Spring), Python (FastAPI) |
| **Cache** | Stores frequently read data in RAM for sub-millisecond access | Redis, Memcached, Dragonfly |
| **Database** | Provides persistent, durable, and queryable data storage | PostgreSQL, MySQL, DynamoDB, MongoDB |
| **Message Queue** | Enables asynchronous decoupling and background task processing | Apache Kafka, RabbitMQ, AWS SQS |
| **Monitoring & Logging** | Tracks latency, errors, traffic metrics, and system health | Prometheus, Grafana, Datadog, ELK Stack |

---

## 3. Typical Request Flow

```mermaid
sequenceDiagram
    autonumber
    actor User as User / Client
    participant DNS as DNS Resolver
    participant LB as Load Balancer
    participant App as Application Server
    participant Cache as Redis Cache
    participant DB as Primary Database

    User->>DNS: Resolve domain (api.example.com)
    DNS-->>User: Return Server IP (192.0.2.1)
    User->>LB: HTTPS Request (GET /products/101)
    LB->>App: Forward request (Least Connections)
    App->>Cache: Check for cached product (key: product:101)
    alt Cache Hit
        Cache-->>App: Return cached JSON (1ms)
    else Cache Miss
        App->>DB: Query: SELECT * FROM products WHERE id=101
        DB-->>App: Return row data (15ms)
        App->>Cache: Asynchronously populate cache (TTL: 10m)
    end
    App-->>LB: 200 OK + Payload
    LB-->>User: Rendered Response
```

> [!NOTE]
> The cache is not queried for every single request. Static assets bypass application servers through CDNs, while write-heavy operations (e.g., checkout transactions) go directly to the primary database with proper ACID transaction guarantees.

---

## 4. The 7 Goals of Good System Design

When architecting systems for scale and reliability, engineering decisions revolve around trade-offs among seven core pillars:

1. **Scalability:** The ability to handle increasing traffic volume, data storage, and concurrency without performance degradation.
2. **Availability:** Ensuring the system remains operational and accessible even when individual hardware or software components fail (e.g., 99.99% "four nines" uptime).
3. **Reliability:** Guaranteeing that the system consistently produces correct, predictable, and fault-tolerant results under changing loads.
4. **Performance:** Minimizing response latency (p50, p95, p99) while maximizing useful throughput (Requests Per Second).
5. **Maintainability:** Structuring services and codebases so that engineering teams can fix bugs, deploy features, and monitor operations without unnecessary friction.
6. **Security:** Protecting user identities, data privacy, and infrastructure against unauthorized access, DDoS, and injection vulnerabilities.
7. **Cost Efficiency:** Maximizing resource utilization without over-provisioning expensive cloud hardware prematurely.

> [!TIP]
> **The Golden Rule of System Design (YAGNI):** "You Aren't Gonna Need It." Never design for 100 million users on Day 1. Start with the simplest working architecture, measure bottlenecks using telemetry, and scale incrementally as genuine demand requires.

---

## 5. Related Topics & Next Steps

* [Client-Server Architecture](./client-server-architecture.md) — How client and server responsibilities are divided.
* [The Request-Response Lifecycle](./request-lifecycle.md) — Detailed millisecond-by-millisecond network breakdown.
* [How One Server Handles Requests](./how-one-server-handles-requests.md) — Concurrency models, CPU vs I/O limits.

---

| ⬅️ Previous Topic | 🏠 Index | Next Topic ➡️ |
|:--- |:---:| ---:|
| [Master Table of Contents](../README.md) | [01. Fundamentals Index](./README.md) | [Client-Server Architecture ➡️](./client-server-architecture.md) |
