#pragma once

#ifndef LZ_RANGE_HPP
#define LZ_RANGE_HPP

#include <Lz/detail/adaptors/range.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

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
static const detail::range_adaptor range{};

#else

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

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif