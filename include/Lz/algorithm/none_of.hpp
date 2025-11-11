#pragma once

#ifndef LZ_ALGORITHM_NONE_OF_HPP
#define LZ_ALGORITHM_NONE_OF_HPP

#include <Lz/algorithm/any_of.hpp>

LZ_MODULE_EXPORT namespace lz {

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
    return !lz::any_of(std::forward<Iterable>(iterable), std::move(unary_predicate));
}

} // namespace lz
#endif // LZ_ALGORITHM_NONE_OF_HPP
