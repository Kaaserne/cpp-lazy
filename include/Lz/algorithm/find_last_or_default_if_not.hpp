#pragma once

#ifndef LZ_ALGORITHM_FIND_LAST_OR_DEFAULT_IF_NOT_HPP
#define LZ_ALGORITHM_FIND_LAST_OR_DEFAULT_IF_NOT_HPP

#include <Lz/algorithm/find_last_or_default_if.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class UnaryPredicate, class U>
LZ_CONSTEXPR_CXX_14 detail::val_t<Iterator>
find_last_or_default_if_not(Iterator begin, S end, UnaryPredicate unary_predicate, U&& default_value) {
    return lz::find_last_or_default_if(
        std::move(begin), std::move(end), [&unary_predicate](detail::ref_t<Iterator> val) { return !unary_predicate(val); },
        std::forward<U>(default_value));
}

/**
 * @brief Find the last element in the iterable that does not satisfy the predicate. If no such element is found, return the
 * default value.
 *
 * @param iterable The iterable to search.
 * @param unary_predicate The predicate to test each element.
 * @param default_value The value to return if no element is found.
 * @return The found element or the default value.
 */
template<class Iterable, class UnaryPredicate, class U>
LZ_CONSTEXPR_CXX_14 detail::val_iterable_t<Iterable>
find_last_or_default_if_not(Iterable&& iterable, UnaryPredicate&& unary_predicate, U&& default_value) {
    return find_last_or_default_if_not(detail::begin(iterable), detail::end(iterable),
                                       std::forward<UnaryPredicate>(unary_predicate), std::forward<U>(default_value));
}

} // namespace lz

#endif // LZ_ALGORITHM_FIND_LAST_OR_DEFAULT_IF_NOT_HPP
