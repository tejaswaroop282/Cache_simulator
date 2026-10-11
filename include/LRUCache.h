#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include "CachePolicy.h"
#include <list>
#include <unordered_map>
#include <utility>

// ============================================================================
// LRUCache: Least Recently Used Cache Policy (LeetCode #146)
// ============================================================================
// Inherits from CachePolicy using public inheritance.
// When capacity is full, the item not accessed for the longest time is evicted.
//
// Data Structures:
// - dll: Doubly-linked list of {key, value} pairs.
//        Front = Most Recently Used (MRU), Back = Least Recently Used (LRU)
// - mp:  Hash map storing key -> iterator to the corresponding node in dll.
// ============================================================================

class LRUCache : public CachePolicy {
private:
    int capacity;
    std::list<std::pair<int, int>> dll;
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> mp;

public:
    // Constructor
    LRUCache(int cap);

    // Member functions implementing the CachePolicy interface
    int get(int key) override;
    void put(int key, int value) override;
    void clear() override;
};

#endif // LRU_CACHE_H
