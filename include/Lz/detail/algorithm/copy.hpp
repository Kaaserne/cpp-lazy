#pragma once

#ifndef LZ_DETAIL_ALGORITHM_COPY_HPP
#define LZ_DETAIL_ALGORITHM_COPY_HPP

#include <Lz/detail/traits/iterator_categories.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {
namespace algorithm {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class OutputIterator>
constexpr void copy(Iterator begin, S end, OutputIterator out) {
    if constexpr (is_ra_v<Iterator>) {
        auto last = begin + (end - begin);
        static_cast<void>(std::copy(std::move(begin), std::move(end), std::move(out)));
    }
    else {
        for (; begin != end; ++begin, ++out) {
            *out = *begin;
        }
    }
}

#else

template<class Iterator, class S, class OutputIterator>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator>::value> copy(Iterator begin, S end, OutputIterator out) {
    for (; begin != end; ++begin, ++out) {
        *out = *begin;
    }
}

template<class Iterator, class S, class OutputIterator>
LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra<Iterator>::value> copy(Iterator begin, S end, OutputIterator out) {
    auto last = begin + (end - begin);
    static_cast<void>(std::copy(std::move(begin), std::move(last), std::move(out)));
}

#endif

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif
