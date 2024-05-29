#pragma once

#include "dist.h"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <numeric>
#include <random>
#include <vector>

#ifdef __linux__
#include <sys/resource.h>
#include <sys/time.h>
#endif

class RandomGenerator {
   public:
    explicit RandomGenerator(
        uint32_t seed = 738'547'485U)  // NOLINT(fuchsia-default-arguments-declarations)
        : gen_(seed) {
    }

    template <class T>
    std::vector<T> GenIntegralVector(size_t count, T from, T to) {
        UniformIntDistribution dist{from, to};
        std::vector<T> result(count);
        for (auto& cur : result) {
            cur = dist(gen_);
        }
        return result;
    }

    std::string GenString(
        size_t count, char from = 'a',  // NOLINT(fuchsia-default-arguments-declarations)
        char to = 'z') {                // NOLINT(fuchsia-default-arguments-declarations)
        UniformIntDistribution<int> dist{from, to};
        std::string result(count, from);
        for (char& x : result) {
            x = static_cast<char>(dist(gen_));
        }
        return result;
    }

    std::vector<double> GenRealVector(size_t count, double from, double to) {
        UniformRealDistribution dist{from, to};
        std::vector<double> result(count);
        for (auto& cur : result) {
            cur = dist(gen_);
        }
        return result;
    }

    std::vector<int> GenPermutation(size_t count) {
        std::vector<int> result(count);
        std::iota(result.begin(), result.end(), 0);
        std::ranges::shuffle(result, gen_);
        return result;
    }

    template <class T>
    T GenInt(T from, T to) {
        if constexpr (std::same_as<T, char>) {
            UniformIntDistribution dist{from, to};
            return static_cast<char>(dist(gen_));
        } else {
            UniformIntDistribution dist{from, to};
            return dist(gen_);
        }
    }

    template <class T>
    T GenInt() {
        return GenInt(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
    }

    char GenChar(char from, char to) {
        UniformIntDistribution<int> dist{from, to};
        return static_cast<char>(dist(gen_));
    }

    char GenChar() {
        return GenChar('a', 'z');
    }

    template <class Iterator>
    void Shuffle(Iterator first, Iterator last) {
        std::shuffle(first, last, gen_);
    }

   private:
    std::mt19937 gen_;
};

inline std::filesystem::path GetFileDir(std::string file) {
    const std::filesystem::path path{std::move(file)};
    if (path.is_absolute() && std::filesystem::is_regular_file(path)) {
        return path.parent_path();
    }
    throw std::runtime_error{"Bad file name"};
}

class Timer {
    using Clock = std::chrono::steady_clock;

    struct Times {
        Clock::duration wall_time;
        std::chrono::microseconds cpu_time;
    };

   public:
    [[nodiscard]] Times GetTimes() const {
        return {Clock::now() - wall_start_, GetCPUTime() - cpu_start_};
    }

   private:
    static std::chrono::microseconds GetCPUTime() {
#ifdef __linux__
        rusage usage{};
        if (getrusage(RUSAGE_SELF, &usage) != 0) {
            throw std::system_error{errno, std::generic_category()};
        }
        auto time = usage.ru_utime;
        return std::chrono::microseconds{1'000'000LL * time.tv_sec + time.tv_usec};
#else
        auto time = Clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(time);
#endif
    }

    std::chrono::time_point<Clock> wall_start_ = Clock::now();
    std::chrono::microseconds cpu_start_ = GetCPUTime();
};

#ifdef __linux__
#include <fstream>
#include <unistd.h>

inline int64_t GetMemoryUsage() {
    rusage usage{};
    if (getrusage(RUSAGE_SELF, &usage) != 0) {
        throw std::system_error{errno, std::generic_category()};
    }
    return usage.ru_maxrss;  // NOLINT(cppcoreguidelines-pro-type-union-access)
}

class MemoryGuard {
   public:
    explicit MemoryGuard(size_t bytes) {
        if (is_active) {
            throw std::runtime_error{"There is an active memory guard"};
        }
        is_active = true;
        bytes += GetDataMemoryUsage();
        if (const rlimit limit{bytes, RLIM_INFINITY}; ::setrlimit(RLIMIT_DATA, &limit)) {
            throw std::system_error{errno, std::generic_category()};
        }
    }

    ~MemoryGuard() {
        is_active = false;
        const rlimit limit{RLIM_INFINITY, RLIM_INFINITY};
        ::setrlimit(RLIMIT_DATA, &limit);
    }

    MemoryGuard(const MemoryGuard&) = delete;
    MemoryGuard& operator=(const MemoryGuard&) = delete;
    MemoryGuard(MemoryGuard&&) = delete;
    MemoryGuard& operator=(MemoryGuard&&) = delete;

   private:
    static size_t GetDataMemoryUsage() {
        size_t pages = 0;
        std::ifstream in{"/proc/self/statm"};
        for (auto i = 0; i < 6; ++i) {
            in >> pages;
        }
        if (!in) {
            throw std::runtime_error{"Failed to get number of pages"};
        }
        return pages * kPageSize;
    }

    static inline const int kPageSize = getpagesize();
    static inline auto is_active = false;  // NOLINT
};

template <class T>
MemoryGuard MakeMemoryGuard(size_t n) {
    return MemoryGuard{n * sizeof(T)};
}

#endif
