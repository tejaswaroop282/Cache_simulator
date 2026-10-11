# Cache Eviction Simulator & Benchmarking Engine

A lightweight C++17 simulation and benchmarking suite for three cache eviction policies — **FIFO**, **LRU** (LeetCode #146), and **LFU** (LeetCode #460) — with **O(1) average-time** `get` and `put`, built on a clean object-oriented design and evaluated against synthetic workloads.

---

## Highlights

- Three eviction policies behind one abstract interface (`CachePolicy`), swappable at runtime via polymorphism
- O(1) LRU using `std::list` + `std::unordered_map` with `splice()` (no heap allocation on access)
- O(1) LFU using frequency buckets and iterator tables, with LRU tie-breaking inside each frequency
- Benchmark harness: 3 workloads x 3 policies x 10 random seeds, reporting avg/min/max hit rate
- Unit tests covering edge cases (capacity 0/1, overwrites, eviction order, tie-breaks)

---

## Project Structure

```text
cache_simulator/
├── include/
│   ├── CachePolicy.h    # Abstract base class (pure virtual get, put, clear)
│   ├── FIFOCache.h
│   ├── LRUCache.h
│   └── LFUCache.h
├── src/
│   ├── FIFOCache.cpp    # unordered_map + queue
│   ├── LRUCache.cpp     # unordered_map + list (splice)
│   ├── LFUCache.cpp     # frequency buckets + iterators
│   └── main.cpp         # Workload generator, benchmark runner, polymorphism demo
├── tests/
│   └── cache_tests.cpp  # Unit tests and edge cases
├── CMakeLists.txt
└── README.md
```

---

## Design

```text
CachePolicy (abstract base class)
    ├── FIFOCache   First-In, First-Out
    ├── LRUCache    Least Recently Used
    └── LFUCache    Least Frequently Used
```

```cpp
class CachePolicy {
public:
    virtual ~CachePolicy() {}
    virtual int  get(int key) = 0;
    virtual void put(int key, int value) = 0;
    virtual void clear() = 0;
};
```

OOP concepts demonstrated:

| Concept | Where it shows up |
| :--- | :--- |
| Encapsulation | Containers, `capacity`, and `minFreq` are `private`; only `get`/`put`/`clear` are public |
| Abstraction | Pure virtual functions force every policy to supply its own eviction logic behind a common signature |
| Inheritance | `FIFOCache`, `LRUCache`, `LFUCache` all derive from `CachePolicy` |
| Runtime polymorphism | The benchmark drives every policy through a `CachePolicy*` without knowing the concrete type |
| Virtual destructor | Guarantees correct cleanup when deleting a derived object through a base pointer |

---

## Eviction Policies

| Policy | Eviction rule | Strength / weakness |
| :--- | :--- | :--- |
| **FIFO** | Evicts the oldest inserted item | Simple, but ignores recency and frequency; hot keys are evicted when their turn in the queue arrives |
| **LRU** | Evicts the item unused for the longest time | Strong when temporal locality is high |
| **LFU** | Evicts the item with the lowest access count (ties broken by LRU) | Best on skewed workloads; keeps hot keys alive through bursts of cold requests |

---

## Data Structures and Complexity

All operations are O(1) average time and O(capacity) space.

### FIFO
- `unordered_map<int, int>`: key to value
- `queue<int>`: insertion order for eviction

### LRU
- `list<pair<int, int>>`: front = most recently used, back = least recently used
- `unordered_map<int, list<...>::iterator>`: key to list node
- `list::splice()` moves a node to the front by relinking pointers, with no allocation or copying

### LFU
- `keyTable`: `unordered_map<int, pair<int, int>>`, key to `{value, frequency}`
- `freqTable`: `unordered_map<int, list<int>>`, frequency to keys at that frequency, ordered by recency
- `iterTable`: `unordered_map<int, list<int>::iterator>`, key to its position inside its frequency list
- `minFreq`: the lowest frequency currently present, so the eviction victim is found without scanning

---

## Benchmark Setup

| Parameter | Value |
| :--- | :--- |
| Accesses per trial | 20,000 |
| Distinct keys | 1,000 |
| Cache capacity | 200 (20% of the key space) |
| Trials | 10 random seeds per workload |

**Workloads**

1. **Uniform random** — every key equally likely (no locality)
2. **80/20 hot-cold** — 80% of requests go to the hottest 20% of keys
3. **90/10 skewed** — 90% of requests go to the hottest 10% of keys

---

## Results

### Experiment 1: Uniform random (no locality)

| Policy | Avg hit rate | Min | Max | Avg miss rate | vs FIFO |
| :--- | :---: | :---: | :---: | :---: | :---: |
| FIFO | 19.91% | 19.61% | 20.34% | 80.09% | baseline |
| LRU | 19.86% | 19.59% | 20.23% | 80.14% | -0.27% |
| LFU | 19.73% | 19.24% | 20.15% | 80.27% | -0.91% |

### Experiment 2: 80/20 hot-cold

| Policy | Avg hit rate | Min | Max | Avg miss rate | vs FIFO |
| :--- | :---: | :---: | :---: | :---: | :---: |
| FIFO | 51.64% | 50.57% | 52.37% | 48.36% | baseline |
| LRU | 57.61% | 56.55% | 58.37% | 42.39% | +11.56% |
| LFU | **74.06%** | 72.84% | 75.97% | 25.94% | **+43.42%** |

### Experiment 3: 90/10 highly skewed

| Policy | Avg hit rate | Min | Max | Avg miss rate | vs FIFO |
| :--- | :---: | :---: | :---: | :---: | :---: |
| FIFO | 83.62% | 82.84% | 84.05% | 16.38% | baseline |
| LRU | 90.60% | 90.16% | 90.86% | 9.40% | +8.35% |
| LFU | 90.60% | 90.10% | 90.93% | 9.40% | +8.35% |

"vs FIFO" is the relative improvement in average hit rate.

### What the results show

- **Uniform:** with no locality, no policy can beat the ~20% baseline set by capacity / key space. All three land within noise of each other, which is a useful sanity check on the simulator.
- **80/20:** the hot set (about 200 keys) is roughly the size of the cache, so every cold request is a threat. LRU lets cold keys push hot ones out; LFU's frequency counts protect the hot set, producing the largest gap (+43.42% over FIFO).
- **90/10:** the hot set (about 100 keys) fits comfortably inside the 200-slot cache, so both LRU and LFU hold it and tie. The advantage of LFU appears when the hot set is close to or larger than capacity.

---

## Build and Run

### Direct compilation with g++

```bash
# Benchmark simulation
g++ -std=c++17 -O3 -Iinclude src/FIFOCache.cpp src/LRUCache.cpp src/LFUCache.cpp src/main.cpp -o cache_sim
./cache_sim

# Unit tests
g++ -std=c++17 -O3 -Iinclude src/FIFOCache.cpp src/LRUCache.cpp src/LFUCache.cpp tests/cache_tests.cpp -o cache_tests
./cache_tests
```

On Windows (PowerShell), use `-o cache_sim.exe` and run `.\cache_sim.exe`.

### CMake

```bash
cmake -B build
cmake --build build
./build/cache_sim
./build/cache_tests
```

On Windows with MinGW: `cmake -B build -G "MinGW Makefiles"`.

---

## Tech Stack

C++17, STL (`unordered_map`, `list`, `queue`), CMake, object-oriented design
