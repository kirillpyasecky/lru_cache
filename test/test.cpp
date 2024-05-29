#include <catch2/catch_all.hpp>

#include "src/cache.h"

const int NUM_OF_TEST1_RECORDS = 100;
const int NUM_OF_TEST2_RECORDS = 100;
const int TEST2_CACHE_CAPACITY = 50;

TEST_CASE("SimplePut") {
    cache::LRUCache<int, int> cache_lru(1);
    cache_lru.Insert(7, 777);
    REQUIRE(cache_lru.Contains(7));
    REQUIRE(777 == cache_lru.Get(7));
    REQUIRE(1 == cache_lru.Size());
}

TEST_CASE("MissingValue") {
    cache::LRUCache<int, int> cache_lru(1);
    REQUIRE_FALSE(cache_lru.Get(7).has_value());
}

TEST_CASE("KeepsAllValuesWithinCapacity") {
    cache::LRUCache<int, int> cache_lru(TEST2_CACHE_CAPACITY);

    for (int i = 0; i < NUM_OF_TEST2_RECORDS; ++i) {
        cache_lru.Insert(i, i);
    }

    for (int i = 0; i < NUM_OF_TEST2_RECORDS - TEST2_CACHE_CAPACITY; ++i) {
        REQUIRE_FALSE(cache_lru.Contains(i));
    }

    for (int i = NUM_OF_TEST2_RECORDS - TEST2_CACHE_CAPACITY; i < NUM_OF_TEST2_RECORDS; ++i) {
        REQUIRE(cache_lru.Contains(i));
        REQUIRE(i == cache_lru.Get(i).value());
    }

    size_t size = cache_lru.Size();
    REQUIRE(TEST2_CACHE_CAPACITY == size);
}