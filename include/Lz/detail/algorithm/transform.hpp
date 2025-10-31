#pragma once

#ifndef LZ_DETAIL_ALGORITHM_TRANSFORM_HPP
#define LZ_DETAIL_ALGORITHM_TRANSFORM_HPP

#include <Lz/detail/procs/get_end.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <algorithm>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class OutputIterator, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void transform(Iterator begin, S end, OutputIterator output, UnaryOp unary_op) {
    if constexpr (std_algo_compat_v<Iterator>) {
        static_cast<void>(std::transform(begin, detail::get_end(begin, end), output, std::move(unary_op)));
    }
    else {
        for (; begin != end; ++begin, ++output) {
            *output = unary_op(*begin);
        }
    }
}

#else

template<class Iterator, class S, class OutputIterator, class UnaryOp>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<std_algo_compat<Iterator, S>::value>
transform(Iterator begin, S end, OutputIterator output, UnaryOp unary_op) {
    static_cast<void>(std::transform(begin, detail::get_end(begin, end), output, std::move(unary_op)));
}

template<class Iterator, class S, class OutputIterator, class UnaryOp>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<!std_algo_compat<Iterator, S>::value>
transform(Iterator begin, S end, OutputIterator output, UnaryOp unary_op) {
    for (; begin != end; ++begin, ++output) {
        *output = unary_op(*begin);
    }
}

#endif

} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_ALGORITHM_TRANSFORM_HPP
