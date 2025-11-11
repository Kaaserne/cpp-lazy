#pragma once

#ifndef LZ_DETAIL_TRAITS_IS_INVOCABLE_HPP
#define LZ_DETAIL_TRAITS_IS_INVOCABLE_HPP

#include <Lz/detail/compiler_config.hpp>
#include <type_traits>

#if !defined(LZ_HAS_CXX_17)

#include <Lz/detail/traits/conditional.hpp>
#include <Lz/detail/traits/void.hpp>
#include <utility>

#endif

namespace lz {
namespace detail {

#if defined(LZ_HAS_CXX_17)

template<class Function, class... Args>
using is_invocable = std::is_invocable<Function, Args...>;

#else

template<class Function, class = void>
struct is_invocable_impl_no_args : std::false_type {};

template<class Function>
struct is_invocable_impl_no_args<Function, void_t<decltype(std::declval<Function>()())>> : std::true_type {};

template<class, class Function, class...>
struct is_invocable_impl_n_args : std::false_type {};

template<class Function, class... Args>
struct is_invocable_impl_n_args<void_t<decltype(std::declval<Function>()(std::declval<Args>()...))>, Function, Args...>
    : std::true_type {};

template<class F, class... Args>
using is_invocable =
    conditional_t<sizeof...(Args) == 0, is_invocable_impl_no_args<F>, is_invocable_impl_n_args<void, F, Args...>>;

#endif // LZ_HAS_CXX_17

} // namespace detail
} // namespace lz
#endif // LZ_DETAIL_TRAITS_IS_INVOCABLE_HPP
