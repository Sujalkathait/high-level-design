/**
 * @file lru_cache.cpp
 * @brief Least Recently Used (LRU) Cache in C++17
 * 
 * Approach:
 * - std::list<std::pair<int, int>>: Doubly linked list
 *   * Front = Least Recently Used (LRU) - Evicted on full capacity
 *   * Back  = Most Recently Used (MRU)  - Appended/moved on access
 * - std::unordered_map: Key -> Iterator to list node for O(1) lookups
 * - std::list::splice: Moves nodes in O(1) without reallocating or invalidating iterators
 */

#include <iostream>
#include <list>
#include <unordered_map>
#include <utility>
#include <iterator>

class LRUCache {
private:
    int capacity;

    // Stores {key, value} pairs
    // Front = Least Recently Used (LRU)
    // Back  = Most Recently Used (MRU)
    std::list<std::pair<int, int>> cacheList;

    // Maps each key to its iterator position in the list
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> cacheMap;

public:
    // Initialize cache capacity
    explicit LRUCache(int cap) : capacity(cap < 0 ? 0 : cap) {}

    int get(int key) {
        // Return -1 if key does not exist
        auto it = cacheMap.find(key);
        if (it == cacheMap.end()) {
            std::cout << "[GET " << key << "] -> Cache MISS (-1)\n";
            return -1;
        }

        // Get the node iterator of the requested key
        auto node = it->second;

        // Extract its value
        int value = node->second;

        // Move the node to the back (most recently used) in O(1)
        cacheList.splice(cacheList.end(), cacheList, node);

        std::cout << "[GET " << key << "] -> Cache HIT (Value: " << value 
                  << ") [Moved to MRU / Back]\n";
        return value;
    }

    void put(int key, int value) {
        // Handle zero or negative capacity cache
        if (capacity <= 0) {
            std::cout << "[PUT " << key << ", " << value << "] -> Ignored: Cache capacity is 0.\n";
            return;
        }

        // Case 1: Key already exists
        auto it = cacheMap.find(key);
        if (it != cacheMap.end()) {
            auto node = it->second;

            // Update its value
            node->second = value;

            // Move the updated node to the back (MRU)
            cacheList.splice(cacheList.end(), cacheList, node);
            std::cout << "[PUT " << key << ", " << value << "] -> Updated existing key [Moved to MRU / Back]\n";
            return;
        }

        // Case 2: Key does not exist
        // If cache is full, evict the least recently used key from the front
        if (static_cast<int>(cacheList.size()) >= capacity) {
            // Get the key at the front (LRU)
            int leastUsedKey = cacheList.front().first;
            int leastUsedVal = cacheList.front().second;

            // Remove the key from the map
            cacheMap.erase(leastUsedKey);

            // Remove the node from the list
            cacheList.pop_front();

            std::cout << "[EVICT] Cache full! Evicted LRU Key: " << leastUsedKey 
                      << " (Value: " << leastUsedVal << ") from Front\n";
        }

        // Insert the new key-value pair at the back (MRU)
        cacheList.push_back({key, value});

        // Store the new node's iterator in the map
        cacheMap[key] = std::prev(cacheList.end());

        std::cout << "[PUT " << key << ", " << value << "] -> Inserted at Back (MRU)\n";
    }

    void printState() const {
        std::cout << "  Cache State (Front = LRU <---> Back = MRU): [ ";
        for (const auto& item : cacheList) {
            std::cout << "{" << item.first << ":" << item.second << "} ";
        }
        std::cout << "]\n\n";
    }

    size_t size() const {
        return cacheList.size();
    }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "       LRU Cache Simulation (Capacity = 2)       \n";
    std::cout << "    Front = LRU (Least Recently Used)            \n";
    std::cout << "    Back  = MRU (Most Recently Used)             \n";
    std::cout << "=================================================\n\n";

    LRUCache cache(2);

    std::cout << "--- Step 1: cache.put(1, 10) ---\n";
    cache.put(1, 10);
    cache.printState();

    std::cout << "--- Step 2: cache.get(1) ---\n";
    cache.get(1);
    cache.printState();

    std::cout << "--- Step 3: cache.put(2, 20) ---\n";
    cache.put(2, 20);
    cache.printState();

    std::cout << "--- Step 4: cache.put(3, 30) [Evicts LRU key 1] ---\n";
    cache.put(3, 30);
    cache.printState();

    std::cout << "--- Step 5: cache.get(2) [Moves key 2 to MRU / Back] ---\n";
    cache.get(2);
    cache.printState();

    std::cout << "--- Step 6: cache.get(1) [Key 1 was evicted] ---\n";
    cache.get(1);
    cache.printState();

    std::cout << "=================================================\n";
    std::cout << "       LRU Cache Simulation (Capacity = 3)       \n";
    std::cout << "=================================================\n\n";

    LRUCache cache3(3);

    std::cout << "--- Step 1: put(1, 100), put(2, 200), put(3, 300) ---\n";
    cache3.put(1, 100);
    cache3.put(2, 200);
    cache3.put(3, 300);
    cache3.printState();

    std::cout << "--- Step 2: get(1) [Key 1 moves from Front to Back] ---\n";
    cache3.get(1);
    cache3.printState();

    std::cout << "--- Step 3: put(4, 400) [Cache full: Evicts Key 2 from Front] ---\n";
    cache3.put(4, 400);
    cache3.printState();

    return 0;
}
