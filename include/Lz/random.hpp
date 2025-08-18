#pragma once

#ifndef LZ_RANDOM_HPP
#define LZ_RANDOM_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/random.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Creates n random numbers in the range [min, max]. It contains a .size() method, is random access and has a sentinel. The
 * callable contains various amount of overloads. Example:
 * ```cpp
 * // overload 1. Uses std::uniform_real_distribution<double> as distribution, a seed length of 8 random numbers (using
 * // std::random_device) and a mt19937 engine.
 * auto random = lz::random(0., 1., 5); // random = { 0.1, 0.2, 0.3, 0.4, 0.5 } (random * numbers)
 *
 * // overload 2 Uses std::uniform_int_distribution<int> as distribution, a seed length of 8 random numbers (using
 * // std::random_device) and a mt19937 engine.
 * auto random = lz::random(0, 10, 5); // random = { 1, 2, 3, 4, 5 } (random numbers)
 *
 * // overload 3. Uses a custom distribution, a custom engine and a custom amount of random numbers.
 * std::mt19937 gen;
 * std::uniform_real_distribution<double> dist(0., 1.);
 * auto random = lz::random(dist, gen, 5); // random = { 0.1, 0.2, 0.3, 0.4, 0.5 } (random * numbers)
 * ```
 */
LZ_INLINE_VAR constexpr detail::random_adaptor<true> random{};

/**
 * @brief Creates n random numbers in the range [min, max]. It contains a .size() method, is random access and does NOT have a
 * sentinel. The callable contains various amount of overloads. Example:
 * ```cpp
 * // overload 1. Uses std::uniform_real_distribution<double> as distribution, a seed length of 8 random numbers (using
 * // std::random_device) and a mt19937 engine.
 * auto random = lz::common_random(0., 1., 5); // random = { 0.1, 0.2, 0.3, 0.4, 0.5 } (random * numbers)
 *
 * // overload 2 Uses std::uniform_int_distribution<int> as distribution, a seed length of 8 random numbers (using
 * // std::random_device) and a mt19937 engine.
 * auto random = lz::common_random(0, 10, 5); // random = { 1, 2, 3, 4, 5 } (random numbers)
 *
 * // overload 3. Uses a custom distribution, a custom engine and a custom amount of random numbers.
 * std::mt19937 gen;
 * std::uniform_real_distribution<double> dist(0., 1.);
 * auto random = lz::common_random(dist, gen, 5); // random = { 0.1, 0.2, 0.3, 0.4, 0.5 } (random * numbers)
 * ```
 */
LZ_INLINE_VAR constexpr detail::random_adaptor<false> common_random{};

/**
 * @brief Common random iterable helper alias.
 * @tparam Arithmetic The arithmetic type of the random numbers.
 * @tparam Distribution The distribution type used to generate the random numbers.
 * @tparam Generator The random number generator type.
 * ```cpp
 * std::poisson_distribution<> distribution(0, 1);
 * std::mt19937 generator;
 * using iterable = lz::common_random_iterable<int, std::poisson_distribution<>, std::mt19937>;
 * iterable = lz::common_random(distribution, generator, 10);
 * ```
 */
template<class Arithmetic, class Distribution, class Generator>
using common_random_iterable = detail::random_iterable<Arithmetic, Distribution, Generator, false>;

/**
 * @brief Random iterable helper alias.
 * @tparam Arithmetic The arithmetic type of the random numbers.
 * @tparam Distribution The distribution type used to generate the random numbers.
 * @tparam Generator The random number generator type.
 * ```cpp
 * std::poisson_distribution<> distribution(0, 1);
 * std::mt19937 generator;
 * using iterable = lz::random_iterable<int, std::poisson_distribution<>, std::mt19937>;
 * iterable = lz::random(distribution, generator, 10);
 * ```
 */
template<class Arithmetic, class Distribution, class Generator>
using random_iterable = detail::random_iterable<Arithmetic, Distribution, Generator, true>;

/**
 * @brief The default random iterable helper alias. Uses std::mt19937 as the default generator and std::uniform_*_distribution as
 * the distribution.
 *
 * @tparam Arithmetic The arithmetic type of the random numbers.
 * ```cpp
 * lz::default_random_iterable<double> random = lz::random(0., 1., 10);
 * lz::default_random_iterable<int> random = lz::random(0, 10, 10);
 * ```
 */
template<class Arithmetic>
using default_random_iterable = decltype(std::declval<detail::random_adaptor<true>>()(Arithmetic{}, Arithmetic{}, size_t{}));

/**
 * @brief The default common random iterable helper alias. Uses std::mt19937 as the default generator and
 * std::uniform_*_distribution as the distribution.
 *
 * @tparam Arithmetic The arithmetic type of the random numbers.
 * ```cpp
 * lz::default_random_iterable<double> random = lz::common_random(0., 1., 10);
 * lz::default_random_iterable<int> random = lz::common_random(0, 10, 10);
 * ```
 */
template<class Arithmetic>
using common_default_random_iterable =
    decltype(std::declval<detail::random_adaptor<false>>()(Arithmetic{}, Arithmetic{}, size_t{}));

} // namespace lz


#endif