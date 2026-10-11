cmake_minimum_required(VERSION 3.14)
project(cache_simulator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -O3)
endif()

include_directories(include)

# Shared cache policy source files
set(CACHE_SOURCES
    src/FIFOCache.cpp
    src/LRUCache.cpp
    src/LFUCache.cpp
)

# 1. Main cache simulation & benchmark program
add_executable(cache_sim
    src/main.cpp
    ${CACHE_SOURCES}
)

# 2. Standalone unit test suite
add_executable(cache_tests
    tests/cache_tests.cpp
    ${CACHE_SOURCES}
)
