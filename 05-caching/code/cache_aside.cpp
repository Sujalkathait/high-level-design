/**
 * @file cache_aside.cpp
 * @brief Cache-Aside Pattern with Simulated Redis, PostgreSQL, and Penetration Defense in C++17
 * 
 * Features:
 * - Read-Through: Cache Check -> DB Fallback -> Cache Write
 * - Write Invalidation: DB Write -> Cache DEL (Invalidation)
 * - Cache Penetration Defense: Caches empty sentinel value for non-existent IDs to protect DB
 */

#include <iostream>
#include <unordered_map>
#include <string>

// Simulated Database (Slow disk/network storage)
class Database {
private:
    std::unordered_map<int, std::string> db;
public:
    Database() {
        db[1] = "Alice (Software Engineer)";
        db[2] = "Bob (Product Manager)";
        db[3] = "Charlie (Site Reliability Engineer)";
    }

    std::string query(int id) {
        std::cout << "  --> [DATABASE DISK READ] Scanning table for user_id = " << id << "...\n";
        auto it = db.find(id);
        if (it != db.end()) return it->second;
        return "";
    }

    void update(int id, const std::string& name) {
        std::cout << "  --> [DATABASE DISK WRITE] Executing UPDATE users SET name = '" 
                  << name << "' WHERE id = " << id << "...\n";
        db[id] = name;
    }
};

// Simulated Cache (Fast in-memory Redis cluster)
class RedisCache {
private:
    std::unordered_map<int, std::string> cache;
public:
    bool get(int id, std::string& outVal) {
        auto it = cache.find(id);
        if (it != cache.end()) {
            outVal = it->second;
            return true;
        }
        return false;
    }

    void set(int id, const std::string& val) {
        cache[id] = val;
    }

    void del(int id) {
        cache.erase(id);
    }

    void printState() const {
        std::cout << "  Current Redis Cache Keys: [ ";
        for (const auto& [k, v] : cache) {
            std::cout << "{" << k << ":" << v << "} ";
        }
        std::cout << "]\n\n";
    }
};

// Application Service implementing Cache-Aside
class UserService {
private:
    RedisCache redis;
    Database db;

public:
    std::string getUserProfile(int userId) {
        std::cout << "\n[REQUEST] User requests profile for ID: " << userId << "\n";
        std::string profile;

        // Step 1: Check Cache (Redis)
        if (redis.get(userId, profile)) {
            if (profile == "__NULL__") {
                std::cout << "[CACHE PENETRATION DEFENSE] Known non-existent ID. Rejected via Cache (~1ms, DB skipped)!\n";
                return "USER_NOT_FOUND";
            }
            std::cout << "[CACHE HIT] Found in Redis: '" << profile << "' (~1ms, DB skipped)\n";
            return profile;
        }

        // Step 2: Cache Miss -> Query Database
        std::cout << "[CACHE MISS] Key not found in Redis. Fetching from Database...\n";
        profile = db.query(userId);

        if (!profile.empty()) {
            // Step 3: Write back to Cache for future requests
            std::cout << "[CACHE WRITE] Stored in Redis with TTL for future requests.\n";
            redis.set(userId, profile);
        } else {
            // Protect against Cache Penetration by caching sentinel "__NULL__"
            std::cout << "[CACHE PENETRATION PREVENTION] ID " << userId 
                      << " not in DB. Caching NULL sentinel with short TTL.\n";
            redis.set(userId, "__NULL__");
            profile = "USER_NOT_FOUND";
        }

        return profile;
    }

    void updateUserProfile(int userId, const std::string& newName) {
        std::cout << "\n[WRITE] Updating user " << userId << " to '" << newName << "'...\n";
        
        // 1. Write to database first
        db.update(userId, newName);

        // 2. Invalidate (delete) cache entry to avoid stale reads
        std::cout << "[CACHE INVALIDATE] Purging stale key " << userId << " from Redis\n";
        redis.del(userId);
    }

    void printCacheState() const {
        redis.printState();
    }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "  Cache-Aside Pattern & Penetration Defense      \n";
    std::cout << "=================================================\n";

    UserService userService;

    std::cout << "--- 1. Cold Start Read (Cache Miss -> DB Fetch -> Cache Store) ---\n";
    userService.getUserProfile(1);
    userService.printCacheState();

    std::cout << "--- 2. Warm Cache Read (Instant Cache Hit) ---\n";
    userService.getUserProfile(1);
    userService.printCacheState();

    std::cout << "--- 3. Database Write & Cache Invalidation ---\n";
    userService.updateUserProfile(1, "Alice Senior Principal Engineer");
    userService.printCacheState();

    std::cout << "--- 4. Fresh Read After Invalidation (Reloads from DB) ---\n";
    userService.getUserProfile(1);
    userService.printCacheState();

    std::cout << "--- 5. Malicious Probe on Non-Existent ID (Cache Penetration Defense) ---\n";
    // First query misses in cache and misses in DB -> caches "__NULL__" sentinel
    userService.getUserProfile(9999);
    userService.printCacheState();

    // Second query hits "__NULL__" sentinel -> DB completely protected!
    userService.getUserProfile(9999);
    userService.printCacheState();

    return 0;
}
