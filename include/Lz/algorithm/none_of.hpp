#pragma once

#ifndef LZ_ALGORITHM_NONE_OF_HPP
#define LZ_ALGORITHM_NONE_OF_HPP

#include <Lz/algorithm/any_of.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 bool none_of(Iterator iterator, S s, UnaryPredicate unary_predicate) {
    return !lz::any_of(std::move(iterator), std::move(s), std::move(unary_predicate));
}

/**
 * @brief Checks whether none of the elements in the range [begin(iterable), end(iterable)) satisfy the unary_predicate @p
 * unary_predicate
 *
 * @param iterable The iterable to check
 * @param unary_predicate The unary_predicate to call whether none of the elements satisfy this condition
 * @return true if none of the elements satisfy the unary_predicate, false otherwise
 */
template<class Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool none_of(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return lz::none_of(detail::begin(iterable), detail::end(iterable), std::move(unary_predicate));
}
} // namespace lz
#endif // LZ_ALGORITHM_NONE_OF_HPP
