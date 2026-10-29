/**
 * @file round_robin.cpp
 * @brief Production-Ready Round Robin Load Balancer in C++17
 * 
 * Modulo arithmetic: index = (index + 1) % total_servers
 * 
 * Features & Bug Fixes:
 * - Empty server pool safety guards
 * - Dynamic server additions and removals
 * - Zero divide protection
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

class RoundRobinLoadBalancer {
private:
    std::vector<std::string> servers;
    size_t currentIndex;

public:
    RoundRobinLoadBalancer() : currentIndex(0) {}

    explicit RoundRobinLoadBalancer(const std::vector<std::string>& serverList)
        : servers(serverList), currentIndex(0) {}

    void addServer(const std::string& server) {
        servers.push_back(server);
        std::cout << "[POOL] Added server: " << server << " (Total: " << servers.size() << ")\n";
    }

    bool removeServer(const std::string& server) {
        auto it = std::find(servers.begin(), servers.end(), server);
        if (it != servers.end()) {
            servers.erase(it);
            if (currentIndex >= servers.size() && !servers.empty()) {
                currentIndex = 0; // Reset index to prevent out-of-bounds
            }
            std::cout << "[POOL] Removed server: " << server << " (Remaining: " << servers.size() << ")\n";
            return true;
        }
        return false;
    }

    std::string routeRequest(const std::string& requestId) {
        // Bug Fix: Guard against division by zero if server pool is empty
        if (servers.empty()) {
            std::cout << "[Request " << requestId << "] -> FAILED: No servers available!\n";
            return "ERROR_NO_SERVERS";
        }

        std::string selectedServer = servers[currentIndex];
        size_t prevIndex = currentIndex;

        // Circular modulo increment
        currentIndex = (currentIndex + 1) % servers.size();

        std::cout << "[Request " << requestId << "] -> Routed to: " 
                  << selectedServer << " (Index: " << prevIndex 
                  << " -> Next: " << currentIndex << ")\n";

        return selectedServer;
    }

    size_t getServerCount() const { return servers.size(); }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "       Round Robin Load Balancing Simulation     \n";
    std::cout << "=================================================\n\n";

    RoundRobinLoadBalancer lb({"Server A", "Server B", "Server C"});

    // Seven sequential client requests
    std::vector<std::string> requests = {"R1", "R2", "R3", "R4", "R5", "R6", "R7"};

    for (const auto& req : requests) {
        lb.routeRequest(req);
    }

    std::cout << "\n--- Testing Dynamic Pool Resize ---\n";
    lb.addServer("Server D"); // Pool is now A, B, C, D
    lb.routeRequest("R8");
    lb.routeRequest("R9");

    lb.removeServer("Server B"); // Remove B
    lb.routeRequest("R10");
    lb.routeRequest("R11");

    return 0;
}

