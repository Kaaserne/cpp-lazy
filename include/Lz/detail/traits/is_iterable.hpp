#pragma once

#ifndef LZ_DETAIL_TRAITS_IS_ITERABLE_HPP
#define LZ_DETAIL_TRAITS_IS_ITERABLE_HPP

#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/traits/void.hpp>
#include <Lz/traits/iter_type.hpp>

namespace lz {
namespace detail {

template<class, class = void>
struct is_iterable : std::false_type {};

template<class T>
struct is_iterable<T, void_t<iter_t<T>, sentinel_t<T>>> : std::true_type {};

template<class T, size_t N>
struct is_iterable<T[N]> : std::true_type {};

#ifdef LZ_HAS_CXX_17

template<class T, class = void>
LZ_INLINE_VAR constexpr bool is_iterable_v = false;

template<class T>
LZ_INLINE_VAR constexpr bool is_iterable_v<T, void_t<iter_t<T>, sentinel_t<T>>> = true;

template<class T, size_t N>
LZ_INLINE_VAR constexpr bool is_iterable_v<T[N]> = true;

#endif

} // namespace detail
} // namespace lz

#endif
