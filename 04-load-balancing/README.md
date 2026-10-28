# 04. Load Balancing and Traffic Routing

[🏠 Home](../README.md) > **04. Load Balancing**

---

## Overview

Load balancing distributes client traffic across a pool of application servers, ensuring no single server is overwhelmed. It is the primary enabler of horizontal scalability, zero-downtime rolling deployments, and fault tolerance against backend crashes.

```mermaid
flowchart LR
    A[01. What is an LB?] --> B[02. Layer 4 vs Layer 7]
    B --> C[03. Round Robin]
    C --> D[04. Least Connections]
    D --> E[05. Consistent Hashing]
```

---

## Module Table of Contents

| # | Topic | Key Focus | Link |
|---|---|---|---|
| **01** | **What Is a Load Balancer?** | Traffic distribution, liveness vs. readiness probes, eliminating SPOF with VRRP/Anycast | [Read Guide ➡️](./what-is-a-load-balancer.md) |
| **02** | **Layer 4 vs. Layer 7 Load Balancing** | Transport (TCP/UDP) vs. Application (HTTP/TLS) routing, two-tier enterprise architecture | [Read Guide ➡️](./layer-4-vs-layer-7.md) |
| **03** | **Round Robin Scheduling** | Sequential rotation, weighted balancing for mixed hardware fleets, trade-offs | [Read Guide ➡️](./round-robin.md) |
| **04** | **Least Connections** | Active connection tracking, handling long-running requests, Least Requests in HTTP/2 | [Read Guide ➡️](./least-connections.md) |
| **05** | **Consistent Hashing** | Hash ring ($0$ to $2^{32}-1$), virtual nodes (vNodes), avoiding cache stampedes | [Read Guide ➡️](./consistent-hashing.md) |

---

| ⬅️ Previous Module | 🏠 Index | Next Module ➡️ |
|:--- |:---:| ---:|
| [03. Networking](../03-networking/README.md) | [System Design Hub](../README.md) | [05. Caching & Acceleration ➡️](../05-caching/README.md) |
