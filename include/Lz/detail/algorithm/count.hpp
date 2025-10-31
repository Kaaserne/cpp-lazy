#pragma once

#ifndef LZ_DETAIL_ALGORITHM_COUNT_HPP
#define LZ_DETAIL_ALGORITHM_COUNT_HPP

#include <Lz/detail/traits/enable_if.hpp>
#include <Lz/detail/traits/std_algo_compat.hpp>
#include <algorithm>

namespace lz {
namespace detail {

template<class I, class S, class T>
LZ_CONSTEXPR_CXX_14 enable_if_t<!std_algo_compat<I, S>::value, diff_type<I>> count(I begin, S end, const T& value) {
    diff_type<I> count = 0;
    for (; begin != end; ++begin) {
        if (*begin == value) {
            ++count;
        }
    }
    return count;
}

template<class I, class S, class T>
LZ_CONSTEXPR_CXX_14 enable_if_t<std_algo_compat<I, S>::value, diff_type<I>> count(I begin, S end, const T& value) {
    return std::count(begin, detail::get_end(begin, end), std::move(value));
}

} // namespace detail
} // namespace lz

#endif
