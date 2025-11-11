#pragma once

#ifndef LZ_ALGORITHM_ACCUMULATE_HPP
#define LZ_ALGORITHM_ACCUMULATE_HPP

#include <Lz/detail/algorithm/accumulate.hpp>
#include <Lz/detail/procs/operators.hpp>
#include <Lz/detail/traits/is_iterable.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

#ifdef LZ_HAS_CXX_20
#include <numeric>
#else
#include <Lz/detail/traits/enable_if.hpp>
#endif

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Accumulates the values in the range [begin, end) using the binary operator @p unary_predicate
 *
 * @param iterable The iterable to accumulate
 * @param init The initial value to start accumulating with
 * @param unary_predicate The binary operator to accumulate the values with
 * @return The accumulated value
 */
template<class Iterable, class T, class UnaryPredicate = LZ_BIN_OP(plus, detail::val_iterable_t<Iterable>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 T accumulate(Iterable&& iterable, T init, UnaryPredicate unary_predicate = {}) {
    return detail::accumulate(detail::begin(iterable), detail::end(iterable), std::move(init), std::move(unary_predicate));
}

} // namespace lz

#endif
