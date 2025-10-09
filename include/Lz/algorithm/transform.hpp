#pragma once

#ifndef LZ_ALGORITHM_TRANSFORM_HPP
#define LZ_ALGORITHM_TRANSFORM_HPP

#include <Lz/detail/algorithm/transform.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class OutputIterator, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void transform(Iterator begin, S end, OutputIterator output, UnaryOp unary_op) {
    if constexpr (detail::is_sentinel_v<Iterator, S>) {
        detail::algorithm::transform(std::move(begin), std::move(end), std::move(output), std::move(unary_op));
    }
    else {
        static_cast<void>(std::transform(std::move(begin), std::move(end), std::move(output), std::move(unary_op)));
    }
}

#else

template<class Iterator, class S, class OutputIterator, class UnaryOp>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<detail::is_sentinel<Iterator, S>::value>
transform(Iterator begin, S end, OutputIterator output, UnaryOp unary_op) {
    detail::algorithm::transform(std::move(begin), std::move(end), std::move(output), std::move(unary_op));
}

template<class Iterator, class S, class OutputIterator, class UnaryOp>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<!detail::is_sentinel<Iterator, S>::value>
transform(Iterator begin, S end, OutputIterator output, UnaryOp unary_op) {
    static_cast<void>(std::transform(std::move(begin), std::move(end), std::move(output), std::move(unary_op)));
}

#endif

/**
 * @brief Applies the given unary operation to the elements in the range [begin(iterable), end(iterable)) and writes the
 * results in the range starting at @p output.
 * @param iterable The iterable to transform
 * @param output The output iterator to write the transformed elements to
 * @param unary_op The unary operation to apply to each element
 */
template<class Iterable, class OutputIterator, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void transform(Iterable&& iterable, OutputIterator output, UnaryOp unary_op) {
    lz::transform(detail::begin(iterable), detail::end(iterable), std::move(output), std::move(unary_op));
}

} // namespace lz

#endif // LZ_ALGORITHM_TRANSFORM_HPP
