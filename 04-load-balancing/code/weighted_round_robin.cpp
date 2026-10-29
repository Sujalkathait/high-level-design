/**
 * @file weighted_round_robin.cpp
 * @brief Smooth Weighted Round Robin Load Balancer in C++17 (NGINX Algorithm)
 * 
 * Features & Bug Fixes:
 * - Empty pool guards
 * - Non-positive weight validation (weight <= 0 ignored)
 * - True mathematical interleaved distribution without bursting
 */

#include <iostream>
#include <vector>
#include <string>

struct ServerNode {
    std::string name;
    int weight;          // Static capacity configured
    int currentWeight;   // Dynamically updated weight
};

class WeightedRoundRobinLB {
private:
    std::vector<ServerNode> servers;

public:
    bool addServer(const std::string& name, int weight) {
        // Bug Fix: Validate weight
        if (weight <= 0) {
            std::cout << "[ERROR] Cannot add server " << name << " with non-positive weight: " << weight << "\n";
            return false;
        }
        servers.push_back({name, weight, 0});
        std::cout << "[POOL] Added " << name << " (Weight: " << weight << ")\n";
        return true;
    }

    std::string routeRequest(const std::string& requestId) {
        // Bug Fix: Guard against empty server pool
        if (servers.empty()) {
            std::cout << "[Request " << requestId << "] -> FAILED: No servers available!\n";
            return "ERROR_NO_SERVERS";
        }

        int totalWeight = 0;
        int bestIndex = 0;

        // Step 1: Accumulate weights
        for (size_t i = 0; i < servers.size(); ++i) {
            servers[i].currentWeight += servers[i].weight;
            totalWeight += servers[i].weight;

            // Step 2: Pick node with highest current weight
            if (servers[i].currentWeight > servers[bestIndex].currentWeight) {
                bestIndex = i;
            }
        }

        // Step 3: Deplete selected node's currentWeight by totalWeight
        servers[bestIndex].currentWeight -= totalWeight;

        std::cout << "[Request " << requestId << "] -> Routed to: " 
                  << servers[bestIndex].name << " (Weights: ";
        for (const auto& s : servers) {
            std::cout << s.name << ":" << s.currentWeight << " ";
        }
        std::cout << ")\n";

        return servers[bestIndex].name;
    }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "  Weighted Round Robin Simulation (A=3, B=2, C=1)\n";
    std::cout << "=================================================\n\n";

    WeightedRoundRobinLB lb;
    lb.addServer("Server A", 3);
    lb.addServer("Server B", 2);
    lb.addServer("Server C", 1);

    // Over 6 requests, Server A must get 3, Server B gets 2, Server C gets 1
    std::vector<std::string> requests = {"R1", "R2", "R3", "R4", "R5", "R6"};

    for (const auto& req : requests) {
        lb.routeRequest(req);
    }

    return 0;
}

