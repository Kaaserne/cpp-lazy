#pragma once

#ifndef LZ_DETAIL_ALGORITHM_COPY_HPP
#define LZ_DETAIL_ALGORITHM_COPY_HPP

#include <Lz/detail/procs/get_end.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/std_algo_compat.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class OutputIterator>
constexpr void copy(Iterator begin, S end, OutputIterator out) {
    if constexpr (std_algo_compat_v<Iterator, S>) {
        static_cast<void>(std::copy(begin, detail::get_end(begin, end), out));
    }
    else {
        for (; begin != end; ++begin, ++out) {
            *out = *begin;
        }
    }
}

#else

template<class Iterator, class S, class OutputIterator>
LZ_CONSTEXPR_CXX_14 enable_if_t<!std_algo_compat<Iterator, S>::value> copy(Iterator begin, S end, OutputIterator out) {
    for (; begin != end; ++begin, ++out) {
        *out = *begin;
    }
}

template<class Iterator, class S, class OutputIterator>
LZ_CONSTEXPR_CXX_14 enable_if_t<std_algo_compat<Iterator, S>::value> copy(Iterator begin, S end, OutputIterator out) {
    static_cast<void>(std::copy(begin, detail::get_end(begin, end), out));
}

#endif

} // namespace detail
} // namespace lz

#endif
