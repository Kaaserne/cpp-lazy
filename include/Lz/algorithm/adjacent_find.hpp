#pragma once

#ifndef LZ_ALGORITHM_ADJACENT_FIND_HPP
#define LZ_ALGORITHM_ADJACENT_FIND_HPP

#include <Lz/detail/algorithm/adjacent_find.hpp>
#include <Lz/detail/procs/operators.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>
#include <Lz/traits/iter_type.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Finds the first occurrence of two adjacent elements in the range [begin(iterable), end(iterable)) that satisfy the
 * binary binary_predicate @p binary_predicate
 *
 * @param iterable The iterable to search in
 * @param binary_predicate The binary binary_predicate to search with
 * @return Iterator to the first occurrence of two adjacent elements that satisfy the binary binary_predicate or
 * `end(iterable)` if the elements are not found
 */
template<class Iterable, class BinaryPredicate = LZ_BIN_OP(equal_to, detail::val_iterable_t<Iterable>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> adjacent_find(Iterable && iterable, BinaryPredicate&& binary_predicate = {}) {
    return detail::adjacent_find(detail::begin(iterable), detail::end(iterable), std::forward<BinaryPredicate>(binary_predicate));
}

} // namespace lz

#endif
