#pragma once

#ifndef LZ_ALGORITHM_FIND_IF_NOT_HPP
#define LZ_ALGORITHM_FIND_IF_NOT_HPP

#include <Lz/algorithm/find_if.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that does not satisfy the unary_predicate @p
 * unary_predicate
 *
 * @param iterable The iterable to find the element in
 * @param unary_predicate The unary_predicate to find the element with
 * @return Iterator to the first element that does not satisfy the unary_predicate or `end(iterable)` if the element is not
 * found
 */
template<class Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> find_if_not(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return lz::find_if(std::forward<Iterable>(iterable),
                       [&unary_predicate](detail::ref_t<iter_t<Iterable>> value) { return !unary_predicate(value); });
}

} // namespace lz

#endif // LZ_ALGORITHM_FIND_IF_NOT_HPP
