#pragma once

#ifndef LZ_DETAIL_TRAITS_STD_ALGO_COMPAT_HPP
#define LZ_DETAIL_TRAITS_STD_ALGO_COMPAT_HPP

#include <Lz/detail/traits/is_sentinel.hpp>
#include <type_traits>
#include <algorithm>

namespace lz {
namespace detail {

template<class I, class S>
using std_algo_compat = std::integral_constant<bool, !detail::is_sentinel<I, S>::value || is_ra<I>::value>;

template<class I1, class S1, class I2, class S2>
using std_algo_compat2 = std::integral_constant<bool, std_algo_compat<I1, S1>::value && std_algo_compat<I2, S2>::value>;

#ifdef LZ_HAS_CXX_17

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
constexpr bool std_equal_helper(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    return std::equal(begin, end, begin2, end2, std::move(binary_predicate));
}

template<class I, class S>
inline constexpr bool std_algo_compat_v = std_algo_compat<I, S>::value;

template<class I1, class S1, class I2, class S2>
inline constexpr bool std_algo_compat2_v = std_algo_compat2<I1, S1, I2, S2>::value;

#else

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 bool std_equal_helper(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
#ifdef LZ_HAS_CXX_14
    return std::equal(begin, end, begin2, end2, std::move(binary_predicate));
#else
    static_cast<void>(end2);
    return std::equal(begin, end, begin2, std::move(binary_predicate));
#endif
}

#endif

} // namespace detail
} // namespace lz

#endif
