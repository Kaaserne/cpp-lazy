#pragma once

#ifndef LZ_ALGORITHM_MIN_ELEMENT_HPP
#define LZ_ALGORITHM_MIN_ELEMENT_HPP

#include <Lz/algorithm/max_element.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Finds the minimum element in the range [begin, end) using the binary binary_predicate @p binary_predicate
 *
 * @param iterable The iterable to find the minimum element in
 * @param binary_predicate The binary operator to find the minimum element with
 * @return The minimum element in the range or `end` if the range is empty
 */
template<class Iterable, class BinaryPredicate = LZ_BIN_OP(less, detail::val_iterable_t<Iterable>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> min_element(Iterable&& iterable, BinaryPredicate binary_predicate = {}) {
    return lz::max_element(std::forward<Iterable>(iterable),
                           [&binary_predicate](detail::ref_t<iter_t<Iterable>> a, detail::ref_t<iter_t<Iterable>> b) {
                               return !binary_predicate(a, b);
                           });
}

} // namespace lz

#endif
