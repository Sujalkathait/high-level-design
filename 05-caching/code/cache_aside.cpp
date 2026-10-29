/**
 * @file cache_aside.cpp
 * @brief Cache-Aside Pattern with Simulated Redis, PostgreSQL, and Penetration Defense in C++17
 * 
 * Features & Bug Fixes:
 * - Complete Read-Through (Cache Check -> DB Fallback -> Cache Write)
 * - Complete Write-Through/Invalidate (DB Write -> Cache Invalidation)
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
        if (db.find(id) != db.end()) return db[id];
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
            // Bug Fix / Pattern: Protect against Cache Penetration by caching sentinel "__NULL__"
            std::cout << "[CACHE PENETRATION PREVENTION] ID " << userId 
                      << " not in DB. Caching NULL sentinel with short TTL.\n";
            redis.set(userId, "__NULL__");
            profile = "USER_NOT_FOUND";
        }

        return profile;
    }

    void updateUserProfile(int userId, const std::string& newProfile) {
        std::cout << "\n[UPDATE] User updates profile for ID: " << userId << "\n";
        // Step 1: Write directly to Database
        db.update(userId, newProfile);

        // Step 2: Invalidate Cache key to avoid serving stale data
        std::cout << "[CACHE INVALIDATE] Evicted stale key from Redis (DEL user:" << userId << ")\n";
        redis.del(userId);
    }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "         Cache-Aside Pattern Simulation          \n";
    std::cout << "=================================================\n";

    UserService service;

    // 1st request: Cache Miss -> Reads DB -> Populates Redis
    service.getUserProfile(1);

    // 2nd request: Cache Hit -> Reads directly from Redis (DB untouched)
    service.getUserProfile(1);

    // Update user profile -> Writes DB -> Invalidates Redis
    service.updateUserProfile(1, "Alice (Senior Staff Architect)");

    // 3rd request after update: Cache Miss -> Reads fresh data from DB
    service.getUserProfile(1);

    std::cout << "\n--- Testing Cache Penetration Defense ---\n";
    // Non-existent user 9999: 1st time queries DB and caches sentinel
    service.getUserProfile(9999);
    // 2nd time: Blocked at Redis layer without touching DB!
    service.getUserProfile(9999);

    return 0;
}

