#pragma once

#ifndef LZ_ALGORITHM_FIND_LAST_OR_DEFAULT_IF_HPP
#define LZ_ALGORITHM_FIND_LAST_OR_DEFAULT_IF_HPP

#include <Lz/algorithm/find_last_if.hpp>

namespace lz {

template<class Iterator, class S, class UnaryPredicate, class U>
LZ_CONSTEXPR_CXX_14 detail::val_t<Iterator>
find_last_or_default_if(Iterator begin, S end, UnaryPredicate unary_predicate, U&& default_value) {
    auto pos = lz::find_last_if(std::move(begin), end, std::move(unary_predicate));
    return static_cast<detail::val_t<Iterator>>(pos == end ? std::forward<U>(default_value) : *pos);
}

/**
 * @brief Finds the last element in the range [begin(iterable), end(iterable)) that satisfies the unary_predicate @p
 * unary_predicate. If the element is found, it returns the value, otherwise it returns @p default_value
 * @param iterable The iterable to search.
 * @param unary_predicate The search unary_predicate in [begin(iterable), end(iterable)). Must return bool.
 * @param default_value The value to return when no element matches unary_predicate @p unary_predicate in [begin(iterable),
 * end(iterable)). Defaults to `Iterable::value_type()`
 * @return The value if it is found, otherwise @p default_value
 */
template<class Iterable, class UnaryPredicate, class U>
LZ_CONSTEXPR_CXX_14 detail::val_iterable_t<Iterable>
find_last_or_default_if(Iterable&& iterable, UnaryPredicate unary_predicate, U&& default_value) {
    return lz::find_last_or_default_if(detail::begin(iterable), detail::end(iterable), std::move(unary_predicate),
                                       std::forward<U>(default_value));
}
} // namespace lz
#endif
