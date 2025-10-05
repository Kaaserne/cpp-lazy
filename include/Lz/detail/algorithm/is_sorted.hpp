#pragma once

#ifndef LZ_DETAIL_ALGORITHM_IS_SORTED_HPP
#define LZ_DETAIL_ALGORITHM_IS_SORTED_HPP

#include <Lz/detail/traits/iterator_categories.hpp>

namespace lz {
namespace detail {
namespace algorithm {

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 bool is_sorted(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if constexpr (is_ra_v<Iterator>) {
        auto last = begin + (end - begin);
        return std::is_sorted(std::move(begin), std::move(last), std::move(binary_predicate));
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

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif
