#pragma once

#ifndef LZ_DETAIL_ALGORITHM_COPY_HPP
#define LZ_DETAIL_ALGORITHM_COPY_HPP

#include <Lz/detail/traits/iterator_categories.hpp>

namespace lz {
namespace detail {
namespace algorithm {

template<class Iterator, class S, class OutputIterator>
void copy(Iterator begin, S end, OutputIterator out) {
    if constexpr (is_ra_v<Iterator>) {
        auto last = begin + (end - begin);
        static_cast<void>(std::copy(begin, last, out));
    }
    else {
        for (; begin != end; ++begin, ++out) {
            *out = *begin;
        }
    }
}

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif
