#pragma once

#ifndef LZ_DETAIL_ALGORITHM_MAX_ELEMENT_HPP
#define LZ_DETAIL_ALGORITHM_MAX_ELEMENT_HPP

#include <Lz/detail/procs/get_end.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <algorithm>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator max_element(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if constexpr (std_algo_compat_v<Iterator, S>) {
        return std::max_element(begin, detail::get_end(begin, end), std::move(binary_predicate));
    }
    else {
        if (begin == end) {
            return begin;
        }
        auto max = begin;
        for (++begin; begin != end; ++begin) {
            if (binary_predicate(*max, *begin)) {
                max = begin;
            }
        }
        return max;
    }
}

#else

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<std_algo_compat<Iterator, S>::value, Iterator>
max_element(Iterator begin, S end, BinaryPredicate binary_predicate) {
    return std::max_element(begin, detail::get_end(begin, end), std::move(binary_predicate));
}

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!std_algo_compat<Iterator, S>::value, Iterator>
max_element(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if (begin == end) {
        return begin;
    }
    auto max = begin;
    for (++begin; begin != end; ++begin) {
        if (binary_predicate(*max, *begin)) {
            max = begin;
        }
    }
    return max;
}

#endif

} // namespace detail
} // namespace lz

#endif
