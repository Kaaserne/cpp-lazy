#pragma once

#ifndef LZ_DETAIL_ALGORITHM_FOR_EACH_HPP
#define LZ_DETAIL_ALGORITHM_FOR_EACH_HPP

#include <Lz/detail/traits/iterator_categories.hpp>
#include <algorithm>

namespace lz {
namespace detail {
namespace algorithm {

template<class Iterator, class S, class Func>
void for_each(Iterator begin, S end, Func func) {
    if constexpr (is_ra_v<Iterator>) {
        auto last = begin + (end - begin);
        std::for_each(begin, last, func);
    }
    else {
        for (; begin != end; ++begin) {
            func(*begin);
        }
    }
}

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_ALGORITHM_FOR_EACH_HPP
