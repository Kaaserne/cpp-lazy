#pragma once

#ifndef LZ_RANDOM_ADAPTOR_HPP
#define LZ_RANDOM_ADAPTOR_HPP

#include <Lz/detail/iterables/random.hpp>
#include <Lz/detail/concepts.hpp>
#include <random>

namespace lz {
namespace detail {
template<std::size_t N>
class seed_sequence {
public:
    using result_type = std::seed_seq::result_type;

private:
    using seed_array = std::array<result_type, N>;
    seed_array _seed;

    template<class Iter>
    LZ_CONSTEXPR_CXX_20 void create(Iter begin, Iter end) {
        using value_type = val_t<Iter>;
        std::transform(begin, end, _seed.begin(), [](const value_type val) { return static_cast<result_type>(val); });
    }

    result_type T(const result_type x) const {
        return x ^ (x >> 27u);
    }

public:
    constexpr seed_sequence() = default;

    explicit seed_sequence(std::random_device& rd) {
        std::generate(_seed.begin(), _seed.end(), [&rd]() { return static_cast<result_type>(rd()); });
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
        const auto n = static_cast<std::size_t>(end - begin);
        constexpr auto s = N;
        const std::size_t m = std::max(s + 1, n);
        const std::size_t t = (n >= 623) ? 11 : (n >= 68) ? 7 : (n >= 39) ? 5 : (n >= 7) ? 3 : (n - 1) / 2;
        const std::size_t p = (n - t) / 2;
        const std::size_t q = p + t;

        auto mask = static_cast<iter_value_type>(1) << 31;
        mask <<= 1;
        mask -= 1;

        for (std::size_t k = 0; k < m - 1; k++) {
            const std::size_t k_mod_n = k % n;
            const std::size_t k_plus_mod_n = (k + p) % n;
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

        for (std::size_t k = m; k < m + n - 1; k++) {
            const std::size_t k_mod_n = k % n;
            const std::size_t k_plus_mod_n = (k + p) % n;
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

    static constexpr std::size_t size() {
        return N;
    }
};

inline std::mt19937 create_mt_engine() {
    std::random_device rd;
    seed_sequence<8> seed_seq(rd);
    return std::mt19937(seed_seq);
}

template<bool UseSentinel>
struct random_adaptor {
    using adaptor = random_adaptor<UseSentinel>;

    template<class Distribution, class Generator>
    LZ_NODISCARD constexpr random_iterable<typename Distribution::result_type, Distribution, Generator, UseSentinel>
    operator()(const Distribution& distribution, Generator& generator, const std::size_t amount) const {
        return { distribution, generator, static_cast<std::ptrdiff_t>(amount) };
    }

    template<class Integral>
    LZ_NODISCARD
    enable_if<std::is_integral<Integral>::value, random_iterable<Integral, std::uniform_int_distribution<Integral>, std::mt19937, UseSentinel>>
    operator()(const Integral min, const Integral max, const std::size_t amount) const {
        static std::mt19937 gen = create_mt_engine();
        std::uniform_int_distribution<Integral> dist(min, max);
        return (*this)(dist, gen, amount);
    }

    template<class Floating>
    LZ_NODISCARD
    enable_if<std::is_floating_point<Floating>::value, random_iterable<Floating, std::uniform_real_distribution<Floating>, std::mt19937, UseSentinel>>
    operator()(const Floating min, const Floating max, const std::size_t amount) const {
        static std::mt19937 gen = create_mt_engine();
        std::uniform_real_distribution<Floating> dist(min, max);
        return (*this)(dist, gen, amount);
    }
};
} // namespace detail
} // namespace lz

#endif