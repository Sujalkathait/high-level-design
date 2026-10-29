/**
 * @file ttl_cache.cpp
 * @brief Time-To-Live (TTL) Cache with Lazy & Active Expiration in C++17
 * 
 * Features & Bug Fixes:
 * - Lazy eviction on get()
 * - Active cleanupExpired() method: Prevents memory leaks from unread abandoned keys
 * - Uses std::chrono::steady_clock to prevent issues with system clock changes
 */

#include <iostream>
#include <unordered_map>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

struct TTLEntry {
    std::string value;
    std::chrono::steady_clock::time_point expiryTime;
};

class TTLCache {
private:
    std::unordered_map<std::string, TTLEntry> store;

public:
    void set(const std::string& key, const std::string& val, std::chrono::milliseconds ttl) {
        auto expiry = std::chrono::steady_clock::now() + ttl;
        store[key] = {val, expiry};
        std::cout << "[SET] '" << key << "' stored with TTL " << ttl.count() << "ms\n";
    }

    std::string get(const std::string& key) {
        auto it = store.find(key);
        if (it == store.end()) {
            std::cout << "[GET] '" << key << "' -> MISS (Key does not exist)\n";
            return "";
        }

        auto now = std::chrono::steady_clock::now();
        if (now > it->second.expiryTime) {
            // Lazy eviction: Evict when requested after expiration
            store.erase(it);
            std::cout << "[GET] '" << key << "' -> EXPIRED & EVICTED (Treated as Cache Miss)\n";
            return "";
        }

        std::cout << "[GET] '" << key << "' -> HIT (Value: " << it->second.value << ")\n";
        return it->second.value;
    }

    // Bug Fix: Active periodic sweep to eliminate unread expired keys (memory leak prevention)
    int cleanupExpired() {
        auto now = std::chrono::steady_clock::now();
        std::vector<std::string> expiredKeys;

        for (const auto& pair : store) {
            if (now > pair.second.expiryTime) {
                expiredKeys.push_back(pair.first);
            }
        }

        for (const auto& k : expiredKeys) {
            store.erase(k);
        }

        if (!expiredKeys.empty()) {
            std::cout << "[CLEANUP] Actively swept and evicted " << expiredKeys.size() << " expired keys.\n";
        }
        return static_cast<int>(expiredKeys.size());
    }

    size_t size() const { return store.size(); }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "         TTL Cache Simulation with Expiration     \n";
    std::cout << "=================================================\n\n";

    TTLCache ttlCache;

    // Store keys with 200ms TTL
    ttlCache.set("session_user_42", "token_abc_xyz_789", std::chrono::milliseconds(200));
    ttlCache.set("abandoned_key", "temporary_data", std::chrono::milliseconds(100));

    // Access immediately: Must HIT
    ttlCache.get("session_user_42");

    std::cout << "\nWaiting 300ms for TTLs to expire...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    // Lazy eviction demo
    ttlCache.get("session_user_42");

    // Active sweep demo: Cleans abandoned_key even though get() was never called on it!
    std::cout << "\nRunning active background memory cleanup sweep:\n";
    std::cout << "Cache size before sweep: " << ttlCache.size() << "\n";
    ttlCache.cleanupExpired();
    std::cout << "Cache size after sweep:  " << ttlCache.size() << "\n";

    return 0;
}

