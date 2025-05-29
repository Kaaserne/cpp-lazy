#pragma once

#ifndef LZ_REVERSE_HPP
#define LZ_REVERSE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/reverse.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Reveres the iterable. Contains a size method if the iterable has a size method. Example:
 * ```cpp
 * std::vector<int> v = { 1, 2, 3, 4, 5 };
 * auto reversed = lz::reverse(v); // { 5, 4, 3, 2, 1 }
 * // or
 * auto reversed = v | lz::reverse; // { 5, 4, 3, 2, 1 }
 * ```
 */
LZ_INLINE_VAR constexpr detail::reverse_adaptor reverse{};


/**
 * @brief Reverse iterable helper alias.
 * @tparam Iterable The type of the iterable to reverse.
 * ```cpp
 * std::vector<int> v = { 1, 2, 3, 4, 5 };
 * lz::reverse_iterable<std::vector<int>> reversed = lz::reverse(v);
 * ```
 */
template<class Iterable>
using reverse_iterable = detail::reverse_iterable<Iterable>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_REVERSE_HPP
