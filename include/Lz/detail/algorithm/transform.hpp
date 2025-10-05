#pragma once

#ifndef LZ_DETAIL_ALGORITHM_TRANSFORM_HPP
#define LZ_DETAIL_ALGORITHM_TRANSFORM_HPP

#include <Lz/detail/traits/iterator_categories.hpp>
#include <algorithm>

namespace lz {
namespace detail {
namespace algorithm {

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

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_ALGORITHM_TRANSFORM_HPP
