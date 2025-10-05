#pragma once

#ifndef LZ_DETAIL_ALGORITHM_FOR_EACH_WHILE_N_HPP
#define LZ_DETAIL_ALGORITHM_FOR_EACH_WHILE_N_HPP

#include <Lz/detail/compiler_checks.hpp>

namespace lz {
namespace detail {
namespace algorithm {

template<class Iterator, class S, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void for_each_while_n(Iterator begin, S end, size_t n, UnaryOp unary_op) {
    for (size_t count = 0; begin != end && count != n; ++begin, ++count) {
        if (!unary_op(*begin)) {
            break;
        }
    }
}

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif
