#pragma once

#ifndef LZ_DETAIL_ALGORITHM_PARTITION_HPP
#define LZ_DETAIL_ALGORITHM_PARTITION_HPP

#include <Lz/algorithm/find_if_not.hpp>
#include <Lz/detail/compiler_checks.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {
namespace algorithm {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class UnaryPredicate>
constexpr Iterator partition(Iterator begin, S end, UnaryPredicate unary_predicate) {
    if constexpr (is_ra_v<Iterator>) {
        auto last = begin + (end - begin);
        return std::partition(std::move(begin), std::move(last), std::move(unary_predicate));
    }
    else {
        begin = lz::find_if_not(begin, end, std::move(unary_predicate));
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
}

#else

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra<Iterator>::value, Iterator>
partition(Iterator begin, S end, UnaryPredicate unary_predicate) {
    auto last = begin + (end - begin);
    return std::partition(std::move(begin), std::move(last), std::move(unary_predicate));
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator>::value, Iterator>
partition(Iterator begin, S end, UnaryPredicate unary_predicate) {
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

#endif

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_ALGORITHM_PARTITION_HPP
