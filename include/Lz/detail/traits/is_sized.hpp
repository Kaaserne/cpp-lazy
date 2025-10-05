#pragma once

#ifndef LZ_DETAIL_TRAITS_IS_SIZED_HPP
#define LZ_DETAIL_TRAITS_IS_SIZED_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits/void.hpp>
#include <Lz/procs/size.hpp>

namespace lz {
namespace detail {

LZ_MODULE_EXPORT template<class T, class = void>
struct is_sized : std::false_type {};

LZ_MODULE_EXPORT template<class T>
struct is_sized<T, void_t<decltype(lz::size(std::declval<T>()))>> : std::true_type {};

#ifdef LZ_HAS_CXX_17

LZ_MODULE_EXPORT template<class T, class = void>
LZ_INLINE_VAR constexpr bool is_sized_v = false;

LZ_MODULE_EXPORT template<class T>
LZ_INLINE_VAR constexpr bool is_sized_v<T, void_t<decltype(lz::size(std::declval<T>()))>> = true;

#endif // LZ_HAS_CXX_17

} // namespace detail
} // namespace lz

#endif
