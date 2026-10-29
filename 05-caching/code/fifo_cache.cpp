/**
 * @file fifo_cache.cpp
 * @brief First-In First-Out (FIFO) Cache in C++17
 * 
 * Approach:
 * - std::queue<int>: Tracks chronological arrival order (Front = Oldest, Back = Newest)
 * - std::unordered_map<int, int>: Key -> Value mapping for O(1) lookups
 * - Reads (get) do NOT change queue eviction order
 */

#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

class FIFOCache {
private:
    int capacity;
    std::queue<int> orderQueue;           // Tracks arrival order (Front = Oldest)
    std::unordered_map<int, int> cacheMap; // Stores {key, value} for O(1) lookups

public:
    explicit FIFOCache(int cap) : capacity(cap < 0 ? 0 : cap) {}

    int get(int key) {
        if (capacity <= 0) return -1;

        auto it = cacheMap.find(key);
        if (it == cacheMap.end()) {
            std::cout << "[GET " << key << "] -> Cache MISS (-1)\n";
            return -1;
        }

        // In FIFO, accessing a key does NOT change its position in the queue!
        std::cout << "[GET " << key << "] -> Cache HIT (Value: " << it->second 
                  << ") [FIFO order unchanged]\n";
        return it->second;
    }

    void put(int key, int value) {
        if (capacity <= 0) {
            std::cout << "[PUT " << key << ", " << value << "] -> Ignored: Capacity is 0.\n";
            return;
        }

        // Case 1: Key already exists -> Update value in-place without altering FIFO order
        if (cacheMap.find(key) != cacheMap.end()) {
            cacheMap[key] = value;
            std::cout << "[PUT " << key << ", " << value << "] -> Updated existing key (Order preserved)\n";
            return;
        }

        // Case 2: Cache is full -> Evict the oldest key from front of queue
        if (static_cast<int>(cacheMap.size()) >= capacity) {
            int oldestKey = orderQueue.front();
            orderQueue.pop();
            cacheMap.erase(oldestKey);
            std::cout << "[EVICT] Cache full! Evicted Oldest Key: " << oldestKey << " from Front\n";
        }

        // Case 3: Insert new key at the back of the queue
        orderQueue.push(key);
        cacheMap[key] = value;
        std::cout << "[PUT " << key << ", " << value << "] -> Inserted at Back (Newest)\n";
    }

    void printState() const {
        std::cout << "  Cache Elements: [ ";
        for (const auto& [k, v] : cacheMap) {
            std::cout << "{" << k << ":" << v << "} ";
        }
        std::cout << "]\n\n";
    }

    size_t size() const { return cacheMap.size(); }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "       FIFO Cache Simulation (Capacity = 3)      \n";
    std::cout << "=================================================\n\n";

    FIFOCache fifo(3);

    std::cout << "--- Step 1: put(1, 100), put(2, 200), put(3, 300) ---\n";
    fifo.put(1, 100);
    fifo.put(2, 200);
    fifo.put(3, 300);
    fifo.printState();

    std::cout << "--- Step 2: get(1) [Accessing key 1 does NOT save it from eviction] ---\n";
    fifo.get(1);
    fifo.printState();

    std::cout << "--- Step 3: put(4, 400) [Evicts Key 1 because it arrived first] ---\n";
    fifo.put(4, 400);
    fifo.printState();

    std::cout << "--- Step 4: Verify Key 1 is evicted, Key 2 remains ---\n";
    fifo.get(1); // Expect MISS (-1)
    fifo.get(2); // Expect HIT (200)

    return 0;
}
