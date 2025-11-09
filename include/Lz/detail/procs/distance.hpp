#pragma once

#ifndef LZ_DETAIL_PROCS_DISTANCE_HPP
#define LZ_DETAIL_PROCS_DISTANCE_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S>
[[nodiscard]] constexpr diff_type<Iterator> distance_impl(Iterator begin, S end) {
    if constexpr (is_ra_v<Iterator>) {
        return end - begin;
    }
    else {
        diff_type<Iterator> dist = 0;
        for (; begin != end; ++begin, ++dist) {
        }
        return dist;
    }
}

#else

template<class Iterator, class S>
LZ_NODISCARD constexpr enable_if_t<is_ra<Iterator>::value, diff_type<Iterator>> distance_impl(Iterator begin, S end) {
    return end - begin;
}

template<class Iterator, class S>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator>::value, diff_type<Iterator>> distance_impl(Iterator begin, S end) {
    diff_type<Iterator> dist = 0;
    for (; begin != end; ++begin, ++dist) {
    }
    return dist;
}

#endif

} // namespace detail
} // namespace lz

#endif
