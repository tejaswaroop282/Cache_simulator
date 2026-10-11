#ifndef CACHE_POLICY_H
#define CACHE_POLICY_H

// ============================================================================
// CachePolicy: Abstract Base Class
// ============================================================================
// This is an abstract class (interface) that defines common operations
// for all cache eviction policies.
//
// Derived classes (FIFOCache, LRUCache, LFUCache) MUST implement
// the pure virtual functions get(), put(), and clear().
// ============================================================================

class CachePolicy {
public:
    // Virtual Destructor:
    // Ensures that when a derived object is deleted via a CachePolicy pointer,
    // the derived class's destructor is called properly.
    virtual ~CachePolicy() {}

    // Pure Virtual Functions (= 0):
    // Subclasses must provide their own implementation for these methods.
    virtual int get(int key) = 0;
    virtual void put(int key, int value) = 0;
    virtual void clear() = 0;
};

#endif // CACHE_POLICY_H
