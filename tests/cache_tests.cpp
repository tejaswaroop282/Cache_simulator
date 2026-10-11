#include <iostream>
#include <cassert>

#include "CachePolicy.h"
#include "FIFOCache.h"
#include "LRUCache.h"
#include "LFUCache.h"

using namespace std;

// ============================================================================
// Demonstration of Runtime Polymorphism:
// A function taking a reference or pointer to the base class (CachePolicy)
// works uniformly with ANY derived cache (FIFO, LRU, LFU).
// ============================================================================
void testPolymorphism(CachePolicy& cache) {
    cache.clear();
    cache.put(10, 100);
    cache.put(20, 200);
    assert(cache.get(10) == 100);
    assert(cache.get(20) == 200);
    assert(cache.get(99) == -1);
}

int main() {
    cout << "=========================================================\n";
    cout << "           Running Cache OOP Unit Test Suite\n";
    cout << "=========================================================\n";

    // 1. FIFO Tests
    cout << "  [TEST] FIFO Eviction & Updates...";
    {
        FIFOCache f(2);
        f.put(1, 10);
        f.put(2, 20);
        assert(f.get(1) == 10); // get(1) does not change FIFO arrival order
        f.put(3, 30);           // evicts 1 (the oldest inserted)
        assert(f.get(1) == -1);
        assert(f.get(2) == 20);
        assert(f.get(3) == 30);

        f.put(2, 200);          // update key 2
        assert(f.get(2) == 200);
        f.put(4, 40);           // evicts 2
        assert(f.get(2) == -1);
        assert(f.get(3) == 30);
        assert(f.get(4) == 40);
    }
    cout << " PASSED\n";

    // 2. LRU Tests (LeetCode #146)
    cout << "  [TEST] LRU Recency & Splicing...";
    {
        LRUCache lru(2);
        lru.put(1, 1);
        lru.put(2, 2);
        assert(lru.get(1) == 1); // 1 is now MRU, 2 is LRU
        lru.put(3, 3);           // evicts 2
        assert(lru.get(2) == -1);
        lru.put(4, 4);           // evicts 1 (since 3 was accessed more recently)
        assert(lru.get(1) == -1);
        assert(lru.get(3) == 3);
        assert(lru.get(4) == 4);

        LRUCache lru2(2);
        lru2.put(1, 10);
        lru2.put(2, 20);
        lru2.put(1, 100);        // updates 1 and promotes it to MRU
        lru2.put(3, 30);         // evicts 2
        assert(lru2.get(2) == -1);
        assert(lru2.get(1) == 100);
        assert(lru2.get(3) == 30);
    }
    cout << " PASSED\n";

    // 3. LFU Tests (LeetCode #460)
    cout << "  [TEST] LFU Frequency & LRU Tie-Breaking...";
    {
        LFUCache lfu(2);
        lfu.put(1, 1);
        lfu.put(2, 2);
        assert(lfu.get(1) == 1); // freq(1)=2, freq(2)=1
        lfu.put(3, 3);           // evicts key 2 (lowest frequency = 1)
        assert(lfu.get(2) == -1);
        assert(lfu.get(3) == 3); // freq(3)=2, freq(1)=2
        lfu.put(4, 4);           // both 1 and 3 have freq=2; key 1 is LRU, so evict 1
        assert(lfu.get(1) == -1);
        assert(lfu.get(3) == 3);
        assert(lfu.get(4) == 4);
    }
    cout << " PASSED\n";

    // 4. Edge Cases
    cout << "  [TEST] Edge Cases (Capacity 0 & Capacity 1)...";
    {
        // Capacity 0: should not store anything
        FIFOCache f0(0); f0.put(1, 1); assert(f0.get(1) == -1);
        LRUCache l0(0);  l0.put(1, 1); assert(l0.get(1) == -1);
        LFUCache u0(0);  u0.put(1, 1); assert(u0.get(1) == -1);

        // Capacity 1: evicts immediately on next put
        FIFOCache f1(1);
        f1.put(1, 10); assert(f1.get(1) == 10);
        f1.put(2, 20); assert(f1.get(1) == -1); assert(f1.get(2) == 20);

        LRUCache l1(1);
        l1.put(1, 10); assert(l1.get(1) == 10);
        l1.put(2, 20); assert(l1.get(1) == -1); assert(l1.get(2) == 20);

        LFUCache u1(1);
        u1.put(1, 10); assert(u1.get(1) == 10);
        u1.put(2, 20); assert(u1.get(1) == -1); assert(u1.get(2) == 20);
    }
    cout << " PASSED\n";

    // 5. Runtime Polymorphism Test
    cout << "  [TEST] Runtime Polymorphism via CachePolicy* & CachePolicy&...";
    {
        FIFOCache fifo(5);
        LRUCache  lru(5);
        LFUCache  lfu(5);

        // Pass each derived class to a function expecting the base class reference
        testPolymorphism(fifo);
        testPolymorphism(lru);
        testPolymorphism(lfu);

        // Test with base class pointer
        CachePolicy* basePtr = &lru;
        basePtr->put(42, 999);
        assert(basePtr->get(42) == 999);
    }
    cout << " PASSED\n";

    cout << "---------------------------------------------------------\n";
    cout << ">> ALL UNIT TESTS PASSED! (100% Correctness)\n";
    cout << "=========================================================\n";
    return 0;
}
