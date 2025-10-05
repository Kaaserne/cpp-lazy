#pragma once

#ifndef LZ_DETAIL_TRAITS_IS_SENTINEL_HPP
#define LZ_DETAIL_TRAITS_IS_SENTINEL_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <type_traits>
#include <Lz/detail/procs/begin_end.hpp>

namespace lz {
namespace detail {

template<class I, class S>
using is_sentinel = std::integral_constant<bool, !std::is_same<I, S>::value>;

template<class I>
using has_sentinel = is_sentinel<decltype(detail::begin(std::declval<I&>())), decltype(detail::end(std::declval<I&>()))>;

#ifdef LZ_HAS_CXX_17

template<class I, class S>
LZ_INLINE_VAR constexpr bool is_sentinel_v = is_sentinel<I, S>::value;

template<class I>
LZ_INLINE_VAR constexpr bool has_sentinel_v = has_sentinel<I>::value;

#endif

} // namespace detail
} // namespace lz

#endif
