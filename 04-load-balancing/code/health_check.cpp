/**
 * @file health_check.cpp
 * @brief Health Checking and Failover Simulation in C++17
 * 
 * Features & Bug Fixes:
 * - Dynamic server crash simulation
 * - Dynamic server recovery simulation
 * - Safe failover when 1 or all nodes are down
 */

#include <iostream>
#include <vector>
#include <string>

struct BackendNode {
    std::string name;
    bool isAlive;
    int consecutiveFailures;
};

class ResilientLoadBalancer {
private:
    std::vector<BackendNode> servers;
    size_t roundRobinIndex;

public:
    ResilientLoadBalancer() : roundRobinIndex(0) {
        servers.push_back({"Server A", true, 0});
        servers.push_back({"Server B", true, 0});
        servers.push_back({"Server C", true, 0});
    }

    void simulateFailure(const std::string& serverName) {
        for (auto& s : servers) {
            if (s.name == serverName) {
                s.isAlive = false;
                s.consecutiveFailures = 3;
                std::cout << "\n>>> [ALERT] Health check failed for " << serverName 
                          << "! Node marked UNHEALTHY and removed from active pool. <<<\n\n";
            }
        }
    }

    void simulateRecovery(const std::string& serverName) {
        for (auto& s : servers) {
            if (s.name == serverName) {
                s.isAlive = true;
                s.consecutiveFailures = 0;
                std::cout << "\n>>> [HEALTHY] " << serverName 
                          << " passed 3 consecutive heartbeats! Returned to active pool. <<<\n\n";
            }
        }
    }

    std::string routeRequest(const std::string& reqId) {
        size_t total = servers.size();
        for (size_t i = 0; i < total; ++i) {
            size_t candidateIdx = (roundRobinIndex + i) % total;
            if (servers[candidateIdx].isAlive) {
                roundRobinIndex = (candidateIdx + 1) % total;
                std::cout << "[Request " << reqId << "] -> Routed to: " 
                          << servers[candidateIdx].name << "\n";
                return servers[candidateIdx].name;
            }
        }
        std::cout << "[Request " << reqId << "] -> CRITICAL: All backend servers are down!\n";
        return "ERROR_ALL_SERVERS_DOWN";
    }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "  Health Check and Automatic Failover Simulation \n";
    std::cout << "=================================================\n\n";

    ResilientLoadBalancer lb;

    std::cout << "--- 1. All 3 servers are healthy ---\n";
    lb.routeRequest("R1");
    lb.routeRequest("R2");
    lb.routeRequest("R3");

    // Server B crashes!
    lb.simulateFailure("Server B");

    std::cout << "--- 2. Routing after Server B failure (Must skip Server B) ---\n";
    lb.routeRequest("R4");
    lb.routeRequest("R5");
    lb.routeRequest("R6");
    lb.routeRequest("R7");

    // Server B recovers
    lb.simulateRecovery("Server B");
    std::cout << "--- 3. Routing after Server B recovery ---\n";
    lb.routeRequest("R8");
    lb.routeRequest("R9");

    return 0;
}

