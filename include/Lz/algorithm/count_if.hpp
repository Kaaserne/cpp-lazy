#pragma once

#ifndef LZ_ALGORITHM_COUNT_IF_HPP
#define LZ_ALGORITHM_COUNT_IF_HPP

#include <Lz/detail/algorithm/count_if.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::diff_type<Iterator> count_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    if constexpr (detail::is_sentinel_v<Iterator, S>) {
        return detail::algorithm::count_if(std::move(begin), std::move(end), std::move(unary_predicate));
    }
    else {
        return std::count_if(std::move(begin), std::move(end), std::move(unary_predicate));
    }
}

/**
 * @brief Counts the amount of elements in the range [begin(iterable), end(iterable)) that satisfy the unary_predicate @p
 * unary_predicate
 *
 * @param iterable The iterable to count
 * @param unary_predicate The unary_predicate to count the elements with
 * @return The amount of elements that satisfy the unary_predicate
 */
template<class Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::diff_iterable_t<Iterable>
count_if(Iterable&& iterable, UnaryPredicate&& unary_predicate) {
    return lz::count_if(detail::begin(iterable), detail::end(iterable), std::forward<UnaryPredicate>(unary_predicate));
}

} // namespace lz

#endif // LZ_ALGORITHM_COUNT_IF_HPP
