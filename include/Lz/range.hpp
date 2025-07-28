#pragma once

#ifndef LZ_RANGE_HPP
#define LZ_RANGE_HPP

#include <Lz/detail/adaptors/range.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Creates n amount of numbers starting from 0 (or specified otherwise). It is a forward iterable. It contains a .size()
 * method and is random access. Example:
 * ```
 * // This will create a range from 0 to 4
 * auto range = lz::range(5); // {0, 1, 2, 3, 4} (T = int)
 * // or, with first param start, second param end, third param step
 * auto range = lz::range(0.0, 1.0, 0.1); // {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9} (T = double)
 * // so this will :
 * auto range = lz::range(0.0, 1.0, 0.2); // {0.0, 0.2, 0.4, 0.6, 0.8} (T = double)
 * ```
 */
LZ_INLINE_VAR constexpr detail::range_adaptor range{};

/**
 * @brief Range helper alias type for range iterables with step.
 * @tparam Arithmetic The arithmetic type to use for the range.
 * ```cpp
 * lz::stepwise_range_iterable<int> r = lz::range(0, 10, 1);
 * ```
 */
template<class Arithmetic>
using stepwise_range_iterable = detail::range_iterable<Arithmetic, true>;

/**
 * @brief Range helper alias type for range iterables without step.
 * @tparam Arithmetic The arithmetic type to use for the range.
 * ```cpp
 * lz::range_iterable<int> r = lz::range(0, 10);
 * ```
 */
template<class Arithmetic>
using range_iterable = detail::range_iterable<Arithmetic, false>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif