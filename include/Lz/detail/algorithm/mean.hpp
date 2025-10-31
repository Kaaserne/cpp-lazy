#pragma once

#ifndef LZ_DETAIL_ALGORITHM_MEAN_HPP
#define LZ_DETAIL_ALGORITHM_MEAN_HPP

#include <Lz/detail/algorithm/accumulate.hpp>
#include <Lz/detail/traits/remove_ref.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

namespace lz {
namespace detail {
// TODO if constexpr
template<class Iterator, class S, class BinaryOp>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator>::value, double> mean(Iterator begin, S end, BinaryOp binary_op) {
    using T = remove_cvref_t<decltype(binary_op(*begin, *begin))>;

    diff_type<Iterator> distance{ 0 };
    T sum{};

    for (; begin != end; ++begin, ++distance) {
        sum = binary_op(std::move(sum), *begin);
    }
    if (distance == 0) {
        return 0;
    }

    return static_cast<double>(sum) / static_cast<double>(distance);
}

template<class Iterator, class S, class BinaryOp>
LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra<Iterator>::value, double> mean(Iterator begin, S end, BinaryOp binary_op) {
    auto len = end - begin;
    return std::accumulate(begin, begin + len, val_t<Iterator>{}, std::move(binary_op)) / static_cast<double>(len);
}

} // namespace detail
} // namespace lz
#endif // LZ_DETAIL_ALGORITHM_MEAN_HPP
