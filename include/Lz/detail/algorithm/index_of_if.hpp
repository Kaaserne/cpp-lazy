#pragma once

#ifndef LZ_DETAIL_ALGORITHM_INDEX_OF_IF_HPP
#define LZ_DETAIL_ALGORITHM_INDEX_OF_IF_HPP

#include <Lz/algorithm/find_if.hpp>
#include <Lz/algorithm/npos.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

namespace lz {
namespace detail {
namespace algorithm {

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 size_t index_of_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    size_t index = 0;
    auto it = lz::find_if(std::move(begin), std::move(end), [&index, &unary_predicate](detail::ref_t<Iterator> val) {
        auto res = unary_predicate(val);
        ++index;
        return res;
    });
    return it == end ? npos : index - 1;
}

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif
