#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <numeric>
#include <algorithm>
#include <string>

#include "CachePolicy.h"
#include "FIFOCache.h"
#include "LRUCache.h"
#include "LFUCache.h"

using namespace std;

// ============================================================================
// Workload Types for Cache Simulation
// ============================================================================
enum class WorkloadType {
    UNIFORM,
    HOT_COLD_80_20,
    HOT_COLD_90_10
};

// Generates a sequence of cache access keys
vector<int> generateWorkload(WorkloadType type, int totalAccesses, int keySpace, unsigned int seed) {
    mt19937 rng(seed);
    vector<int> workload;
    workload.reserve(totalAccesses);

    if (type == WorkloadType::UNIFORM) {
        uniform_int_distribution<int> dist(0, keySpace - 1);
        for (int i = 0; i < totalAccesses; ++i) {
            workload.push_back(dist(rng));
        }
    } else {
        double hotKeyRatio  = (type == WorkloadType::HOT_COLD_90_10) ? 0.10 : 0.20;
        double hotAccessPct = (type == WorkloadType::HOT_COLD_90_10) ? 0.90 : 0.80;

        int hotSetSize = max(1, (int)(keySpace * hotKeyRatio));
        uniform_int_distribution<int> hotDist(0, hotSetSize - 1);
        uniform_int_distribution<int> coldDist(hotSetSize, keySpace - 1);
        uniform_real_distribution<double> prob(0.0, 1.0);

        for (int i = 0; i < totalAccesses; ++i) {
            if (prob(rng) < hotAccessPct) {
                workload.push_back(hotDist(rng));
            } else {
                workload.push_back(coldDist(rng));
            }
        }
    }

    return workload;
}

// ============================================================================
// Benchmark Statistics
// ============================================================================
struct PolicyStats {
    string name;
    vector<double> hitRates;
    vector<double> missRates;

    double avgHitRate() const {
        return accumulate(hitRates.begin(), hitRates.end(), 0.0) / hitRates.size();
    }
    double minHitRate() const {
        return *min_element(hitRates.begin(), hitRates.end());
    }
    double maxHitRate() const {
        return *max_element(hitRates.begin(), hitRates.end());
    }
    double avgMissRate() const {
        return accumulate(missRates.begin(), missRates.end(), 0.0) / missRates.size();
    }
};

// Evaluates a cache policy on a workload using a base class pointer
double evaluatePolicy(CachePolicy* cache, const vector<int>& workload) {
    cache->clear();
    int hits = 0;

    for (int key : workload) {
        // Polymorphic call: calls the correct get() method for FIFO, LRU, or LFU
        if (cache->get(key) != -1) {
            hits++;
        } else {
            // Polymorphic call: calls the correct put() method
            cache->put(key, key);
        }
    }

    return (double)hits / workload.size() * 100.0;
}

// Runs simulation on all policies across multiple random seeds
void runWorkloadBenchmark(const string& title, WorkloadType type, int accesses, int keySpace, int capacity, const vector<unsigned int>& seeds) {
    cout << "=======================================================================================\n";
    cout << "  BENCHMARK: " << title << "\n";
    cout << "  Accesses: " << accesses << " | Key Space: " << keySpace
         << " | Capacity: " << capacity << " (" << (capacity * 100 / keySpace) << "% of keyspace)\n";
    cout << "=======================================================================================\n";

    PolicyStats fifoStats{"FIFO", {}, {}};
    PolicyStats lruStats{"LRU", {}, {}};
    PolicyStats lfuStats{"LFU", {}, {}};

    // Instantiate our cache policies
    FIFOCache fifo(capacity);
    LRUCache  lru(capacity);
    LFUCache  lfu(capacity);

    for (unsigned int seed : seeds) {
        // Generate EXACT same access sequence for fair comparison
        vector<int> workload = generateWorkload(type, accesses, keySpace, seed);

        // Evaluate using polymorphism via base pointer (CachePolicy*)
        double fHit = evaluatePolicy(&fifo, workload);
        fifoStats.hitRates.push_back(fHit);
        fifoStats.missRates.push_back(100.0 - fHit);

        double lHit = evaluatePolicy(&lru, workload);
        lruStats.hitRates.push_back(lHit);
        lruStats.missRates.push_back(100.0 - lHit);

        double uHit = evaluatePolicy(&lfu, workload);
        lfuStats.hitRates.push_back(uHit);
        lfuStats.missRates.push_back(100.0 - uHit);
    }

    // Print summary table
    cout << left << setw(8)  << "Policy"
                 << setw(16) << "Avg Hit Rate"
                 << setw(16) << "Min Hit Rate"
                 << setw(16) << "Max Hit Rate"
                 << setw(16) << "Avg Miss Rate"
                 << setw(18) << "vs FIFO Improv." << "\n";
    cout << string(87, '-') << "\n";

    vector<PolicyStats> allStats = {fifoStats, lruStats, lfuStats};
    for (const auto& p : allStats) {
        double relImprov = (p.avgHitRate() - fifoStats.avgHitRate()) / fifoStats.avgHitRate() * 100.0;

        cout << left << setw(8) << p.name
             << fixed << setprecision(2)
             << setw(14) << p.avgHitRate() << "% "
             << setw(14) << p.minHitRate() << "% "
             << setw(14) << p.maxHitRate() << "% "
             << setw(14) << p.avgMissRate() << "% ";

        if (p.name == "FIFO") {
            cout << "baseline\n";
        } else {
            cout << (relImprov >= 0 ? "+" : "") << relImprov << "%\n";
        }
    }
    cout << string(87, '-') << "\n\n";
}

int main() {
    cout << "=========================================================\n";
    cout << "        C++ Cache Simulator (Simple OOP Architecture)    \n";
    cout << "=========================================================\n\n";

    // 1. Demonstration of Basic OOP & Polymorphism
    cout << "--- Demonstration of Polymorphism via Base Pointer ---\n";
    FIFOCache myFIFO(2);
    LRUCache  myLRU(2);
    LFUCache  myLFU(2);

    CachePolicy* policies[3] = { &myFIFO, &myLRU, &myLFU };
    string policyNames[3]    = { "FIFO", "LRU", "LFU" };

    for (int i = 0; i < 3; ++i) {
        // Polymorphic put() and get() calls:
        policies[i]->put(1, 100);
        policies[i]->put(2, 200);
        cout << "Policy " << policyNames[i]
             << " -> get(1): " << policies[i]->get(1)
             << " | get(2): " << policies[i]->get(2)
             << " | get(3): " << policies[i]->get(3) << " (not found)\n";
    }
    cout << "------------------------------------------------------\n\n";

    // 2. Simulation Benchmark
    vector<unsigned int> seeds = {42, 101, 202, 303, 404, 505, 606, 707, 808, 909};
    const int TOTAL_ACCESSES = 20000;
    const int KEY_SPACE      = 1000;
    const int CAPACITY       = 200; // 20% of keyspace

    // Test 1: Uniform Random Workload
    runWorkloadBenchmark("Uniform Random Workload (Zero Locality)",
                         WorkloadType::UNIFORM,
                         TOTAL_ACCESSES, KEY_SPACE, CAPACITY, seeds);

    // Test 2: 80/20 Hot-Cold Workload
    runWorkloadBenchmark("80/20 Hot-Cold Workload (80% accesses on top 20% keys)",
                         WorkloadType::HOT_COLD_80_20,
                         TOTAL_ACCESSES, KEY_SPACE, CAPACITY, seeds);

    // Test 3: 90/10 Highly Skewed Workload
    runWorkloadBenchmark("90/10 Highly-Skewed Workload (90% accesses on top 10% keys)",
                         WorkloadType::HOT_COLD_90_10,
                         TOTAL_ACCESSES, KEY_SPACE, CAPACITY, seeds);

    return 0;
}
