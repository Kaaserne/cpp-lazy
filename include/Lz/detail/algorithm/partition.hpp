#pragma once

#ifndef LZ_DETAIL_ALGORITHM_PARTITION_HPP
#define LZ_DETAIL_ALGORITHM_PARTITION_HPP

#include <Lz/algorithm/find_if_not.hpp>
#include <Lz/detail/compiler_checks.hpp>

namespace lz {
namespace detail {
namespace algorithm {

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator partition(Iterator begin, S end, UnaryPredicate unary_predicate) {
    begin = lz::find_if_not(begin, end, unary_predicate);
    if (begin == end) {
        return begin;
    }

    for (auto i = std::next(begin); i != end; ++i) {
        if (!unary_predicate(*i)) {
            continue;
        }
        std::swap(*i, *begin);
        ++begin;
    }

    return begin;
}

} // namespace algorithm
} // namespace detail
} // namespace lz
#endif // LZ_DETAIL_ALGORITHM_PARTITION_HPP
