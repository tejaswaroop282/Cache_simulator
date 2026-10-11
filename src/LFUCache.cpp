#include "LFUCache.h"

// Constructor: initializes capacity and minFreq
LFUCache::LFUCache(int cap) : capacity(cap), minFreq(0) {}

// updateFrequency: private helper method to increment frequency of a key
void LFUCache::updateFrequency(int key) {
    int currFreq = keyTable[key].second;

    // Erase key from its current frequency list
    freqTable[currFreq].erase(iterTable[key]);
    if (freqTable[currFreq].empty()) {
        freqTable.erase(currFreq);
        // If this was the minimum frequency list and it's now empty, increase minFreq
        if (minFreq == currFreq) {
            minFreq++;
        }
    }

    // Add key to the front of the new frequency list (currFreq + 1)
    int newFreq = currFreq + 1;
    keyTable[key].second = newFreq;
    freqTable[newFreq].push_front(key);
    iterTable[key] = freqTable[newFreq].begin();
}

// get: Returns value of key if present, otherwise returns -1
// On hit, updates the frequency of the key.
int LFUCache::get(int key) {
    if (keyTable.find(key) == keyTable.end()) {
        return -1;
    }

    updateFrequency(key);
    return keyTable[key].first;
}

// put: Inserts or updates key-value pair
// Evicts least frequently used item (LRU tie-breaker) when full.
void LFUCache::put(int key, int value) {
    if (capacity <= 0) return;

    // If key already exists, update value and bump frequency
    if (keyTable.find(key) != keyTable.end()) {
        keyTable[key].first = value;
        updateFrequency(key);
        return;
    }

    // If at capacity, evict the least frequently used key
    if ((int)keyTable.size() >= capacity) {
        int evictKey = freqTable[minFreq].back();
        freqTable[minFreq].pop_back();
        if (freqTable[minFreq].empty()) {
            freqTable.erase(minFreq);
        }
        keyTable.erase(evictKey);
        iterTable.erase(evictKey);
    }

    // Insert new key with frequency = 1
    minFreq = 1;
    keyTable[key] = {value, 1};
    freqTable[1].push_front(key);
    iterTable[key] = freqTable[1].begin();
}

// clear: Empties all tables and resets minFreq
void LFUCache::clear() {
    keyTable.clear();
    freqTable.clear();
    iterTable.clear();
    minFreq = 0;
}
