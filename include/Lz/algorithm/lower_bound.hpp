#pragma once

#ifndef LZ_ALGORITHM_LOWER_BOUND_HPP
#define LZ_ALGORITHM_LOWER_BOUND_HPP

#include <Lz/detail/algorithm/lower_bound.hpp>
#include <Lz/detail/procs/operators.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/detail/traits/is_iterable.hpp>
#include <Lz/procs/distance.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Searches for the first element in the partitioned range [begin(iterable), end(iterable)) which is not ordered before
 * @p value.
 *
 * @param iterable The iterable to check
 * @param value The value to check for
 * @param binary_predicate The to use when comparing the values
 * @return Iteartor to the first element that satisfies the value or `end(iterable)` if the element is not found
 */
template<class Iterable, class T, class BinaryPredicate = LZ_BIN_OP(less, detail::val_iterable_t<Iterable>)>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<detail::is_iterable<Iterable>::value, iter_t<Iterable>>
lower_bound(Iterable&& iterable, const T& value, BinaryPredicate binary_predicate = {}) {
    return detail::lower_bound(std::forward<Iterable>(iterable), value, std::move(binary_predicate));
}

} // namespace lz

#endif // LZ_ALGORITHM_LOWER_BOUND_HPP
