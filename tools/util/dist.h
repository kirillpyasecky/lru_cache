#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <stdexcept>
#include <type_traits>

template <typename IntType = int>
class UniformIntDistribution {
    static_assert(std::is_integral_v<IntType>, "template argument must be an integral type");

   public:
    UniformIntDistribution() : UniformIntDistribution(0) {
    }

    explicit UniformIntDistribution(IntType a, IntType b = std::numeric_limits<IntType>::max())
        : a_{a}, b_{b} {
    }

    template <class Gen>
    IntType operator()(Gen& gen) {
        return this->operator()(gen, a_, b_);
    }

   private:
    IntType a_;
    IntType b_;

    template <class Wp, class Urbg, class Up>
    static Up SNd(Urbg& g, Up range) {
        using UpTraits = std::numeric_limits<Up>;
        using WpTraits = std::numeric_limits<Wp>;
        static_assert(!UpTraits::is_signed, "U must be unsigned");
        static_assert(!WpTraits::is_signed, "W must be unsigned");
        static_assert(WpTraits::digits == (2 * UpTraits::digits), "W must be twice as wide as U");

        Wp product = static_cast<Wp>(g()) * static_cast<Wp>(range);
        auto low = static_cast<Up>(product);
        if (low < range) {
            Up threshold = -range % range;
            while (low < threshold) {
                product = static_cast<Wp>(g()) * static_cast<Wp>(range);
                low = static_cast<Up>(product);
            }
        }
        return product >> static_cast<uint64_t>(UpTraits::digits);
    }

    template <class Gen>
    IntType operator()(Gen& urng, IntType a, IntType b);

    template <class>
    static constexpr auto kDependentFalse = false;
};

template <class IntType>
template <class Gen>
IntType UniformIntDistribution<IntType>::operator()(Gen& urng, IntType a, IntType b) {
    using UType = std::make_unsigned_t<IntType>;
    using UCType = std::common_type_t<typename Gen::result_type, UType>;

    constexpr UCType kUrngMin = Gen::min();
    constexpr UCType kUrngMax = Gen::max();
    static_assert(kUrngMin < kUrngMax, "Uniform random bit generator must define min() < max()");
    constexpr UCType kUrngRange = kUrngMax - kUrngMin;

    const UCType urange = static_cast<UCType>(b) - static_cast<UCType>(a);
    UCType ret{};
    if (kUrngRange > urange) {
        const UCType uerange = urange + 1;
#if __SIZEOF_INT128__
        if constexpr (kUrngRange == std::numeric_limits<uint64_t>::max()) {
            uint64_t u64erange = uerange;
            ret = __extension__ SNd<unsigned __int128>(urng, u64erange);
        } else
#else
#error "__int128 is not defined"
#endif
            if constexpr (kUrngRange == std::numeric_limits<uint32_t>::max()) {
            const uint32_t u32erange = uerange;
            ret = SNd<uint64_t>(urng, u32erange);
        } else {
            const UCType scaling = kUrngRange / uerange;
            const UCType past = uerange * scaling;
            do {  // NOLINT(cppcoreguidelines-avoid-do-while)
                ret = static_cast<UCType>(urng()) - kUrngMin;
            } while (ret >= past);
            ret /= scaling;
        }
    } else if (kUrngRange < urange) {
        const UCType uerngrange = kUrngRange + 1;
        if (uerngrange == 0) {
            __builtin_unreachable();
        }
        UCType tmp;
        do {  // NOLINT(cppcoreguidelines-avoid-do-while)
            if (uerngrange == 0) {
                throw std::runtime_error("");
            }
            tmp = (uerngrange * operator()(urng, 0, urange / uerngrange));
            ret = tmp + (static_cast<UCType>(urng()) - kUrngMin);
        } while (ret > urange || ret < tmp);
    } else {
        ret = static_cast<UCType>(urng()) - kUrngMin;
    }

    return ret + a;
}

template <class RealType = double>
class UniformRealDistribution {
   public:
    static_assert(std::is_floating_point_v<RealType>, "result_type must be a floating point type");

    UniformRealDistribution() : UniformRealDistribution(0.) {
    }

    explicit UniformRealDistribution(RealType a, RealType b = RealType{1}) : a_{a}, b_{b} {
    }

    RealType operator()(auto& urng) {
        return GenerateCanonical(urng) * (b_ - a_) + a_;
    }

   private:
    template <class Gen>
    static RealType GenerateCanonical(Gen& urng) {
        constexpr auto kBits = std::numeric_limits<RealType>::digits;
        constexpr auto kR =
            static_cast<long double>(Gen::max()) - static_cast<long double>(Gen::min()) + 1.L;
        const size_t log2r = std::log(kR) / std::log(2.L);
        const size_t m = std::max<size_t>(1UL, (kBits + log2r - 1UL) / log2r);
        RealType sum{0};
        RealType tmp{1};
        for (auto k = m; k != 0; --k) {
            sum += static_cast<RealType>(urng() - urng.min()) * tmp;
            tmp *= kR;
        }
        RealType ret = sum / tmp;
        if (ret >= RealType{1}) {
            return std::nextafter(RealType{1}, RealType{0});
        }
        return ret;
    }

    RealType a_;
    RealType b_;
};
