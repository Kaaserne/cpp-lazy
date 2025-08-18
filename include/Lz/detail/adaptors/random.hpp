#pragma once

#ifndef LZ_RANDOM_ADAPTOR_HPP
#define LZ_RANDOM_ADAPTOR_HPP

#include <Lz/detail/iterables/random.hpp>
#include <algorithm>
#include <random>

namespace lz {
namespace detail {
template<size_t N>
class seed_sequence {
public:
    using result_type = std::seed_seq::result_type;

private:
    using seed_array = result_type[N];
    seed_array _seed;

    template<class Iter>
    LZ_CONSTEXPR_CXX_20 void create(Iter begin, Iter end) {
        using value_type = val_t<Iter>;
        std::transform(begin, end, std::begin(_seed), [](const value_type val) { return static_cast<result_type>(val); });
    }

    result_type T(const result_type x) const {
        return x ^ (x >> 27u);
    }

public:
    constexpr seed_sequence() = default;

    explicit seed_sequence(std::random_device& rd) {
        std::generate(std::begin(_seed), std::end(_seed), [&rd]() { return static_cast<result_type>(rd()); });
    }

    template<class T>
    LZ_CONSTEXPR_CXX_20 seed_sequence(std::initializer_list<T> values) {
        create(values.begin(), values.end());
    }

    template<class Iter>
    LZ_CONSTEXPR_CXX_20 seed_sequence(Iter first, Iter last) {
        create(first, last);
    }

    seed_sequence(const seed_sequence&) = delete;
    seed_sequence& operator=(const seed_sequence&) = delete;

    template<class Iter>
    LZ_CONSTEXPR_CXX_20 void generate(Iter begin, Iter end) const {
        if (begin == end) {
            return;
        }

        using iter_value_type = val_t<Iter>;

        std::fill(begin, end, 0x8b8b8b8b);
        const auto n = static_cast<size_t>(end - begin);
        constexpr auto s = N;
        const size_t m = std::max(s + 1, n);
        const size_t t = (n >= 623) ? 11 : (n >= 68) ? 7 : (n >= 39) ? 5 : (n >= 7) ? 3 : (n - 1) / 2;
        const size_t p = (n - t) / 2;
        const size_t q = p + t;

        auto mask = static_cast<iter_value_type>(1) << 31;
        mask <<= 1;
        mask -= 1;

        for (size_t k = 0; k < m - 1; k++) {
            const size_t k_mod_n = k % n;
            const size_t k_plus_mod_n = (k + p) % n;
            const result_type r1 = 1664525 * T(begin[k_mod_n] ^ begin[k_plus_mod_n] ^ begin[(k - 1) % n]);

            result_type r2;
            if (k == 0) {
                r2 = static_cast<result_type>((r1 + s) & mask);
            }
            else if (k <= s) {
                r2 = static_cast<result_type>((r1 + k_mod_n + _seed[k - 1]) & mask);
            }
            else {
                r2 = static_cast<result_type>((r1 + k_mod_n) & mask);
            }

            begin[k_plus_mod_n] += (r1 & mask);
            begin[(k + q) % n] += (r2 & mask);
            begin[k_mod_n] = r2;
        }

        for (size_t k = m; k < m + n - 1; k++) {
            const size_t k_mod_n = k % n;
            const size_t k_plus_mod_n = (k + p) % n;
            const result_type r3 = 1566083941 * T(begin[k_mod_n] + begin[k_plus_mod_n] + begin[(k - 1) % n]);
            const auto r4 = static_cast<result_type>((r3 - k_mod_n) & mask);

            begin[k_plus_mod_n] ^= (r3 & mask);
            begin[(k + q) % n] ^= (r4 & mask);
            begin[k_mod_n] = r4;
        }
    }

    template<class Iter>
    LZ_CONSTEXPR_CXX_20 void param(Iter output_iter) const {
        std::copy(_seed.begin(), _seed.end(), output_iter);
    }

    static constexpr size_t size() noexcept {
        return N;
    }
};

using prng_engine = std::mt19937;

inline prng_engine create_engine() {
    std::random_device rd;
    seed_sequence<8> seed_seq(rd);
    return prng_engine(seed_seq);
}

template<bool UseSentinel>
struct random_adaptor {
    using adaptor = random_adaptor<UseSentinel>;

    /**
     * @brief Generates n amount of random numbers. May or may not contain a sentinel, depending on the adaptor used. Use
     * `lz::common_random` for no sentinel, otherwise use `lz::random`. Prefer using `lz::random` if you do not need an actual end
     * iterator to avoid unnecessary overhead. Further, it contains a .size() method and is random access. Example:
     * ```cpp
     * std::mt19937 gen;
     * std::uniform_real_distribution<double> dist(0., 1.);
     * auto random = lz::random(dist, gen, 5); // random = { 0.1, 0.2, 0.3, 0.4, 0.5 } (random * numbers)
     * ```
     * @param distribution The random distribution to use, for instance std::uniform_real_distribution<double>.
     * @param generator The random engine to use, for instance std::mt19937.
     * @param amount The amount of random numbers to generate.
     */
    template<class Distribution, class Generator>
    LZ_NODISCARD constexpr random_iterable<typename Distribution::result_type, Distribution, Generator, UseSentinel>
    operator()(const Distribution& distribution, Generator& generator, const size_t amount) const {
        return { distribution, generator, amount };
    }

#ifdef LZ_HAS_CXX_17

