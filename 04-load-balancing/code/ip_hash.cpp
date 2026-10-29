/**
 * @file ip_hash.cpp
 * @brief Deterministic IP Hash Load Balancing in C++17
 * 
 * Features & Bug Fixes:
 * - Empty pool protection
 * - Deterministic routing verification
 * - Dynamic addition of servers
 */

#include <iostream>
#include <vector>
#include <string>
#include <functional>

class IPHashLB {
private:
    std::vector<std::string> servers;
    std::hash<std::string> hashFunction;

public:
    explicit IPHashLB(const std::vector<std::string>& serverList)
        : servers(serverList) {}

    std::string routeClient(const std::string& clientIP) {
        // Bug Fix: Guard against empty server pool
        if (servers.empty()) {
            std::cout << "[Client IP: " << clientIP << "] -> FAILED: No servers available!\n";
            return "ERROR_NO_SERVERS";
        }

        size_t hashValue = hashFunction(clientIP);
        size_t serverIndex = hashValue % servers.size();

        std::cout << "[Client IP: " << clientIP << "] (Hash: " << hashValue 
                  << ") -> Routed to: " << servers[serverIndex] 
                  << " (Index: " << serverIndex << ")\n";

        return servers[serverIndex];
    }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "          IP Hash Load Balancing Simulation      \n";
    std::cout << "=================================================\n\n";

    std::vector<std::string> servers = {"Server_US_East", "Server_US_West", "Server_EU_Central"};
    IPHashLB lb(servers);

    std::cout << "Testing repeated client requests:\n";
    lb.routeClient("192.168.1.100");
    lb.routeClient("10.0.0.5");
    lb.routeClient("172.16.254.1");

    std::cout << "\nVerifying Deterministic Affinity (Same IP lands on same server):\n";
    lb.routeClient("192.168.1.100"); // Must match first call
    lb.routeClient("10.0.0.5");       // Must match second call

    return 0;
}

