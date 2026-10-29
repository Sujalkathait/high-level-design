/**
 * @file least_connections.cpp
 * @brief Least Connections Load Balancing in C++17
 * 
 * Features & Bug Fixes:
 * - Empty pool safety checks
 * - Negative connection count underflow protection in finishRequest
 * - Complete status monitoring
 */

#include <iostream>
#include <vector>
#include <string>

struct Server {
    std::string name;
    int activeConnections;
};

class LeastConnectionsLB {
private:
    std::vector<Server> servers;

public:
    explicit LeastConnectionsLB(const std::vector<Server>& serverList)
        : servers(serverList) {}

    int routeNewRequest(const std::string& requestId) {
        // Bug Fix: Empty pool check
        if (servers.empty()) {
            std::cout << "[Request " << requestId << "] -> FAILED: No servers available!\n";
            return -1;
        }

        int selectedIndex = 0;
        for (size_t i = 1; i < servers.size(); ++i) {
            if (servers[i].activeConnections < servers[selectedIndex].activeConnections) {
                selectedIndex = i;
            }
        }

        std::cout << "[Request " << requestId << "] -> Routed to: " 
                  << servers[selectedIndex].name 
                  << " (Active Connections: " << servers[selectedIndex].activeConnections 
                  << " -> " << servers[selectedIndex].activeConnections + 1 << ")\n";

        servers[selectedIndex].activeConnections++;
        return selectedIndex;
    }

    void finishRequest(int serverIndex) {
        if (serverIndex >= 0 && serverIndex < static_cast<int>(servers.size())) {
            // Bug Fix: Never decrement below 0 to avoid negative connection underflow
            if (servers[serverIndex].activeConnections > 0) {
                servers[serverIndex].activeConnections--;
                std::cout << "[Completed] " << servers[serverIndex].name 
                          << " connection released. (Now active: " 
                          << servers[serverIndex].activeConnections << ")\n";
            } else {
                std::cout << "[WARN] " << servers[serverIndex].name 
                          << " already has 0 active connections. Ignored underflow.\n";
            }
        }
    }

    void printStatus() const {
        std::cout << "  Current Active Connections: [ ";
        for (const auto& s : servers) {
            std::cout << s.name << ": " << s.activeConnections << " | ";
        }
        std::cout << "]\n";
    }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "       Least Connections Simulation              \n";
    std::cout << "=================================================\n\n";

    // Initial state: Server A has 5, Server B has 2, Server C has 8
    std::vector<Server> initialServers = {
        {"Server A", 5},
        {"Server B", 2},
        {"Server C", 8}
    };

    LeastConnectionsLB lb(initialServers);
    lb.printStatus();

    std::cout << "\n1. New request R1 arrives:\n";
    int sIdx = lb.routeNewRequest("R1"); // Selects Server B (2 is lowest)
    lb.printStatus();

    std::cout << "\n2. Server B finishes an ongoing request:\n";
    lb.finishRequest(sIdx);
    lb.printStatus();

    std::cout << "\n3. Another new request R2 arrives:\n";
    lb.routeNewRequest("R2");
    lb.printStatus();

    std::cout << "\n4. Edge-case test: Attempting to finish on idle server with 0 conns:\n";
    Server emptyServer{"Server Test", 0};
    LeastConnectionsLB edgeLB({emptyServer});
    edgeLB.finishRequest(0); // Safely handled, no negative count

    return 0;
}

