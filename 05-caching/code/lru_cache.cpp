/**
 * @file lru_cache.cpp
 * @brief Least Recently Used (LRU) Cache in C++17
 * 
 * Data structures:
 * - std::list<std::pair<int, int>>: Doubly linked list (Front = MRU, Back = LRU)
 * - std::unordered_map: Key -> Iterator to list node for O(1) lookups
 * 
 * Features & Bug Fixes:
 * - Non-positive capacity guard (if capacity <= 0, ignores put/get)
 * - Move to front on cache hit
 * - Eviction of exact least recently used item on full capacity
 */

#include <iostream>
#include <list>
#include <unordered_map>
#include <utility>

class LRUCache {
private:
    int capacity;
    std::list<std::pair<int, int>> dll; // {key, value}
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> cacheMap;

public:
    explicit LRUCache(int cap) : capacity(cap) {
        if (capacity < 0) capacity = 0;
    }

    int get(int key) {
        if (capacity <= 0) return -1;

        auto it = cacheMap.find(key);
        if (it == cacheMap.end()) {
            std::cout << "[GET " << key << "] -> Cache MISS (-1)\n";
            return -1;
        }

        // Move accessed node to front (MRU)
        int value = it->second->second;
        dll.erase(it->second);
        dll.push_front({key, value});
        cacheMap[key] = dll.begin();

        std::cout << "[GET " << key << "] -> Cache HIT (Value: " << value << ")\n";
        return value;
    }

    void put(int key, int value) {
        // Bug Fix: Guard against zero or negative capacity cache
        if (capacity <= 0) {
            std::cout << "[PUT " << key << "] -> Ignored: Cache capacity is 0.\n";
            return;
        }

        auto it = cacheMap.find(key);

        if (it != cacheMap.end()) {
            // Key already exists: update value and move to front (MRU)
            dll.erase(it->second);
            dll.push_front({key, value});
            cacheMap[key] = dll.begin();
            std::cout << "[PUT " << key << ", " << value << "] -> Updated existing key\n";
            return;
        }

        // Cache is full: evict LRU (back of list)
        if (static_cast<int>(dll.size()) >= capacity) {
            auto lruItem = dll.back();
            std::cout << "[EVICT] Cache full! Evicted LRU Key: " 
                      << lruItem.first << " (Value: " << lruItem.second << ")\n";
            cacheMap.erase(lruItem.first);
            dll.pop_back();
        }

        // Insert new node at front (MRU)
        dll.push_front({key, value});
        cacheMap[key] = dll.begin();
        std::cout << "[PUT " << key << ", " << value << "] -> Inserted as MRU\n";
    }

    void printState() const {
        std::cout << "  Cache State (MRU -> LRU): [ ";
        for (const auto& item : dll) {
            std::cout << "{" << item.first << ":" << item.second << "} ";
        }
        std::cout << "]\n\n";
    }

    size_t size() const { return dll.size(); }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "       LRU Cache Simulation (Capacity = 3)       \n";
    std::cout << "=================================================\n\n";

    LRUCache cache(3);

    std::cout << "--- Step 1: put(1, 10) ---\n";
    cache.put(1, 10);
    cache.printState();

    std::cout << "--- Step 2: put(2, 20) ---\n";
    cache.put(2, 20);
    cache.printState();

    std::cout << "--- Step 3: put(3, 30) ---\n";
    cache.put(3, 30);
    cache.printState();

    std::cout << "--- Step 4: get(1) [Access key 1, moves to MRU] ---\n";
    cache.get(1);
    cache.printState();

    std::cout << "--- Step 5: put(4, 40) [Evicts LRU key 2] ---\n";
    cache.put(4, 40);
    cache.printState();

    return 0;
}

