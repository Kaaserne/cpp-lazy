#pragma once

#ifndef LZ_ALGORITHM_IS_SORTED_HPP
#define LZ_ALGORITHM_IS_SORTED_HPP

#include <Lz/detail/algorithm/is_sorted.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/procs/begin_end.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) is sorted using the binary binary_predicate @p
 * binary_predicate
 *
 * @param iterable The iterable to check
 * @param binary_predicate The binary binary_predicate to check the range with
 * @return true if the range is sorted, false otherwise
 */
template<class Iterable, class BinaryPredicate = LZ_BIN_OP(less, detail::val_iterable_t<Iterable>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool is_sorted(Iterable && iterable, BinaryPredicate binary_predicate = {}) {
    return detail::is_sorted(detail::begin(iterable), detail::end(iterable), std::move(binary_predicate));
}

} // namespace lz

#endif
