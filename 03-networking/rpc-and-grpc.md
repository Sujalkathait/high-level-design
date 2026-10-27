# RPC and gRPC

> High-Level System Design | Networking & Communication Protocols

---

### 1. What is it?
**RPC (Remote Procedure Call)** is a communication pattern where a client executes a function on a remote server as if it were a local function call in memory. **gRPC** is Google's open-source, high-performance RPC framework built on top of **HTTP/2 transport** and **Protocol Buffers (Protobuf)** binary serialization.

---

### 2. Why do we need it?
* **Problem without it:** In a backend with 50 microservices, using REST/JSON creates massive serialization overhead, large text payloads over the network, and lack of strict type safety between services.
* **Real-world benefit:** gRPC is **7 to 10 times faster than REST**, uses strictly typed binary `.proto` contracts, and supports bi-directional streaming natively.

---

### 3. Architecture & Data Flow

```mermaid
flowchart LR
    subgraph ClientService [Order Service (Client)]
        Stub[gRPC Client Stub]
    end

    subgraph ServerService [Payment Service (Server)]
        Impl[gRPC Service Impl]
    end

    Stub -->|HTTP/2 Binary Stream: Protobuf Payload| Impl
    Impl -->>|HTTP/2 Binary Stream: Protobuf Response| Stub
```

---

### 4. Protocol Buffers (Protobuf) vs. JSON

```protobuf
// Definition in order.proto
syntax = "proto3";

message OrderRequest {
    int64 order_id = 1;
    string user_id = 2;
    double amount = 3;
}
```

* **Binary Serialization:** Fields are encoded into compact binary tag-value pairs instead of ASCII JSON keys (`{"order_id": 123456}`).
* **Type Safety:** The `.proto` file acts as the single source of truth across Go, Java, Python, and C++ services.

---

### 5. Practical Trade-offs

| Dimension | gRPC | Traditional REST / JSON |
|---|---|---|
| **Speed & Latency** | **[+] 7-10x Faster:** Minimal CPU serialization cost | Slower text parsing and string allocations |
| **Payload Size** | **[+] Compact:** Binary format cuts bandwidth by 60%+ | Larger JSON text payloads |
| **Multiplexing** | **[+] Native HTTP/2:** Multiple requests over single TCP socket | HTTP/1.1 requires connection pooling |
| **Streaming** | **[+] Native:** Client streaming, Server streaming, Bi-directional | Requires WebSockets or SSE |
| **Browser Compatibility** | **[-] Poor:** Requires gRPC-Web proxy for browsers | **[+] Native:** Works in every browser directly |
| **Human Readability** | **[-] Binary:** Cannot read payload in Wireshark or cURL | **[+] Human-readable JSON** |

---

### 6. Real-World Case Study
* **Netflix & Google:** Use gRPC for all internal microservice-to-microservice traffic across their datacenters to shave milliseconds of latency off internal fan-out calls, while exposing standard REST/GraphQL to external mobile and web clients.
