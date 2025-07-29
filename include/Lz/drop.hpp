#pragma once

#ifndef LZ_DROP_HPP
#define LZ_DROP_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/drop.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief This adaptor is used to drop the first n elements of an iterable. The iterator category is the same as the input
 * iterator category. Its end() function will return the same type as its input iterable. If its input iterable has a
 * .size() method, then this iterable will also have a .size() method. If the amount to drop is larger than the size, size will be
 * 0. Example:
 * ```cpp
 * auto vec = std::vector<int>{1, 2, 3, 4, 5};
 * auto res = lz::drop(vec, 2); // res = {3, 4, 5}
 * // or
 * auto res = vec | lz::drop(2); // res = {3, 4, 5}
 * auto res = lz::drop(vec, 50); // res = {}, size = 0
 * ```
 */
LZ_INLINE_VAR constexpr detail::drop_adaptor drop{};

/**
 * @brief This is a type alias for the `drop` iterable.
 * @tparam Iterable The iterable to drop elements from.
 * ```cpp
 * auto vec = std::vector<int>{1, 2, 3, 4, 5};
 * lz::drop_iterable<std::vector<int>> res = lz::drop(vec, 2);
 * ```
 */
template<class Iterable>
using drop_iterable = detail::drop_iterable<Iterable>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_DROP_HPP