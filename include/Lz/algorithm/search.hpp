#pragma once

#ifndef LZ_ALGORITHM_SEARCH_HPP
#define LZ_ALGORITHM_SEARCH_HPP

#include <Lz/detail/algorithm/search.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/procs/operators.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Searches for the first occurrence of the range [begin(iterable2), end(iterable2)) in the range [begin(iterable),
 * end(iterable))
 *
 * @param iterable The iterable to search in
 * @param iterable2 The iterable to search for
 * @param binary_predicate The binary predicate to search with
 * @return The iterator to the first occurrence of the range [begin(iterable2), end(iterable2)) in the range [begin(iterable),
 * end(iterable)) or `end(iterable)` if the range is not found
 */
template<class Iterable, class Iterable2, class BinaryPredicate = LZ_BIN_OP(equal_to, detail::val_iterable_t<Iterable>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::pair<iter_t<Iterable>, iter_t<Iterable>> search(
    Iterable && iterable, Iterable2 && iterable2, BinaryPredicate binary_predicate = {}) {
    return detail::search(detail::begin(iterable), detail::end(iterable), detail::begin(iterable2), detail::end(iterable2),
                          std::move(binary_predicate));
}

} // namespace lz

#endif
