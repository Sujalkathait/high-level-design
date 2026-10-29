/**
 * @file fifo_cache.cpp
 * @brief First-In First-Out (FIFO) Cache in C++17
 * 
 * Features & Bug Fixes:
 * - Chronological insertion eviction
 * - Non-positive capacity guards
 * - Clean order preservation on reads
 */

#include <iostream>
#include <queue>
#include <unordered_map>

class FIFOCache {
private:
    int capacity;
    std::queue<int> orderQueue;
    std::unordered_map<int, int> cache;

public:
    explicit FIFOCache(int cap) : capacity(cap) {
        if (capacity < 0) capacity = 0;
    }

    int get(int key) {
        if (capacity <= 0) return -1;

        auto it = cache.find(key);
        if (it == cache.end()) {
            std::cout << "[GET " << key << "] -> MISS (-1)\n";
            return -1;
        }
        std::cout << "[GET " << key << "] -> HIT (" << it->second 
                  << ") - Notice: FIFO order does not change on access!\n";
        return it->second;
    }

    void put(int key, int value) {
        // Bug Fix: Zero/negative capacity guard
        if (capacity <= 0) {
            std::cout << "[PUT " << key << "] -> Ignored: Capacity is 0.\n";
            return;
        }

        if (cache.find(key) != cache.end()) {
            cache[key] = value;
            std::cout << "[PUT " << key << ", " << value << "] -> Updated existing value\n";
            return;
        }

        if (static_cast<int>(cache.size()) >= capacity) {
            int oldestKey = orderQueue.front();
            orderQueue.pop();
            cache.erase(oldestKey);
            std::cout << "[EVICT] FIFO Evicted Oldest Key: " << oldestKey << "\n";
        }

        orderQueue.push(key);
        cache[key] = value;
        std::cout << "[PUT " << key << ", " << value << "] -> Inserted into FIFO queue\n";
    }

    size_t size() const { return cache.size(); }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "       FIFO Cache Simulation (Capacity = 3)      \n";
    std::cout << "=================================================\n\n";

    FIFOCache fifo(3);

    fifo.put(1, 100);
    fifo.put(2, 200);
    fifo.put(3, 300);

    // Reading key 1 does NOT save it from being evicted next!
    fifo.get(1);

    // Capacity is 3; adding key 4 must evict key 1 (the oldest inserted)
    fifo.put(4, 400);

    fifo.get(1); // Expect MISS (-1)
    fifo.get(2); // Expect HIT (200)

    return 0;
}

