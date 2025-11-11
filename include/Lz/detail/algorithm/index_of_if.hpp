#pragma once

#ifndef LZ_DETAIL_ALGORITHM_INDEX_OF_IF_HPP
#define LZ_DETAIL_ALGORITHM_INDEX_OF_IF_HPP

#include <Lz/algorithm/find_if.hpp>
#include <Lz/algorithm/npos.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

namespace lz {
namespace detail {

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 size_t index_of_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    for (size_t index = 0; begin != end; ++begin, ++index) {
        if (unary_predicate(*begin)) {
            return index;
        }
    }
    return npos;
}

} // namespace detail
} // namespace lz

#endif
