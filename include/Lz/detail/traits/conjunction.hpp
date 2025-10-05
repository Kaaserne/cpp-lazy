#pragma once

#ifndef LZ_DETAIL_TRAITS_CONJUNCTION_HPP
#define LZ_DETAIL_TRAITS_CONJUNCTION_HPP

#include <Lz/detail/traits/conditional.hpp>
#include <type_traits>

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class... Ts>
struct conjunction : std::bool_constant<(Ts::value && ...)> {};

#else

template<class... Ts>
struct conjunction : std::true_type {};

template<class T, class... Ts>
struct conjunction<T, Ts...> : conditional_t<T::value, conjunction<Ts...>, std::false_type> {};

#endif

} // namespace detail
} // namespace lz

#endif
