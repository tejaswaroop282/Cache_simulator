#ifndef LFU_CACHE_H
#define LFU_CACHE_H

#include "CachePolicy.h"
#include <list>
#include <unordered_map>
#include <utility>

// ============================================================================
// LFUCache: Least Frequently Used Cache Policy (LeetCode #460)
// ============================================================================
// Inherits from CachePolicy using public inheritance.
// When capacity is full, the item with the lowest access frequency is evicted.
// If multiple items have the same lowest frequency, the LRU item among them
// is evicted.
//
// Data Structures:
// - keyTable:  Maps key -> {value, frequency}
// - freqTable: Maps frequency -> list of keys with that frequency (MRU at front)
// - iterTable: Maps key -> iterator inside freqTable[frequency]
// - minFreq:   Current minimum frequency in the cache
// ============================================================================

class LFUCache : public CachePolicy {
private:
    int capacity;
    int minFreq;
    std::unordered_map<int, std::pair<int, int>> keyTable;
    std::unordered_map<int, std::list<int>> freqTable;
    std::unordered_map<int, std::list<int>::iterator> iterTable;

    // Helper method to update frequency when a key is accessed or modified
    void updateFrequency(int key);

public:
    // Constructor
    LFUCache(int cap);

    // Member functions implementing the CachePolicy interface
    int get(int key) override;
    void put(int key, int value) override;
    void clear() override;
};

#endif // LFU_CACHE_H
