#include "FIFOCache.h"

// Constructor: initializes the capacity data member
FIFOCache::FIFOCache(int cap) : capacity(cap) {}

// get: Returns value of key if present, otherwise returns -1
// In FIFO, lookup does NOT alter eviction order.
int FIFOCache::get(int key) {
    if (cache.find(key) == cache.end()) {
        return -1;
    }
    return cache[key];
}

// put: Inserts a new key-value pair or updates existing key
// Evicts oldest key (front of queue) when full.
void FIFOCache::put(int key, int value) {
    if (capacity <= 0) return;

    // If key already exists, just update its value
    if (cache.find(key) != cache.end()) {
        cache[key] = value;
        return;
    }

    // If at capacity, evict the oldest key
    if ((int)cache.size() >= capacity) {
        int oldest = q.front();
        q.pop();
        cache.erase(oldest);
    }

    cache[key] = value;
    q.push(key);
}

// clear: Empties both the hash map and the queue
void FIFOCache::clear() {
    cache.clear();
    while (!q.empty()) {
        q.pop();
    }
}
