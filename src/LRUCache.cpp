#include "LRUCache.h"

// Constructor: initializes capacity
LRUCache::LRUCache(int cap) : capacity(cap) {}

// get: Returns value of key if present, otherwise returns -1
// On hit, moves the accessed item to the front of dll (mark as MRU).
int LRUCache::get(int key) {
    if (mp.find(key) == mp.end()) {
        return -1;
    }

    // Move the node from its current position to the front of dll in O(1)
    auto it = mp[key];
    dll.splice(dll.begin(), dll, it);
    return it->second;
}

// put: Inserts or updates key-value pair
// Evicts least recently used item (back of dll) when full.
void LRUCache::put(int key, int value) {
    if (capacity <= 0) return;

    // If key already exists, update value and move to front
    if (mp.find(key) != mp.end()) {
        auto it = mp[key];
        it->second = value;
        dll.splice(dll.begin(), dll, it);
        return;
    }

    // If at capacity, evict the least recently used element (back of list)
    if ((int)dll.size() >= capacity) {
        int lruKey = dll.back().first;
        dll.pop_back();
        mp.erase(lruKey);
    }

    // Insert new node at the front (MRU)
    dll.push_front({key, value});
    mp[key] = dll.begin();
}

// clear: Empties both the list and the map
void LRUCache::clear() {
    dll.clear();
    mp.clear();
}
