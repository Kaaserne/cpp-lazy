#pragma once

#ifndef LZ_TRAITS_IS_SIZED_HPP
#define LZ_TRAITS_IS_SIZED_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits/is_sized.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Helper to check whether a type is sized i.e. contains a .size() method. Example:
 * ```cpp
 * std::vector<int> v;
 * static_assert(lz::is_sized<decltype(v)>::value, "Vector is sized");
 *
 * auto not_sized = lz::c_string("Hello");
 * static_assert(!lz::is_sized<decltype(not_sized)>::value, "C string is not sized"); // C strings are not sized
 * ```
 */
using detail::is_sized;

#ifdef LZ_HAS_CXX_17
/**
 * @brief Helper to check whether a type is sized i.e. contains a .size() method. Example:
 * ```cpp
 * std::vector<int> v;
 * static_assert(lz::is_sized_v<decltype(v)>, "Vector is sized");
 *
 * auto not_sized = lz::c_string("Hello");
 * static_assert(!lz::is_sized_v<decltype(not_sized)>, "C string is not sized"); // C strings are not sized
 * ```
 */
using detail::is_sized_v;
#endif
} // namespace lz

#endif
