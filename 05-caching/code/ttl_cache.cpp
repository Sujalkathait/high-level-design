/**
 * @file ttl_cache.cpp
 * @brief Time-To-Live (TTL) Cache with Lazy & Active Expiration in C++17
 * 
 * Approach:
 * - std::chrono::steady_clock: Monotonic clock that never jumps backwards
 * - std::unordered_map<string, TTLEntry>: Key -> {value, expiryTime}
 * - Lazy Eviction on get(): Removes expired item on access
 * - Active Cleanup via cleanupExpired(): Sweeps unread abandoned keys to prevent leaks
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
            std::cout << "[GET] '" << key << "' -> EXPIRED & EVICTED (Treated as Cache MISS)\n";
            return "";
        }

        std::cout << "[GET] '" << key << "' -> HIT (Value: " << it->second.value << ")\n";
        return it->second.value;
    }

    // Active periodic sweep to eliminate unread expired keys (memory leak prevention)
    int cleanupExpired() {
        auto now = std::chrono::steady_clock::now();
        std::vector<std::string> expiredKeys;

        for (const auto& [k, entry] : store) {
            if (now > entry.expiryTime) {
                expiredKeys.push_back(k);
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

    void printState() const {
        std::cout << "  Active Cache Size: " << store.size() << " keys in RAM\n\n";
    }

    size_t size() const { return store.size(); }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "         TTL Cache Simulation with Expiration     \n";
    std::cout << "=================================================\n\n";

    TTLCache ttlCache;

    // Store keys with 200ms TTL
    std::cout << "--- Step 1: Set keys with distinct TTLs ---\n";
    ttlCache.set("session_user_42", "token_abc_xyz_789", std::chrono::milliseconds(200));
    ttlCache.set("abandoned_key", "temporary_data", std::chrono::milliseconds(100));
    ttlCache.printState();

    // Access immediately: Must HIT
    std::cout << "--- Step 2: Immediate access (Before TTL expiry) ---\n";
    ttlCache.get("session_user_42");
    ttlCache.printState();

    std::cout << "Waiting 300ms for TTLs to expire...\n\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    // Lazy eviction demo
    std::cout << "--- Step 3: Access after expiration (Demonstrating Lazy Eviction) ---\n";
    ttlCache.get("session_user_42");
    ttlCache.printState();

    // Active sweep demo: Cleans abandoned_key even though get() was never called on it!
    std::cout << "--- Step 4: Active Periodic Background Cleanup (Demonstrating Active Sweep) ---\n";
    std::cout << "Cache size before sweep: " << ttlCache.size() << "\n";
    ttlCache.cleanupExpired();
    std::cout << "Cache size after sweep: " << ttlCache.size() << "\n";

    return 0;
}
