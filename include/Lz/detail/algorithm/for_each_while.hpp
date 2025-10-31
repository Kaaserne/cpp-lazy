#pragma once

#ifndef LZ_DETAIL_ALGORITHM_FOR_EACH_WHILE_HPP
#define LZ_DETAIL_ALGORITHM_FOR_EACH_WHILE_HPP

#include <Lz/detail/compiler_checks.hpp>

namespace lz {
namespace detail {

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 void for_each_while(Iterator begin, S end, UnaryPredicate unary_predicate) {
    for (; begin != end; ++begin) {
        if (!unary_predicate(*begin)) {
            break;
        }
    }
}

} // namespace detail
} // namespace lz

#endif
