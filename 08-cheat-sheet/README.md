# 08. Engineering Cheat Sheets & Revision

[🏠 Home](../README.md) > **08. Cheat Sheets**

---

## Overview

A consolidated reference section containing high-yield revision summaries, back-of-the-envelope power-of-two approximations, latency numbers every engineer should know, and the top 20 foundational system design interview questions with model answers.

---

## 🎯 Key Learning Objectives

* **High-Yield Rapid Revision**: Review condensed summary matrices across Architecture, Networking, Load Balancing, Caching, and Databases in under 5 minutes.
* **Latency Numbers Every Systems Engineer Should Know**: Memorize orders of magnitude between CPU L1/L2 cache, RAM, NVMe SSD, HDD, and cross-datacenter round trips.
* **Powers of Two & Capacity Math**: Internalize approximations ($2^{10} \approx 1\text{KB}$, $2^{20} \approx 1\text{MB}$, $2^{30} \approx 1\text{GB}$, $2^{40} \approx 1\text{TB}$) to rapidly calculate storage and QPS on whiteboards.
* **Availability SLAs**: Quantify downtime budgets for "Three Nines" (99.9% $\approx$ 8.7 hrs/yr) vs. "Four Nines" (99.99% $\approx$ 52 mins/yr) vs. "Five Nines" (99.999% $\approx$ 5 mins/yr).
* **Top 20 Interview Q&A**: Master clear, articulate responses to the most frequently asked high-level design interview questions.

---

## 📚 Module Table of Contents

| # | Topic | Key Focus | Link |
|---|---|---|---|
| **01** | **Quick Revision Cheat Sheet & Top 20 Questions** | Summary tables across Architecture, Networking, Load Balancing, Caching, Databases + Top 20 Q&A | [Read Guide ➡️](./quick-revision-cheat-sheet.md) |

---

## 🔍 Detailed Topic Breakdown

* 📌 **[01. Quick Revision Cheat Sheet & Top 20 Questions](./quick-revision-cheat-sheet.md)**
  * **Domain Reference Cards**:
    * *Architecture*: Monolithic, N-Tier, Microservices, Vertical vs. Horizontal Scaling.
    * *Networking*: REST, RPC, gRPC (Protobuf + HTTP/2), GraphQL, WebSockets.
    * *Load Balancing*: Layer 4 vs. Layer 7, Round Robin, Least Connections, Consistent Hashing.
    * *Caching*: Cache-Aside, Write-Through/Back/Around, LRU/LFU, Redis vs. Memcached, Cache stampede & penetration fixes.
    * *Databases*: ACID vs. BASE, CAP Theorem, Primary-Replica replication, Horizontal sharding.
  * **Numbers Every Engineer Must Know**:
    * L1 CPU Cache: $\approx 0.5–1\text{ ns}$.
    * Main Memory (RAM): $\approx 100\text{ ns}$.
    * NVMe SSD Random Read: $\approx 10–50\text{ }\mu\text{s}$.
    * HDD Seek: $\approx 1–10\text{ ms}$ ($100,000\times$ slower than RAM!).
    * Same Datacenter Round Trip: $\approx 0.5\text{ ms}$.
    * US East to US West Round Trip: $\approx 60–80\text{ ms}$.
    * Transatlantic Round Trip (NYC $\leftrightarrow$ London): $\approx 150\text{ ms}$.
  * **Capacity Estimation Shortcuts**:
    * $1\text{ Day} = 86,400\text{ seconds} \approx 10^5\text{ seconds}$.
    * $1\text{ Million requests/day} \approx 12\text{ RPS}$ (Average), $\approx 24–36\text{ RPS}$ (Peak $2\times$–$3\times$).
    * $1\text{ Billion requests/day} \approx 12,000\text{ RPS}$.
  * **Top 20 Interview Questions**: Model answers covering SQL vs. NoSQL, CAP theorem trade-offs, CDN caching, Redis cluster failover, WebSockets vs. polling, database connection pooling, and circuit breaker patterns.

---

## 💡 Key Architectural Rules of Thumb

* **Estimate Before Designing**: Always perform rough back-of-the-envelope math (QPS, bandwidth, 5-year storage) in the first 5 minutes of any system design conversation.
* **Keep the 86,400 Constant Handy**: Use $10^5$ seconds/day to instantly convert daily active volume into operations per second.
* **Design for 99th Percentile SLA**: High availability SLAs are made or broken by the slowest 1% of queries; optimize tail latency rather than average latency.
* **Never Over-Engineer**: The best answer in a design interview is the simplest architecture that solves the stated business constraints with clear trade-off awareness.

---

| ⬅️ Previous Module | 🏠 Index |
|:--- |:---:|
| [07. System Design Examples](../07-system-design-examples/README.md) | [System Design Hub](../README.md) |
