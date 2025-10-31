#pragma once

#ifndef LZ_DETAIL_ALGORITHM_IS_SORTED_HPP
#define LZ_DETAIL_ALGORITHM_IS_SORTED_HPP

#include <Lz/detail/traits/iterator_categories.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif
// TODO remove algorithm:: namespace
namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 bool is_sorted(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if constexpr (std_algo_compat_v<Iterator, S>) {
        return std::is_sorted(begin, detail::get_end(begin, end), std::move(binary_predicate));
    }
    else {
        if (begin == end) {
            return true;
        }
        auto next = begin;
        for (++next; next != end; ++begin, ++next) {
            if (binary_predicate(*next, *begin)) {
                return false;
            }
        }
        return true;
    }
}

#else

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<std_algo_compat<Iterator, S>::value, bool>
is_sorted(Iterator begin, S end, BinaryPredicate binary_predicate) {
    return std::is_sorted(begin, detail::get_end(begin, end), std::move(binary_predicate));
}

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!std_algo_compat<Iterator, S>::value, bool>
is_sorted(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if (begin == end) {
        return true;
    }
    auto next = begin;
    for (++next; next != end; ++begin, ++next) {
        if (binary_predicate(*next, *begin)) {
            return false;
        }
    }
    return true;
}

#endif

} // namespace detail
} // namespace lz

#endif
