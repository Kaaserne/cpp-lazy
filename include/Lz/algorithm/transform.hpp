#pragma once

#ifndef LZ_ALGORITHM_TRANSFORM_HPP
#define LZ_ALGORITHM_TRANSFORM_HPP

#include <Lz/detail/algorithm/transform.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Applies the given unary operation to the elements in the range [begin(iterable), end(iterable)) and writes the
 * results in the range starting at @p output.
 * @param iterable The iterable to transform
 * @param output The output iterator to write the transformed elements to
 * @param unary_op The unary operation to apply to each element
 */
template<class Iterable, class OutputIterator, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void transform(Iterable&& iterable, OutputIterator output, UnaryOp unary_op) {
    detail::transform(detail::begin(iterable), detail::end(iterable), std::move(output), std::move(unary_op));
}

} // namespace lz

#endif // LZ_ALGORITHM_TRANSFORM_HPP
