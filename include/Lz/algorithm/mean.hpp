#pragma once

#ifndef LZ_ALGORITHM_MEAN_HPP
#define LZ_ALGORITHM_MEAN_HPP

#include <Lz/detail/algorithm/mean.hpp>
#include <Lz/detail/procs/operators.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * Gets the mean of a sequence.
 * @param iterable The iterable to calculate the mean of.
 * @param binary_op The binary operator to calculate the mean with.
 * @return The mean of the container.
 */
template<class Iterable, class BinaryOp = LZ_BIN_OP(plus, detail::val_iterable_t<Iterable>)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 double mean(Iterable && iterable, BinaryOp binary_op = {}) {
    return detail::mean(detail::begin(iterable), detail::end(iterable), std::move(binary_op));
}

} // namespace lz
#endif // LZ_ALGORITHM_MEAN_HPP
