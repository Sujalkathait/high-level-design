/**
 * @file lfu_cache.cpp
 * @brief Least Frequently Used (LFU) Cache in C++17 (O(1) Get and Put)
 * 
 * Features & Bug Fixes:
 * - O(1) time complexity using frequency lists + minimum frequency tracking
 * - Safe handling of capacity <= 0
 * - Clean tie-breaking: If frequencies match, least recently used is evicted
 */

#include <iostream>
#include <unordered_map>
#include <list>

struct LFUNode {
    int key;
    int value;
    int frequency;
};

class LFUCache {
private:
    int capacity;
    int minFreq;
    std::unordered_map<int, std::list<LFUNode>::iterator> keyMap;
    std::unordered_map<int, std::list<LFUNode>> freqLists;

    void updateFrequency(std::list<LFUNode>::iterator nodeIt) {
        int key = nodeIt->key;
        int value = nodeIt->value;
        int oldFreq = nodeIt->frequency;
        int newFreq = oldFreq + 1;

        freqLists[oldFreq].erase(nodeIt);

        if (freqLists[oldFreq].empty()) {
            freqLists.erase(oldFreq);
            if (minFreq == oldFreq) {
                minFreq = newFreq;
            }
        }

        freqLists[newFreq].push_front({key, value, newFreq});
        keyMap[key] = freqLists[newFreq].begin();
    }

public:
    explicit LFUCache(int cap) : capacity(cap), minFreq(0) {
        if (capacity < 0) capacity = 0;
    }

    int get(int key) {
        if (capacity <= 0) return -1;

        auto it = keyMap.find(key);
        if (it == keyMap.end()) {
            std::cout << "[GET " << key << "] -> MISS (-1)\n";
            return -1;
        }

        int val = it->second->value;
        updateFrequency(it->second);
        std::cout << "[GET " << key << "] -> HIT (" << val 
                  << ", new frequency=" << keyMap[key]->frequency << ")\n";
        return val;
    }

    void put(int key, int value) {
        // Bug Fix: Zero/negative capacity check
        if (capacity <= 0) {
            std::cout << "[PUT " << key << "] -> Ignored: Capacity is 0.\n";
            return;
        }

        auto it = keyMap.find(key);
        if (it != keyMap.end()) {
            it->second->value = value;
            updateFrequency(it->second);
            std::cout << "[PUT " << key << ", " << value << "] -> Updated existing key\n";
            return;
        }

        if (static_cast<int>(keyMap.size()) >= capacity) {
            auto nodeToEvict = freqLists[minFreq].back();
            std::cout << "[EVICT] LFU Evicted Key " << nodeToEvict.key 
                      << " (freq=" << nodeToEvict.frequency << ")\n";
            keyMap.erase(nodeToEvict.key);
            freqLists[minFreq].pop_back();
            if (freqLists[minFreq].empty()) {
                freqLists.erase(minFreq);
            }
        }

        minFreq = 1;
        freqLists[1].push_front({key, value, 1});
        keyMap[key] = freqLists[1].begin();
        std::cout << "[PUT " << key << ", " << value << "] -> Inserted with frequency=1\n";
    }
};

int main() {
    std::cout << "=================================================\n";
    std::cout << "       LFU Cache Simulation (Capacity = 2)       \n";
    std::cout << "=================================================\n\n";

    LFUCache lfu(2);

    lfu.put(1, 10); // freq: 1(1)
    lfu.put(2, 20); // freq: 1(1), 2(1)
    lfu.get(1);     // Key 1 freq -> 2

    // Now key 2 has freq 1, key 1 has freq 2.
    // Adding key 3 will evict key 2 because its frequency is lowest!
    lfu.put(3, 30); // Evicts key 2

    lfu.get(2); // Miss (-1)
    lfu.get(3); // Hit (30)

    return 0;
}

