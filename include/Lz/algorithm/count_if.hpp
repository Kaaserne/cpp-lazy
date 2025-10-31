#pragma once

#ifndef LZ_ALGORITHM_COUNT_IF_HPP
#define LZ_ALGORITHM_COUNT_IF_HPP

#include <Lz/detail/algorithm/count_if.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Counts the amount of elements in the range [begin(iterable), end(iterable)) that satisfy the unary_predicate @p
 * unary_predicate
 *
 * @param iterable The iterable to count
 * @param unary_predicate The unary_predicate to count the elements with
 * @return The amount of elements that satisfy the unary_predicate
 */
template<class Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::diff_iterable_t<Iterable>
count_if(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return detail::count_if(detail::begin(iterable), detail::end(iterable), std::move(unary_predicate));
}

} // namespace lz

#endif // LZ_ALGORITHM_COUNT_IF_HPP
