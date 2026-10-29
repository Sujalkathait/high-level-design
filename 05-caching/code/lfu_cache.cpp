/**
 * @file lfu_cache.cpp
 * @brief Least Frequently Used (LFU) Cache in C++17 (O(1) Get and Put)
 * 
 * Approach:
 * - keyToValue: unordered_map<int, int> (Key -> Value)
 * - keyToFrequency: unordered_map<int, int> (Key -> Frequency)
 * - frequencyToKeys: unordered_map<int, list<int>> (Frequency -> Doubly Linked List of keys)
 *   * Front = Least Recently Used (LRU tie-breaker)
 *   * Back  = Most Recently Used (MRU)
 * - keyToIterator: unordered_map<int, list<int>::iterator> (Key -> Iterator in its frequency list for O(1) removal)
 * - minimumFrequency: int (Tracks lowest frequency present in cache)
 */

#include <iostream>
#include <list>
#include <unordered_map>
#include <iterator>

class LFUCache {
private:
    int capacity;
    int minimumFrequency;

    // Stores each key's value
    std::unordered_map<int, int> keyToValue;

    // Stores each key's usage frequency
    std::unordered_map<int, int> keyToFrequency;

    // Stores keys grouped by frequency:
    // Front = Least Recently Used (LRU)
    // Back  = Most Recently Used (MRU)
    std::unordered_map<int, std::list<int>> frequencyToKeys;

    // Stores each key's position in its frequency list for O(1) removal
    std::unordered_map<int, std::list<int>::iterator> keyToIterator;

    // Increases a key's frequency and updates its position in frequency lists
    void increaseFrequency(int key) {
        int oldFrequency = keyToFrequency[key];

        // 1. Remove the key from its current frequency list in O(1)
        frequencyToKeys[oldFrequency].erase(keyToIterator[key]);

        // 2. If the old frequency list became empty, clean up the bucket
        if (frequencyToKeys[oldFrequency].empty()) {
            frequencyToKeys.erase(oldFrequency);

            // If the minimum frequency bucket is now empty, increment it
            if (minimumFrequency == oldFrequency) {
                minimumFrequency++;
            }
        }

        // 3. Increment the key's frequency
        int newFrequency = oldFrequency + 1;
        keyToFrequency[key] = newFrequency;

        // 4. Append key to the back of its new frequency list (MRU within bucket)
        frequencyToKeys[newFrequency].push_back(key);

        // 5. Store the new iterator position
        keyToIterator[key] = std::prev(frequencyToKeys[newFrequency].end());
    }

public:
    // Initialize cache with maximum capacity
    explicit LFUCache(int cap) : capacity(cap < 0 ? 0 : cap), minimumFrequency(0) {}

    int get(int key) {
        // Return -1 if key does not exist
        if (keyToValue.find(key) == keyToValue.end()) {
            std::cout << "[GET " << key << "] -> Cache MISS (-1)\n";
            return -1;
        }

        // Increase its frequency and update recency position
        increaseFrequency(key);

        int value = keyToValue[key];
        std::cout << "[GET " << key << "] -> Cache HIT (Value: " << value 
                  << ", new frequency: " << keyToFrequency[key] << ")\n";
        return value;
    }

    void put(int key, int value) {
        // Guard against zero-capacity cache
        if (capacity == 0) {
            std::cout << "[PUT " << key << ", " << value << "] -> Ignored: Capacity is 0.\n";
            return;
        }

        // Case 1: Key already exists
        if (keyToValue.find(key) != keyToValue.end()) {
            keyToValue[key] = value;
            increaseFrequency(key);
            std::cout << "[PUT " << key << ", " << value 
                      << "] -> Updated existing key (new freq: " << keyToFrequency[key] << ")\n";
            return;
        }

        // Case 2: Cache is full -> Evict least frequently used key
        if (static_cast<int>(keyToValue.size()) == capacity) {
            // Front of minimumFrequency list is the LRU key among the least frequent
            int leastUsedKey = frequencyToKeys[minimumFrequency].front();

            // Remove from the frequency list
            frequencyToKeys[minimumFrequency].pop_front();

            if (frequencyToKeys[minimumFrequency].empty()) {
                frequencyToKeys.erase(minimumFrequency);
            }

            // Remove from all tracking maps
            int evictedFreq = keyToFrequency[leastUsedKey];
            int evictedVal = keyToValue[leastUsedKey];
            keyToValue.erase(leastUsedKey);
            keyToFrequency.erase(leastUsedKey);
            keyToIterator.erase(leastUsedKey);

            std::cout << "[EVICT] Cache full! Evicted Key: " << leastUsedKey 
                      << " (Value: " << evictedVal << ", freq: " << evictedFreq << ")\n";
        }

        // Case 3: Insert new key with frequency 1
        keyToValue[key] = value;
        keyToFrequency[key] = 1;

        // Add to frequency 1 list at back (MRU within bucket)
        frequencyToKeys[1].push_back(key);

        // Store its iterator
        keyToIterator[key] = std::prev(frequencyToKeys[1].end());

        // A brand new key always resets minimumFrequency to 1
        minimumFrequency = 1;

        std::cout << "[PUT " << key << ", " << value << "] -> Inserted with frequency=1\n";
    }

    void printState() const {
        std::cout << "  Cache State (minFreq = " << minimumFrequency << "):\n";
        for (const auto& [freq, keys] : frequencyToKeys) {
            std::cout << "    Freq " << freq << " [LRU -> MRU]: ";
            for (int k : keys) {
                auto itVal = keyToValue.find(k);
                int v = (itVal != keyToValue.end()) ? itVal->second : -1;
                std::cout << "{" << k << ":" << v << "} ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    size_t size() const {
        return keyToValue.size();
    }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "       LFU Cache Simulation (Capacity = 2)       \n";
    std::cout << "=================================================\n\n";

    LFUCache lfu(2);

    std::cout << "--- Step 1: lfu.put(1, 1) ---\n";
    lfu.put(1, 1);
    lfu.printState();

    std::cout << "--- Step 2: lfu.put(2, 2) ---\n";
    lfu.put(2, 2);
    lfu.printState();

    std::cout << "--- Step 3: lfu.get(1) [Key 1 freq: 1 -> 2] ---\n";
    lfu.get(1);
    lfu.printState();

    std::cout << "--- Step 4: lfu.put(3, 3) [Cache full: Evicts Key 2 (freq 1)] ---\n";
    lfu.put(3, 3);
    lfu.printState();

    std::cout << "--- Step 5: lfu.get(2) [Key 2 was evicted] ---\n";
    lfu.get(2);
    lfu.printState();

    std::cout << "--- Step 6: lfu.get(3) [Key 3 freq: 1 -> 2] ---\n";
    lfu.get(3);
    lfu.printState();

    std::cout << "--- Step 7: lfu.put(4, 4) [Both 1 & 3 have freq 2; Key 1 is LRU, evicted!] ---\n";
    lfu.put(4, 4);
    lfu.printState();

    std::cout << "--- Step 8: lfu.get(1) [Key 1 was evicted] ---\n";
    lfu.get(1);
    lfu.printState();

    std::cout << "--- Step 9: lfu.get(3) [Key 3 freq: 2 -> 3] ---\n";
    lfu.get(3);
    lfu.printState();

    std::cout << "--- Step 10: lfu.get(4) [Key 4 freq: 1 -> 2] ---\n";
    lfu.get(4);
    lfu.printState();

    return 0;
}
