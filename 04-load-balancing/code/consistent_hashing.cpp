/**
 * @file consistent_hashing.cpp
 * @brief Consistent Hashing Ring with Virtual Nodes in C++17
 * 
 * Features & Bug Fixes:
 * - Proper <cstdint> inclusion for uint32_t portability across all compilers
 * - Empty ring handling
 * - Minimal key remapping proof when adding nodes
 */

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <functional>
#include <vector>
#include <cstdint>

class ConsistentHashRing {
private:
    int virtualNodes; // Replicas per physical server
    std::map<uint32_t, std::string> ring; // Sorted 360-degree hash ring
    std::hash<std::string> hasher;

    uint32_t getHash(const std::string& input) {
        return static_cast<uint32_t>(hasher(input));
    }

public:
    explicit ConsistentHashRing(int vNodes = 3) : virtualNodes(vNodes) {
        if (virtualNodes <= 0) virtualNodes = 1; // Bug Fix: minimum 1 vNode
    }

    void addServer(const std::string& serverName) {
        for (int i = 0; i < virtualNodes; ++i) {
            std::string vNodeKey = serverName + "#VN" + std::to_string(i);
            uint32_t hashVal = getHash(vNodeKey);
            ring[hashVal] = serverName;
        }
        std::cout << "[Ring] Added '" << serverName << "' with " 
                  << virtualNodes << " virtual nodes.\n";
    }

    void removeServer(const std::string& serverName) {
        for (int i = 0; i < virtualNodes; ++i) {
            std::string vNodeKey = serverName + "#VN" + std::to_string(i);
            uint32_t hashVal = getHash(vNodeKey);
            ring.erase(hashVal);
        }
        std::cout << "[Ring] Removed '" << serverName << "' and its virtual nodes.\n";
    }

    std::string routeKey(const std::string& key) {
        // Bug Fix: Guard against empty ring
        if (ring.empty()) return "ERROR_NO_SERVERS";

        uint32_t keyHash = getHash(key);

        // Find the first virtual node at or clockwise after keyHash
        auto it = ring.lower_bound(keyHash);

        // Circular wrap-around
        if (it == ring.end()) {
            it = ring.begin();
        }

        return it->second;
    }

    size_t getTotalVirtualNodes() const { return ring.size(); }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << " Consistent Hashing with Virtual Nodes Simulation\n";
    std::cout << "=================================================\n\n";

    ConsistentHashRing ring(3);

    // Initial 3 servers
    ring.addServer("Server_A");
    ring.addServer("Server_B");
    ring.addServer("Server_C");

    std::vector<std::string> keys = {
        "user_profile_101",
        "user_profile_202",
        "user_profile_303",
        "user_profile_404",
        "user_profile_505"
    };

    std::cout << "\n--- Initial Key Distribution ---\n";
    for (const auto& k : keys) {
        std::cout << "  Key [" << k << "] -> Handled by: " << ring.routeKey(k) << "\n";
    }

    std::cout << "\n--- Scaling Out: Adding Server_D ---\n";
    ring.addServer("Server_D");

    std::cout << "\n--- Key Distribution After Adding Server_D ---\n";
    for (const auto& k : keys) {
        std::cout << "  Key [" << k << "] -> Handled by: " << ring.routeKey(k) << "\n";
    }

    std::cout << "\nNotice: Only a small subset of keys migrated to Server_D;\n";
    std::cout << "the rest stayed on their original servers without disruption!\n";

    return 0;
}

