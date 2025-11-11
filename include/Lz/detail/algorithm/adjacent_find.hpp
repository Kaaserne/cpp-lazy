#pragma once

#ifndef LZ_DETAIL_ALGORITHM_ADJACENT_FIND_HPP
#define LZ_DETAIL_ALGORITHM_ADJACENT_FIND_HPP

#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/procs/get_end.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/std_algo_compat.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

#include <algorithm>

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class BinaryPredicate>
constexpr Iterator adjacent_find(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if constexpr (std_algo_compat_v<Iterator, S>) {
        return std::adjacent_find(begin, detail::get_end(begin, end), std::move(binary_predicate));
    }
    else {
        if (begin == end) {
            return begin;
        }
        auto next = begin;
        for (++next; next != end; ++begin, ++next) {
            if (binary_predicate(*begin, *next)) {
                return begin;
            }
        }
        return next;
    }
}

#else

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!std_algo_compat<Iterator, S>::value, Iterator>
adjacent_find(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if (begin == end) {
        return begin;
    }

    auto next = begin;
    for (++next; next != end; ++begin, ++next) {
        if (binary_predicate(*begin, *next)) {
            return begin;
        }
    }
    return next;
}

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<std_algo_compat<Iterator, S>::value, Iterator>
adjacent_find(Iterator begin, S end, BinaryPredicate binary_predicate) {
    return std::adjacent_find(begin, detail::get_end(begin, end), std::move(binary_predicate));
}

#endif

} // namespace detail
} // namespace lz

#endif
