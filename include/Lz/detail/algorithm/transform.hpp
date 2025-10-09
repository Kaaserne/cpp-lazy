#pragma once

#ifndef LZ_DETAIL_ALGORITHM_TRANSFORM_HPP
#define LZ_DETAIL_ALGORITHM_TRANSFORM_HPP

#include <Lz/detail/traits/iterator_categories.hpp>
#include <algorithm>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {
namespace algorithm {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class OutputIterator, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void transform(Iterator begin, S end, OutputIterator output, UnaryOp unary_op) {
    if constexpr (is_ra_v<Iterator>) {
        auto last = begin + (end - begin); // TODO make generic func for this
        static_cast<void>(std::transform(std::move(begin), std::move(last), std::move(output), std::move(unary_op)));
    }
    else {
        for (; begin != end; ++begin, ++output) {
            *output = unary_op(*begin);
        }
    }
}

#else

template<class Iterator, class S, class OutputIterator, class UnaryOp>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<is_ra<Iterator>::value>
transform(Iterator begin, S end, OutputIterator output, UnaryOp unary_op) {
    auto last = begin + (end - begin);
    static_cast<void>(std::transform(std::move(begin), std::move(last), std::move(output), std::move(unary_op)));
}

template<class Iterator, class S, class OutputIterator, class UnaryOp>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<!is_ra<Iterator>::value>
transform(Iterator begin, S end, OutputIterator output, UnaryOp unary_op) {
    for (; begin != end; ++begin, ++output) {
        *output = unary_op(*begin);
    }
}

#endif

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_ALGORITHM_TRANSFORM_HPP