    /**
     * @brief Generates n amount of random numbers. May or may not contain a sentinel, depending on the adaptor used. Use
     * `lz::common_random` for no sentinel, otherwise use `lz::random`. Prefer using `lz::random` if you do not need an actual end
     * iterator to avoid unnecessary overhead. Further, it contains a .size() method and is random access. Example:
     * ```cpp
     * // Uses std::uniform_real_distribution<double> as distribution, a seed length of 8 random numbers (using
     * // std::random_device) and a std::mt19937 engine.
     * auto random = lz::common_random(0., 1., 5); // random = { 0.1, 0.2, 0.3, 0.4, 0.5 } 5 random double numbers between 0 and 1
     * // (inclusive)
     * // with integers. Uses std::uniform_int_distribution<int> as distribution, a seed length of 8 random numbers (using
     * // std::random_device) and a std::mt19937 engine.
     * auto random = lz::common_random(0, 10, 5); // random = { 1, 2, 3, 4, 5 } 5 random integers between 0 and 10 (inclusive)
     * ```
     * @param min The minimum value of the random numbers (inclusive).
     * @param max The maximum value of the random numbers (inclusive).
     * @param amount The amount of random numbers to generate.
     */
    template<class T>
    [[nodiscard]] auto operator()(const T min, const T max, const size_t amount) const {
        static auto gen = create_engine();
        if constexpr (std::is_integral_v<T>) {
            return (*this)(std::uniform_int_distribution<T>{ min, max }, gen, amount);
        }
        else if constexpr (std::is_floating_point_v<T>) {
            return (*this)(std::uniform_real_distribution<T>{ min, max }, gen, amount);
        }
        else {
            static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                          "Type must be either integral or floating point for random generation.");
        }
    }
#else

    /**
     * @brief Generates n amount of random numbers. May or may not contain a sentinel, depending on the adaptor used. Use
     * `lz::common_random` for no sentinel, otherwise use `lz::random`. Prefer using `lz::random` if you do not need an actual end
     * iterator to avoid unnecessary overhead. Further, it contains a .size() method. Example:
     * ```cpp
     * // Uses std::uniform_real_distribution<double> as distribution, a seed length of 8 random numbers (using
     * // std::random_device) and a std::mt19937 engine.
     * auto random = lz::common_random(0., 1., 5); // random = { 0.1, 0.2, 0.3, 0.4, 0.5 } 5 random double numbers between 0 and 1
     * // (inclusive)
     * // with integers. Uses std::uniform_int_distribution<int> as distribution, a seed length of 8 random numbers (using
     * // std::random_device) and a std::mt19937 engine.
     * auto random = lz::common_random(0, 10, 5); // random = { 1, 2, 3, 4, 5 } 5 random integers between 0 and 10 (inclusive)
     * ```
     * @param min The minimum value of the random numbers (inclusive).
     * @param max The maximum value of the random numbers (inclusive).
     * @param amount The amount of random numbers to generate.
     */
    template<class Integral>
    LZ_NODISCARD enable_if<std::is_integral<Integral>::value,
                           random_iterable<Integral, std::uniform_int_distribution<Integral>, prng_engine, UseSentinel>>
    operator()(const Integral min, const Integral max, const size_t amount) const {
        static auto gen = create_engine();
        return (*this)(std::uniform_int_distribution<Integral>{ min, max }, gen, amount);
    }

    /**
     * @brief Generates n amount of random numbers. May or may not contain a sentinel, depending on the adaptor used. Use
     * `lz::common_random` for no sentinel, otherwise use `lz::random`. Prefer using `lz::random` if you do not need an actual end
     * iterator to avoid unnecessary overhead. Further, it contains a .size() method. Example:
     * ```cpp
     * // Uses std::uniform_real_distribution<double> as distribution, a seed length of 8 random numbers (using
     * // std::random_device) and a std::std::mt19937 engine.
     * auto random = lz::common_random(0., 1., 5); // random = { 0.1, 0.2, 0.3, 0.4, 0.5 } 5 random double numbers between 0 and 1
     * // (inclusive). Uses std::uniform_int_distribution<int> as distribution, a seed length of 8 random numbers (using
     * // std::random_device) and a std::std::mt19937 engine.
     * auto random = lz::common_random(0, 10, 5); // random = { 1, 2, 3, 4, 5 } 5 random integers between 0 and 10 (inclusive)
     * ```
     * @param min The minimum value of the random numbers (inclusive).
     * @param max The maximum value of the random numbers (inclusive).
     * @param amount The amount of random numbers to generate.
     */
    template<class Floating>
    LZ_NODISCARD enable_if<std::is_floating_point<Floating>::value,
                           random_iterable<Floating, std::uniform_real_distribution<Floating>, prng_engine, UseSentinel>>
    operator()(const Floating min, const Floating max, const size_t amount) const {
        static auto gen = create_engine();
        return (*this)(std::uniform_real_distribution<Floating>{ min, max }, gen, amount);
    }

#endif
};
} // namespace detail
} // namespace lz

#endif