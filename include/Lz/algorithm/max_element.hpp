#pragma once

#ifndef LZ_ALGORITHM_MAX_ELEMENT_HPP
#define LZ_ALGORITHM_MAX_ELEMENT_HPP

#include <Lz/detail/algorithm/max_element.hpp>
#include <Lz/detail/procs/operators.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Finds the maximum element in the range [begin, end) using the binary binary_predicate @p binary_predicate
 * @param iterable The iterable to find the maximum element in
 * @param binary_predicate The binary operator to find the maximum element with
 * @return The maximum element in the range, if the range is empty, the return value is `end`
 */
template<class Iterable, class BinaryPredicate = LZ_BIN_OP(less, detail::val_iterable_t<Iterable>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> max_element(Iterable&& iterable, BinaryPredicate binary_predicate = {}) {
    return detail::max_element(detail::begin(iterable), detail::end(iterable), std::move(binary_predicate));
}
} // namespace lz

#endif
