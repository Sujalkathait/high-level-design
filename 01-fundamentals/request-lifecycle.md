# 03. The Request-Response Lifecycle

[🏠 Home](../README.md) > [01. Fundamentals](./README.md) > **The Request-Response Lifecycle**

---

### 1. What is it?

The complete millisecond-by-millisecond journey of data from the exact moment a user taps a button on their phone until pixels render on screen.
* **Why it matters:** When an API response takes 800ms, you cannot fix it until you know where that time was spent:
  $$\text{Total Latency} = T_{\text{DNS}} + T_{\text{TCP}} + T_{\text{TLS}} + T_{\text{Network RTT}} + T_{\text{Server CPU}} + T_{\text{Database Query}}$$

---

### 2. End-to-End Sequence Diagram

```mermaid
sequenceDiagram
    autonumber
    actor Client as  Client App
    participant DNS as  DNS Resolver
    participant CDN as  Edge CDN (Cloudflare)
    participant LB as  Load Balancer (Nginx)
    participant App as  Backend App Server
    participant Cache as  Redis Cache
    participant DB as  PostgreSQL DB
    Client->>DNS: 1. Lookup 'api.myapp.com'
    DNS-->>Client: Returns IP: 104.16.12.34 (Edge Anycast IP)
    Note over Client,CDN: 2. Network Handshake (TCP 3-Way + TLS 1.3)
    Client->>CDN: TCP SYN
    CDN-->>Client: TCP SYN-ACK
    Client->>CDN: TCP ACK + TLS ClientHello
    CDN-->>Client: TLS ServerHello + Encrypted Session Keys
    Note over Client,CDN: 3. Encrypted HTTP Request
    Client->>CDN: GET /v1/users/42 (Headers, Bearer Token)
    alt Static Asset or Edge Cache Hit
        CDN-->>Client: 200 OK (Served in 5ms from Edge)
    else Dynamic API Request
        CDN->>LB: Forward request over persistent backhaul connection
        LB->>App: Route via Least-Connections algorithm
        App->>Cache: GET user:42
        alt Cache Hit (RAM)
            Cache-->>App: { id: 42, name: "Alice" } (1ms)
        else Cache Miss
            App->>DB: SELECT * FROM users WHERE id = 42; (10ms)
            DB-->>App: Row data
            App->>Cache: SETEX user:42 300 (Populate cache for 5m)
        end
        App-->>LB: 200 OK + JSON payload
        LB-->>CDN: Forward response
        CDN-->>Client: Gzip/Brotli compressed JSON response
    end
```

---

### 3. Step-by-Step Breakdown

#### Step 1: DNS Lookup (Domain $\to$ IP Address)

* The client checks in order:
  1. **Browser Cache:** (Kept for 60 seconds).
  2. **OS Cache:** (`hosts` file & local DNS resolver).
  3. **Router / ISP DNS Cache:** (Recursive resolver).
  4. **Authoritative Name Server:** Returns the public IP address.
* *Optimization:* Use long TTLs (e.g. 1 hour) and Anycast DNS (Cloudflare 1.1.1.1 / Route 53) to keep lookups under 20ms.

#### Step 2: TCP Handshake + TLS Encryption

* **TCP 3-Way Handshake:** `SYN` $\to$ `SYN-ACK` $\to$ `ACK` establishes a reliable connection.
* **TLS 1.3 Handshake:** Negotiates encryption keys in **just 1 Round-Trip Time (RTT)** (older TLS 1.2 took 2 RTTs).
* *Optimization:* **Connection Keep-Alive & HTTP/2 Multiplexing** reuse the established connection for hundreds of requests instead of repeating handshakes.

#### Step 3: Edge Routing & CDN

* Requests hit the nearest geographical Point of Presence (PoP) via **Anycast routing**.
* If the user in Tokyo requests static images, Tokyo's CDN serves them directly instead of traveling 10,000 km to a US datacenter.

#### Step 4: Reverse Proxy & Load Balancer (SSL Termination)

* Decrypts HTTPS (SSL Termination) so backend servers don't waste CPU cycles decrypting packets.
* Inspects HTTP headers, validates JWT tokens, strips malicious headers, and forwards to a healthy backend instance.

#### Step 5: Application Worker & Connection Pooling

* A worker thread or asynchronous event loop picks up the request.
* **Database Connection Pool:** Instead of opening a new TCP connection to the database (50ms cost), the server borrows an existing connection from a pool (e.g., HikariCP / PgBouncer), executes the query in 5ms, and returns the connection.

#### Step 6: Response & Client Render

* The server compresses the JSON/HTML with **Brotli or Gzip** (reducing payload size by up to 70%).
* Client receives the byte stream, decompresses it, parses JSON, and renders UI state.

---

### 4. Where Does Latency Hide? (Latency Budget Table)

| Component | Unoptimized Time | Optimized Time | How It Was Optimized |
|---|---|---|---|
| **DNS Resolution** | 120 ms | 5 ms | Anycast DNS + Local caching |
| **TCP + TLS Handshake** | 180 ms | 0 ms | HTTP/2 Connection reuse & Keep-Alive |
| **Network Travel (RTT)** | 150 ms | 15 ms | CDN Edge caching close to user |
| **Server Processing** | 80 ms | 10 ms | Asynchronous non-blocking I/O |
| **Database Query** | 350 ms | 2 ms | B-Tree Indexes + Redis In-Memory Cache |
| **Total Response Time** | **880 ms (Laggy)** | **32 ms (Instant)** | **~27x speedup!** |

---

### 5. Practical Rules of Thumb

1. **Reuse connections:** A new HTTPS connection over intercontinental fiber costs 200ms before a single byte of data is sent. Always enable HTTP Keep-Alive.
2. **Terminate TLS at the Load Balancer:** Offload encryption work at the edge so your internal microservices communicate over fast private networks.
3. **Never query without an index:** An unindexed query does a full table scan ($O(N)$ Disk I/O); an indexed query does a B-Tree lookup ($O(\log N)$ RAM/SSD read).

---

## Related Topics & Next Steps

* [Client-Server Architecture](./client-server-architecture.md) — The roles of frontend and backend.
* [How One Server Handles Requests](./how-one-server-handles-requests.md) — OS scheduling, worker threads, and capacity.
* [Latency vs. Throughput](./latency-throughput.md) — Tail latencies (p99) and Little's Law.
* [Layer 4 vs. Layer 7 Load Balancing](../04-load-balancing/layer-4-vs-layer-7.md) — How reverse proxies route requests.

---

| ⬅️ Previous Topic | 🏠 Index | Next Topic ➡️ |
|:--- |:---:| ---:|
| [Client-Server Architecture](./client-server-architecture.md) | [01. Fundamentals Index](./README.md) | [How One Server Handles Requests ➡️](./how-one-server-handles-requests.md) |
