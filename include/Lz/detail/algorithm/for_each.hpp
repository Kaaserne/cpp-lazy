#pragma once

#ifndef LZ_DETAIL_ALGORITHM_FOR_EACH_HPP
#define LZ_DETAIL_ALGORITHM_FOR_EACH_HPP

#include <Lz/detail/traits/iterator_categories.hpp>
#include <algorithm>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {
namespace algorithm {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class Func>
constexpr void for_each(Iterator begin, S end, Func func) {
    if constexpr (is_ra_v<Iterator>) {
        auto last = begin + (end - begin);
        static_cast<void>(std::for_each(std::move(begin), std::move(last), std::move(func)));
    }
    else {
        for (; begin != end; ++begin) {
            func(*begin);
        }
    }
}

#else

template<class Iterator, class S, class Func>
enable_if_t<is_ra<Iterator>::value> for_each(Iterator begin, S end, Func func) {
    auto last = begin + (end - begin);
    static_cast<void>(std::for_each(std::move(begin), std::move(last), std::move(func)));
}

template<class Iterator, class S, class Func>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator>::value> for_each(Iterator begin, S end, Func func) {
    for (; begin != end; ++begin) {
        func(*begin);
    }
}

#endif

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_ALGORITHM_FOR_EACH_HPP
