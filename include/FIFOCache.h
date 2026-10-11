#ifndef FIFO_CACHE_H
#define FIFO_CACHE_H

#include "CachePolicy.h"
#include <unordered_map>
#include <queue>

// ============================================================================
// FIFOCache: First-In, First-Out Cache Policy
// ============================================================================
// Inherits from CachePolicy using public inheritance.
// When capacity is full, the item inserted earliest is evicted first.
//
// Data Structures:
// - cache: std::unordered_map<int, int> for O(1) key -> value lookup
// - q:     std::queue<int> to track the insertion order of keys
// ============================================================================

class FIFOCache : public CachePolicy {
private:
    int capacity;
    std::unordered_map<int, int> cache;
    std::queue<int> q;

public:
    // Constructor
    FIFOCache(int cap);

    // Member functions implementing the CachePolicy interface
    int get(int key) override;
    void put(int key, int value) override;
    void clear() override;
};

#endif // FIFO_CACHE_H
