#pragma once

#ifndef LZ_DETAIL_ALGORITHM_FOR_EACH_HPP
#define LZ_DETAIL_ALGORITHM_FOR_EACH_HPP

#include <Lz/detail/procs/get_end.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/std_algo_compat.hpp>
#include <algorithm>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class Func>
constexpr void for_each(Iterator begin, S end, Func func) {
    if constexpr (std_algo_compat_v<Iterator, S>) {
        static_cast<void>(std::for_each(begin, detail::get_end(begin, end), std::move(func)));
    }
    else {
        for (; begin != end; ++begin) {
            func(*begin);
        }
    }
}

#else

template<class Iterator, class S, class Func>
enable_if_t<std_algo_compat<Iterator, S>::value> for_each(Iterator begin, S end, Func func) {
    static_cast<void>(std::for_each(begin, detail::get_end(begin, end), std::move(func)));
}

template<class Iterator, class S, class Func>
LZ_CONSTEXPR_CXX_14 enable_if_t<!std_algo_compat<Iterator, S>::value> for_each(Iterator begin, S end, Func func) {
    for (; begin != end; ++begin) {
        func(*begin);
    }
}

#endif

} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_ALGORITHM_FOR_EACH_HPP
