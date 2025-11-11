#pragma once

#ifndef LZ_ALGORITHM_ANY_OF_HPP
#define LZ_ALGORITHM_ANY_OF_HPP

#include <Lz/algorithm/find_if.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Checks whether any of the elements in the range [begin(iterable), end(iterable)) satisfy the unary_predicate @p
 * unary_predicate
 *
 * @param iterable The iterable to check
 * @param unary_predicate The unary_predicate to call whether any of the elements satisfy this condition
 * @return true if any of the elements satisfy the unary_predicate, false otherwise
 */
template<class Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool any_of(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return lz::find_if(std::forward<Iterable>(iterable), std::move(unary_predicate)) != detail::end(iterable);
}

} // namespace lz
#endif
