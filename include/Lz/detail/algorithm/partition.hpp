#pragma once

#ifndef LZ_DETAIL_ALGORITHM_PARTITION_HPP
#define LZ_DETAIL_ALGORITHM_PARTITION_HPP

#include <Lz/algorithm/find_if_not.hpp>
#include <Lz/detail/compiler_config.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class UnaryPredicate>
constexpr Iterator partition(Iterator begin, S end, UnaryPredicate unary_predicate) {
    if constexpr (std_algo_compat_v<Iterator, S>) {
        return std::partition(begin, detail::get_end(begin, end), std::move(unary_predicate));
    }
    else {
        begin =
            detail::find_if(begin, end, [p = std::move(unary_predicate)](detail::ref_t<Iterator> value) { return !p(value); });
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
LZ_CONSTEXPR_CXX_14 enable_if_t<std_algo_compat<Iterator, S>::value, Iterator>
partition(Iterator begin, S end, UnaryPredicate unary_predicate) {
    return std::partition(begin, detail::get_end(begin, end), std::move(unary_predicate));
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!std_algo_compat<Iterator, S>::value, Iterator>
partition(Iterator begin, S end, UnaryPredicate unary_predicate) {
    begin = detail::find_if(begin, end, [&unary_predicate](detail::ref_t<Iterator> value) { return !unary_predicate(value); });
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

} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_ALGORITHM_PARTITION_HPP
