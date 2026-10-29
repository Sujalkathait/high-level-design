# 🚀 Load Balancing in C++ — Complete Beginner-Friendly Explanation

[🏠 Home](../README.md) > [04. Load Balancing](./README.md) > **Load Balancing in C++**

---

**Problem:** Design and implement high-efficiency Load Balancers in C++ using core scheduling algorithms (Round Robin, Weighted Round Robin, Least Connections, and Consistent Hashing).

**Difficulty:** Medium  
**Core Approaches:** Modulo Arithmetic, Active State Tracking, and Distributed Hash Rings (`std::map` with Virtual Nodes)  
**Time Complexities:** $O(1)$ for Round Robin & Least Connections; $O(\log V)$ for Consistent Hashing  
**Space Complexity:** $O(N)$ for $N$ servers, $O(N \times V)$ with $V$ virtual nodes  
**Executable Code Directory:** [`code/`](./code/)

---

## 📚 Table of Contents

1. [🧠 1. Understand What a Load Balancer Is](#1--understand-what-a-load-balancer-is)
2. [💡 2. Understand the Problem with a Real-Life Example](#2--understand-the-problem-with-a-real-life-example)
3. [🔍 3. Dry Run the Core Algorithms](#3--dry-run-the-core-algorithms)
   - [3.1 Round Robin Dry Run](#31-round-robin-modulo-arithmetic)
   - [3.2 Weighted Round Robin Dry Run](#32-weighted-round-robin-smooth-interleaving)
   - [3.3 Least Connections Dry Run](#33-least-connections-tracking)
   - [3.4 Consistent Hashing Dry Run](#34-consistent-hashing-ring)
4. [🛠️ 4. Understand Which Data Structures We Need](#4-️-understand-which-data-structures-we-need)
5. [💻 5. Complete C++ Code Implementation](#5--write-the-complete-c-code)
6. [📖 6. Line-by-Line Code Explanation](#6--understand-the-code-line-by-line)
7. [⏱️ 7. Time and Space Complexity Analysis](#7-️-analyze-time-and-space-complexity)
8. [🎯 8. Final Revision Notes for Interviews](#8--final-revision-notes-for-interviews)

---

# 1. 🧠 Understand What a Load Balancer Is

## What is a Load Balancer?

A **Load Balancer (LB)** is a specialized traffic controller that sits between client users and a pool of backend servers. Its primary job is to **distribute incoming network requests uniformly and intelligently across all available healthy servers**.

```text
               CLIENTS
        ┌─────┬───────┬─────┐
        │ R1  │  R2   │ R3  │
        └─────┴───────┴─────┘
                  │
                  ▼
         [ LOAD BALANCER ]
        (Traffic Controller)
        ┌─────────┼─────────┐
        │         │         │
        ▼         ▼         ▼
    [Server A] [Server B] [Server C]
```

### Why do we need a Load Balancer?

1. **Prevents Server Overload:** If $100{,}000$ users hit a single server simultaneously, its CPU and RAM exhaust, causing latency spikes and crashes. A load balancer spreads the load across $10$ servers, so each handles only $10{,}000$ requests.
2. **High Availability (HA) & Fault Tolerance:** If `Server B` crashes, the load balancer detects the failure via heartbeat health checks and immediately redirects incoming traffic to `Server A` and `Server C` with zero downtime.
3. **Horizontal Scalability:** When business traffic doubles on Black Friday, engineers can launch $10$ new servers. The load balancer automatically incorporates them into the rotation without changing client URLs or DNS records.
4. **Maintenance & Rolling Updates:** Servers can be gracefully drained and taken offline for operating system security patches one by one without disconnecting users.

---

# 2. 💡 Understand the Problem with a Real-Life Example

## The Bank Branch Analogy

Imagine walking into a large bank branch. There are **three teller windows** open:
* **Teller A**
* **Teller B**
* **Teller C**

Outside the counters, customers arrive one after another.

```text
CUSTOMERS: [ Alice, Bob, Charlie, David, Emma, Frank, Grace ]
                               │
                               ▼
                        [ FLOOR MANAGER ]
                        (Load Balancer)
                  ┌────────────┼────────────┐
                  │            │            │
                  ▼            ▼            ▼
             [Teller A]   [Teller B]   [Teller C]
```

The bank manager acts as the **Load Balancer**. Depending on how the manager directs customers, different algorithms emerge:

### Scenario 1: Round Robin (Simple Turns)
* Customer 1 (Alice) $\to$ **Teller A**
* Customer 2 (Bob) $\to$ **Teller B**
* Customer 3 (Charlie) $\to$ **Teller C**
* Customer 4 (David) $\to$ **Teller A**
* Customer 5 (Emma) $\to$ **Teller B**
* Customer 6 (Frank) $\to$ **Teller C**

*What happens if Customer 1 has a 45-minute commercial mortgage inquiry while Customers 2 and 3 only need a quick 30-second balance check?*  
Teller A is overwhelmed while B and C sit idle! Simple turns assume every customer takes the same amount of time.

### Scenario 2: Weighted Round Robin (Senior vs. Junior Tellers)
Suppose:
* **Teller A** is a Senior Veteran (can process 3 customers quickly).
* **Teller B** is an Experienced Officer (can process 2 customers).
* **Teller C** is a New Trainee (can process 1 customer).

The manager gives work in proportion to ability:  
**Pattern:** `[A, B, A, C, B, A]` $\implies$ 3 to A, 2 to B, 1 to C.

### Scenario 3: Least Connections (Queue Count Observation)
The manager looks at how many customers are currently standing in front of each counter:
* Teller A has $3$ people waiting.
* Teller B has $1$ person waiting.
* Teller C has $4$ people waiting.

When the next customer arrives, the manager directs them to **Teller B** because Teller B has the shortest active queue.

### Scenario 4: IP Hash / Consistent Hashing (Relationship Banking)
Certain clients always need the same banker because that banker holds their paper file or local cached records. If customer ID or IP address maps to Teller B, they are consistently routed to Teller B unless Teller B is sick, in which case their work shifts cleanly to the next available teller.

---

# 3. 🔍 Dry Run the Core Algorithms

Let's perform a step-by-step dry run for each major algorithm.

---

## 3.1 Round Robin (Modulo Arithmetic)

* **Pool:** 3 Servers: `Server A` (index 0), `Server B` (index 1), `Server C` (index 2). Total $N = 3$.
* **Formula:**
  $$\text{Target Index} = \text{currentIndex}$$
  $$\text{currentIndex} = (\text{currentIndex} + 1) \pmod N$$

### 7 Sequential Requests Trace:

| Request | `currentIndex` Before | Calculation | Chosen Server | `currentIndex` After | Result / Notes |
|:---:|:---:|:---|:---:|:---:|---|
| **R1** | 0 | `servers[0]` $\to$ `(0+1)%3` | **Server A** | 1 | Request 1 lands on A |
| **R2** | 1 | `servers[1]` $\to$ `(1+1)%3` | **Server B** | 2 | Request 2 lands on B |
| **R3** | 2 | `servers[2]` $\to$ `(2+1)%3` | **Server C** | 0 | Request 3 lands on C; wraps to 0 |
| **R4** | 0 | `servers[0]` $\to$ `(0+1)%3` | **Server A** | 1 | Cycle 2 begins on A |
| **R5** | 1 | `servers[1]` $\to$ `(1+1)%3` | **Server B** | 2 | Lands on B |
| **R6** | 2 | `servers[2]` $\to$ `(2+1)%3` | **Server C** | 0 | Lands on C; wraps to 0 |
| **R7** | 0 | `servers[0]` $\to$ `(0+1)%3` | **Server A** | 1 | Lands on A |

* **Final Distribution:** Server A = 3 requests, Server B = 2 requests, Server C = 2 requests. Strict circular fairness.

---

## 3.2 Weighted Round Robin (Smooth Interleaving)

* **Pool:** Server A (weight 3), Server B (weight 2), Server C (weight 1). Total Weight $S = 3 + 2 + 1 = 6$.
* Rather than sending 3 consecutive requests to A (`A, A, A, B, B, C`), the modern Nginx smooth algorithm interleaves them evenly (`A, B, A, C, B, A`), preventing initial request spikes on Server A.

### State Transition Table:
| Request | Effective Weights Before Selection | Winner (Max Weight) | Weight Subtraction ($-\text{Total}$) | Restored Weights After Turn |
|:---:|:---|:---:|:---:|:---|
| **R1** | A:3, B:2, C:1 | **Server A** | $3 - 6 = -3$ | A: 0, B: 4, C: 2 |
| **R2** | A:0, B:4, C:2 | **Server B** | $4 - 6 = -2$ | A: 3, B: 0, C: 3 |
| **R3** | A:3, B:0, C:3 | **Server A** | $3 - 6 = -3$ | A: 0, B: 2, C: 4 |
| **R4** | A:0, B:2, C:4 | **Server C** | $4 - 6 = -2$ | A: 3, B: 4, C: 0 |
| **R5** | A:3, B:4, C:0 | **Server B** | $4 - 6 = -2$ | A: 6, B: 0, C: 1 |
| **R6** | A:6, B:0, C:1 | **Server A** | $6 - 6 = 0$  | A: 3, B: 2, C: 1 |

* **Sequence Produced:** `Server A` $\to$ `Server B` $\to$ `Server A` $\to$ `Server C` $\to$ `Server B` $\to$ `Server A`. Perfectly smooth!

---

## 3.3 Least Connections Tracking

* **Pool:**
  * `Server A`: 5 active connections
  * `Server B`: 2 active connections
  * `Server C`: 8 active connections

```text
Current Active Connections: [ Server A: 5 | Server B: 2 | Server C: 8 ]
                                             ▲
                                             │
                              Lowest Count: Server B (2)
```

1. **Incoming Request `R1`:**
   * Compares active counts: $\min(5, 2, 8) = 2$ (`Server B`).
   * Routes `R1` to `Server B`.
   * `Server B` active count increments: $2 \to 3$.
   * State: `[ Server A: 5 | Server B: 3 | Server C: 8 ]`.

2. **Incoming Request `R2`:**
   * Compares active counts: $\min(5, 3, 8) = 3$ (`Server B`).
   * Routes `R2` to `Server B`.
   * `Server B` active count increments: $3 \to 4$.
   * State: `[ Server A: 5 | Server B: 4 | Server C: 8 ]`.

3. **Incoming Request `R3`:**
   * Compares active counts: $\min(5, 4, 8) = 4$ (`Server B`).
   * Routes `R3` to `Server B`.
   * `Server B` active count increments: $4 \to 5$.
   * State: `[ Server A: 5 | Server B: 5 | Server C: 8 ]`.

4. **Connection Finishes on `Server A`:**
   * Server A finishes 2 requests: $5 \to 3$.
   * State: `[ Server A: 3 | Server B: 5 | Server C: 8 ]`.
   * Now `Server A` has the lowest connections and will receive subsequent traffic!

---

## 3.4 Consistent Hashing Ring

### The Flaw of Traditional Modulo:
With $\text{hash}(key) \pmod N$, if $N = 3 \to 4$ (one server added), over **75% of cached keys remap**, causing a massive database cache stampede!

### Consistent Hashing Solution:
1. Arrange a circular 32-bit address space from $0$ to $2^{32}-1$.
2. Hash server names and place them on the circle.
3. Hash incoming keys onto the same circle.
4. Walk **clockwise** from the key's position; the first server encountered handles the key.

```text
                     0 / 2^32-1
                    .-----.
                 .-'       '-.
             Server A             Server B
           .                           .
          |   key_1                     |
          |                             |
           .                           .
             Server C             [New Server D]
                 '-.       .-'
                    '-----'
```

* **Virtual Nodes (vNodes):** To prevent clusters of keys from overloading a single physical server due to uneven hash gaps, each physical server is replicated $V$ times (e.g., `ServerA#VN0`, `ServerA#VN1`, `ServerA#VN2`).

---

# 4. 🛠️ Understand Which Data Structures We Need

| Algorithm | Primary Data Structures | Purpose | Complexity |
|---|---|---|:---:|
| **Round Robin** | `std::vector<std::string>` + `size_t currentIndex` | Stores server addresses and tracks the circular modulo pointer | Lookup: $O(1)$ |
| **Weighted Round Robin** | `std::vector<Server>` (`weight`, `currentWeight`) | Tracks dynamic balance weights across heterogeneous servers | Lookup: $O(N)$ or $O(1)$ |
| **Least Connections** | `std::vector<Server>` (`activeConnections`) | Tracks in-flight TCP connections per server | Lookup: $O(N)$ or $O(\log N)$ |
| **IP Hash** | `std::hash<std::string>` + `std::vector<std::string>` | Hashes client IP modulo total healthy backends | Lookup: $O(1)$ |
| **Consistent Hashing** | `std::map<uint32_t, std::string>` | **Red-Black Tree** maintaining sorted 360° virtual node positions; uses binary search `std::map::lower_bound` | Lookup: $O(\log(N \times V))$ |

---

# 5. 💻 Write the Complete C++ Code

Here is a unified, production-grade Load Balancer in C++ demonstrating all core algorithms cleanly in one modular architecture:

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <cstdint>

// Represents a backend server instance with capacity and connection metrics
struct BackendServer {
    std::string name;
    int weight;
    int activeConnections;
    bool isHealthy;

    BackendServer(const std::string& n, int w = 1)
        : name(n), weight(w), activeConnections(0), isHealthy(true) {}
};

// -------------------------------------------------------------
// 1. Production Round Robin Load Balancer
// -------------------------------------------------------------
class RoundRobinLB {
private:
    std::vector<std::string> servers;
    size_t currentIndex;

public:
    RoundRobinLB() : currentIndex(0) {}

    void addServer(const std::string& server) {
        servers.push_back(server);
    }

    std::string routeRequest(const std::string& clientRequestId) {
        if (servers.empty()) {
            return "ERROR_NO_SERVERS";
        }

        std::string selected = servers[currentIndex];
        // Circular modulo pointer advance
        currentIndex = (currentIndex + 1) % servers.size();
        return selected;
    }
};

// -------------------------------------------------------------
// 2. Production Least Connections Load Balancer
// -------------------------------------------------------------
class LeastConnectionsLB {
private:
    std::vector<BackendServer> servers;

public:
    void addServer(const std::string& name, int weight = 1) {
        servers.emplace_back(name, weight);
    }

    int routeRequest(const std::string& clientRequestId) {
        if (servers.empty()) return -1;

        int selectedIndex = -1;
        int minConnections = INT32_MAX;

        for (size_t i = 0; i < servers.size(); ++i) {
            if (servers[i].isHealthy && servers[i].activeConnections < minConnections) {
                minConnections = servers[i].activeConnections;
                selectedIndex = static_cast<int>(i);
            }
        }

        if (selectedIndex != -1) {
            servers[selectedIndex].activeConnections++;
        }
        return selectedIndex;
    }

    void finishRequest(int serverIndex) {
        if (serverIndex >= 0 && serverIndex < static_cast<int>(servers.size())) {
            // Guard against underflow
            if (servers[serverIndex].activeConnections > 0) {
                servers[serverIndex].activeConnections--;
            }
        }
    }

    const BackendServer& getServer(int index) const {
        return servers[index];
    }
};

// -------------------------------------------------------------
// 3. Consistent Hashing Ring with Virtual Nodes
// -------------------------------------------------------------
class ConsistentHashRingLB {
private:
    int virtualNodes;
    std::map<uint32_t, std::string> ring; // Sorted 360-degree hash ring
    std::hash<std::string> hasher;

    uint32_t getHash(const std::string& input) {
        return static_cast<uint32_t>(hasher(input));
    }

public:
    explicit ConsistentHashRingLB(int vNodes = 3) : virtualNodes(vNodes < 1 ? 1 : vNodes) {}

    void addServer(const std::string& serverName) {
        for (int i = 0; i < virtualNodes; ++i) {
            std::string vNodeKey = serverName + "#VN" + std::to_string(i);
            uint32_t hashVal = getHash(vNodeKey);
            ring[hashVal] = serverName;
        }
    }

    void removeServer(const std::string& serverName) {
        for (int i = 0; i < virtualNodes; ++i) {
            std::string vNodeKey = serverName + "#VN" + std::to_string(i);
            uint32_t hashVal = getHash(vNodeKey);
            ring.erase(hashVal);
        }
    }

    std::string routeKey(const std::string& key) {
        if (ring.empty()) return "ERROR_NO_SERVERS";

        uint32_t keyHash = getHash(key);

        // Binary search clockwise on Red-Black BST: first node with hash >= keyHash
        auto it = ring.lower_bound(keyHash);

        // Circular wrap-around: if past the end, map to the first node
        if (it == ring.end()) {
            it = ring.begin();
        }

        return it->second;
    }

    size_t getTotalTokens() const { return ring.size(); }
};

// -------------------------------------------------------------
// Verification Simulation
// -------------------------------------------------------------
int main() {
    std::cout << "=================================================\n";
    std::cout << "    High-Performance Load Balancers in C++17     \n";
    std::cout << "=================================================\n\n";

    // 1. Test Round Robin
    std::cout << "--- 1. Testing Round Robin ---\n";
    RoundRobinLB rrlb;
    rrlb.addServer("Server_A");
    rrlb.addServer("Server_B");
    rrlb.addServer("Server_C");

    for (int i = 1; i <= 6; ++i) {
        std::cout << "Request R" << i << " -> " << rrlb.routeRequest("R" + std::to_string(i)) << "\n";
    }

    // 2. Test Least Connections
    std::cout << "\n--- 2. Testing Least Connections ---\n";
    LeastConnectionsLB lclb;
    lclb.addServer("Server_A");
    lclb.addServer("Server_B");
    lclb.addServer("Server_C");

    // Route 3 requests
    int s1 = lclb.routeRequest("Req1");
    int s2 = lclb.routeRequest("Req2");
    int s3 = lclb.routeRequest("Req3");
    std::cout << "Req1 landed on: " << lclb.getServer(s1).name << " (Active: " << lclb.getServer(s1).activeConnections << ")\n";
    std::cout << "Req2 landed on: " << lclb.getServer(s2).name << " (Active: " << lclb.getServer(s2).activeConnections << ")\n";
    std::cout << "Req3 landed on: " << lclb.getServer(s3).name << " (Active: " << lclb.getServer(s3).activeConnections << ")\n";

    // Release connection on s1
    lclb.finishRequest(s1);
    std::cout << "Finished Req1 on " << lclb.getServer(s1).name << ". Next request should route to it:\n";
    int s4 = lclb.routeRequest("Req4");
    std::cout << "Req4 landed on: " << lclb.getServer(s4).name << " (Active: " << lclb.getServer(s4).activeConnections << ")\n";

    // 3. Test Consistent Hashing
    std::cout << "\n--- 3. Testing Consistent Hashing ---\n";
    ConsistentHashRingLB ring(3);
    ring.addServer("CacheServer_Alpha");
    ring.addServer("CacheServer_Beta");
    ring.addServer("CacheServer_Gamma");

    std::vector<std::string> sessionKeys = {"user_1001", "user_1002", "user_1003", "user_1004"};
    for (const auto& key : sessionKeys) {
        std::cout << "Key '" << key << "' maps to: " << ring.routeKey(key) << "\n";
    }

    return 0;
}
```

---

# 6. 📖 Understand the Code Line by Line

## 6.1 Round Robin: Modulo Index Pointer
```cpp
std::string selected = servers[currentIndex];
currentIndex = (currentIndex + 1) % servers.size();
return selected;
```
1. `servers[currentIndex]`: Reads the server currently pointed to.
2. `(currentIndex + 1) % servers.size()`: Advances the index by 1. When it reaches `servers.size()`, the modulo `%` operator resets it back to `0`.
3. **Guard against empty vector:** If `servers.empty()`, dividing by zero causes a crash. The check `if (servers.empty()) return "ERROR_NO_SERVERS";` protects production services.

---

## 6.2 Least Connections: Linear Minimum Scan & Underflow Guard
```cpp
for (size_t i = 0; i < servers.size(); ++i) {
    if (servers[i].isHealthy && servers[i].activeConnections < minConnections) {
        minConnections = servers[i].activeConnections;
        selectedIndex = static_cast<int>(i);
    }
}
```
1. Scans healthy servers to identify the instance with the smallest `activeConnections`.
2. Increments `servers[selectedIndex].activeConnections++` immediately to reserve the slot for the in-flight request.
3. In `finishRequest()`:
   ```cpp
   if (servers[serverIndex].activeConnections > 0) {
       servers[serverIndex].activeConnections--;
   }
   ```
   **Underflow Guard:** Never allows the counter to drop below 0 if an uncounted connection disconnects.

---

## 6.3 Consistent Hashing Ring: Binary Search on Red-Black Tree
```cpp
auto it = ring.lower_bound(keyHash);
if (it == ring.end()) {
    it = ring.begin();
}
return it->second;
```
1. `ring`: An `std::map<uint32_t, std::string>` sorted in ascending order of 32-bit token hashes.
2. `ring.lower_bound(keyHash)`: Executes a binary search in $O(\log (\text{vNodes}))$, returning an iterator to the first virtual node whose hash is $\ge \text{keyHash}$ (moving clockwise).
3. `if (it == ring.end()) it = ring.begin();`: **Circular wrap-around**. If the key's hash is larger than the highest server hash on the ring, it wraps around to the very first server token at the start of the ring ($0^\circ$).

---

# 7. ⏱️ Analyze Time and Space Complexity

| Algorithm | Routing Decision Time | Server Add / Remove Time | Auxiliary Space Complexity | Optimal Workload |
|---|:---:|:---:|:---:|---|
| **Round Robin** | **$O(1)$** | $O(1)$ add / $O(N)$ remove | $O(N)$ | Homogeneous servers, uniform short requests |
| **Weighted Round Robin** | **$O(N)$** or $O(1)$ | $O(1)$ | $O(N)$ | Heterogeneous server capacities (e.g., 16-core vs 4-core) |
| **Least Connections** | **$O(N)$** (or $O(\log N)$ heap) | $O(1)$ | $O(N)$ | Long-lived connections (WebSockets, SQL transactions) |
| **IP Hash** | **$O(1)$** | $O(1)$ | $O(N)$ | Stateful sessions without a centralized Redis session store |
| **Consistent Hashing** | **$O(\log (N \cdot V))$** | $O(V \log(N \cdot V))$ | $O(N \cdot V)$ | Distributed caching (Redis, Memcached), database sharding |

* Where $N = \text{number of servers}$, and $V = \text{virtual nodes per server}$ (typically 100–300 in production).

---

# 8. 🎯 Final Revision Notes for Interviews

### ⭐ Layer 4 vs. Layer 7 Comparison
* **Layer 4 (Transport / TCP/UDP):** Routes solely by client IP and TCP port. Super fast (millions of requests per second), zero decryption overhead, cannot inspect HTTP URLs. Example: AWS NLB.
* **Layer 7 (Application / HTTP):** Decrypts SSL/TLS. Can inspect URL paths (`/api` vs. `/images`), cookies, and request headers. Supports smart microservice routing. Example: Nginx, AWS ALB.

---

### ⭐ High Availability & Failover Patterns
1. **Active-Passive with VRRP / Keepalived:** Two load balancers share a virtual IP (VIP). If the primary load balancer dies, the backup claims the IP within milliseconds.
2. **Active-Active with BGP Anycast:** Multiple load balancers advertise the exact same IP address to internet routers. Traffic automatically routes to the closest healthy data center via BGP routing.

---

### ⭐ Five Most Important Code Lines to Remember

1. **Round Robin Modulo Increment:**
   ```cpp
   currentIndex = (currentIndex + 1) % servers.size();
   ```
2. **Least Connections Underflow Guard:**
   ```cpp
   if (servers[i].activeConnections > 0) servers[i].activeConnections--;
   ```
3. **Virtual Node Key Generation:**
   ```cpp
   std::string vNodeKey = serverName + "#VN" + std::to_string(v);
   ```
4. **Clockwise Hash Ring Binary Search:**
   ```cpp
   auto it = ring.lower_bound(keyHash);
   ```
5. **Hash Ring Circular Wrap-Around:**
   ```cpp
   if (it == ring.end()) it = ring.begin();
   ```
