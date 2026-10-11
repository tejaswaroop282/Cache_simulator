# C++ Cache Eviction Simulator & Benchmarking Engine

A lightweight, interview-ready C++17 cache simulation and benchmarking suite implementing **FIFO**, **LRU** (LeetCode #146), and **LFU** (LeetCode #460) cache eviction policies with **$O(1)$ average-time complexity**, structured with clean, beginner-friendly C++ OOP architecture.

---

## 1. Object-Oriented Architecture

The project uses clean, conventional C++ Object-Oriented Programming (OOP) with an abstract base class and public inheritance:

```text
CachePolicy (Abstract Base Class / Interface)
    │
    ├── FIFOCache  (First-In, First-Out)
    ├── LRUCache   (Least Recently Used)
    └── LFUCache   (Least Frequently Used)
```

### File Organization

```text
cache_simulator/
├── include/
│   ├── CachePolicy.h    # Abstract base class (virtual get, put, clear)
│   ├── FIFOCache.h      # FIFOCache declaration
│   ├── LRUCache.h       # LRUCache declaration
│   └── LFUCache.h       # LFUCache declaration
├── src/
│   ├── FIFOCache.cpp    # FIFO implementation (unordered_map + queue)
│   ├── LRUCache.cpp     # LRU implementation (unordered_map + list with splice)
│   ├── LFUCache.cpp     # LFU implementation (frequency buckets + iterators)
│   └── main.cpp         # Main simulation & polymorphism demo
├── tests/
│   └── cache_tests.cpp  # Unit test suite & edge case checks
├── CMakeLists.txt       # CMake build configuration
└── README.md
```

### Core OOP Concepts Used

1. **Class**: User-defined blueprint grouping related data (state) and member functions (behavior) together.
2. **Private Members**: Data structures (`queue`, `list`, `unordered_map`, `capacity`, `minFreq`) are marked `private` so external code cannot corrupt cache internals directly (**Encapsulation**).
3. **Public Methods**: Methods like `get()`, `put()`, and `clear()` are marked `public` as the standard external interface for using the cache.
4. **Constructor**: Initializes cache capacity and starting state when an object is instantiated.
5. **Inheritance (`: public CachePolicy`)**: Allows `FIFOCache`, `LRUCache`, and `LFUCache` to share a common type and interface.
6. **Virtual Destructor (`virtual ~CachePolicy() {}`)**: Ensures derived objects are properly deleted when destroyed through a base class pointer.
7. **Pure Virtual Functions (`virtual int get(int key) = 0;`)**: Forces every derived class to provide its own eviction logic while ensuring a consistent calling signature (**Abstraction**).
8. **Runtime Polymorphism**: Any function or caller can interact with `CachePolicy*` or `CachePolicy&` without knowing whether it is FIFO, LRU, or LFU under the hood.

---

## 2. How the Cache Policies Work

| Policy | Eviction Strategy | Key Insight / Weakness |
| :--- | :--- | :--- |
| **FIFO** (First-In, First-Out) | Evicts the item that was inserted earliest. | Ignores frequency and recency. Frequently accessed "hot" keys get repeatedly evicted when their queue turn arrives. |
| **LRU** (Least Recently Used) | Evicts the item that has not been accessed for the longest period. | Excels when temporal locality is high. Performs well on recency-dominated workloads. |
| **LFU** (Least Frequently Used) | Evicts the item with the lowest total access count. Breaks frequency ties using LRU. | Ideal for skewed workloads (80/20 & 90/10). Retains frequently accessed keys even when intermittent cold requests arrive. |

---

## 3. Data Structures & Time/Space Complexity

### 🔹 FIFO Cache
* **Data Structures:** 
  * `std::unordered_map<int, int>`: $O(1)$ key $\to$ value lookup.
  * `std::queue<int>`: $O(1)$ push/pop to track insertion order.
* **Complexities:**
  * `get(key)`: **$O(1)$** average time
  * `put(key, val)`: **$O(1)$** average time
  * Space: **$O(\text{Capacity})$**

### 🔹 LRU Cache ($O(1)$ LeetCode #146)
* **Data Structures:**
  * `std::list<std::pair<int, int>>`: Doubly-linked list where **Front = Most Recently Used (MRU)** and **Back = Least Recently Used (LRU)**.
  * `std::unordered_map<int, std::list<std::pair<int, int>>::iterator>`: Maps key to list node iterator for $O(1)$ node lookup and splicing.
* **Key Operation:**
  * `std::list::splice()` moves nodes to the front in **$O(1)$ pointer-relocation time** without heap allocations.
* **Complexities:**
  * `get(key)`: **$O(1)$** average time
  * `put(key, val)`: **$O(1)$** average time
  * Space: **$O(\text{Capacity})$**

### 🔹 LFU Cache ($O(1)$ LeetCode #460)
* **Data Structures (Frequency Bucketing):**
  * `keyTable`: `std::unordered_map<int, std::pair<int, int>>` $\to$ maps `key` to `{value, frequency}`.
  * `freqTable`: `std::unordered_map<int, std::list<int>>` $\to$ maps `frequency` to a doubly-linked list of keys sharing that frequency (ordered by recency).
  * `iterTable`: `std::unordered_map<int, std::list<int>::iterator>` $\to$ maps `key` to its iterator position inside `freqTable[frequency]`.
  * `minFreq`: Integer tracking the global minimum frequency currently in the cache.
* **Complexities:**
  * `get(key)`: **$O(1)$** average time
  * `put(key, val)`: **$O(1)$** average time
  * Space: **$O(\text{Capacity})$**

---

## 4. Empirical Benchmark Results

**Simulation Setup:**
* **Total Accesses per trial:** 20,000 requests
* **Key Space:** 1,000 distinct keys
* **Cache Capacity:** 200 items (20% of keyspace)
* **Number of Seeds:** 10 trials

### Experiment 1: Uniform Random Workload (Zero Locality)
| Policy | Avg Hit Rate | Min Hit Rate | Max Hit Rate | Avg Miss Rate | vs FIFO Improv. |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **FIFO** | **19.91%** | 19.61% | 20.34% | 80.09% | *baseline* |
| **LRU**  | **19.86%** | 19.59% | 20.23% | 80.14% | -0.27% |
| **LFU**  | **19.73%** | 19.24% | 20.15% | 80.27% | -0.91% |

### Experiment 2: 80/20 Hot-Cold Workload (Realistic Workload)
| Policy | Avg Hit Rate | Min Hit Rate | Max Hit Rate | Avg Miss Rate | vs FIFO Improv. |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **FIFO** | **51.64%** | 50.57% | 52.37% | 48.36% | *baseline* |
| **LRU**  | **57.61%** | 56.55% | 58.37% | 42.39% | **+11.56%** |
| **LFU**  | **74.06%** | 72.84% | 75.97% | 25.94% | **+43.42%** |

### Experiment 3: 90/10 Highly-Skewed Workload
| Policy | Avg Hit Rate | Min Hit Rate | Max Hit Rate | Avg Miss Rate | vs FIFO Improv. |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **FIFO** | **83.62%** | 82.84% | 84.05% | 16.38% | *baseline* |
| **LRU**  | **90.60%** | 90.16% | 90.86% | 9.40% | **+8.35%** |
| **LFU**  | **90.60%** | 90.10% | 90.93% | 9.40% | **+8.35%** |

---

## 5. How to Build and Run

### Option A: Direct Compilation with `g++`
```powershell
# Compile and run the benchmark simulation
g++ -std=c++17 -O3 -Iinclude src/FIFOCache.cpp src/LRUCache.cpp src/LFUCache.cpp src/main.cpp -o cache_sim.exe
.\cache_sim.exe

# Compile and run the unit test suite
g++ -std=c++17 -O3 -Iinclude src/FIFOCache.cpp src/LRUCache.cpp src/LFUCache.cpp tests/cache_tests.cpp -o cache_tests.exe
.\cache_tests.exe
```

### Option B: Build with CMake
```powershell
cmake -B build -G "MinGW Makefiles"
cmake --build build
.\build\cache_sim.exe
.\build\cache_tests.exe
```
