#pragma once

#ifndef LZ_TAKE_HPP
#define LZ_TAKE_HPP

#include <Lz/detail/adaptors/take.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to take the first n elements of an iterable. Its end() function will return a sentinel, if the
 * input iterable has a forward iterator. Has a .size() method that essentially returns `n`.
 * ```cpp
 * auto vec = std::vector<int>{1, 2, 3, 4, 5};
 * auto res = lz::take(vec, 2); // res = {1, 2}
 * // or
 * auto res = vec | lz::take(2); // res = {1, 2}
 * ```
 */
static const detail::take_adaptor take{};

#else

/**
 * @brief This adaptor is used to take the first n elements of an iterable. The iterator category is the same as the input
 * iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator. Has a .size()
 * method that essentially returns `n`. Example:
 * ```cpp
 * auto vec = std::vector<int>{1, 2, 3, 4, 5};
 * auto res = lz::take(vec, 2); // res = {1, 2}
 * // or
 * auto res = vec | lz::take(2); // res = {1, 2}
 * ```
 */
LZ_INLINE_VAR constexpr detail::take_adaptor take{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif