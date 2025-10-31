#pragma once

#ifndef LZ_DETAIL_ALGORITHM_BACK_HPP
#define LZ_DETAIL_ALGORITHM_BACK_HPP

#include <Lz/detail/procs/assert.hpp>
#include <Lz/detail/procs/get_end.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S>
constexpr ref_t<Iterator> back(Iterator begin, S end) {
    LZ_ASSERT(begin != end, "Cannot get back of empty iterable");

    if constexpr (std_algo_compat_v<Iterator, S>) {
        auto last = detail::get_end(begin, end);
        return *--last;
    }
    else {
        auto prev = begin++;

        while (begin != end) {
            prev = begin;
            ++begin;
        }

        return *prev;
    }
}

#else

template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14 enable_if_t<std_algo_compat<Iterator, S>::value, ref_t<Iterator>> back(Iterator begin, S end) {
    LZ_ASSERT(begin != end, "Cannot get back of empty iterable");
    auto last = detail::get_end(begin, end);
    return *--last;
}

template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14 enable_if_t<!std_algo_compat<Iterator, S>::value, ref_t<Iterator>> back(Iterator begin, S end) {
    LZ_ASSERT(begin != end, "Cannot get back of empty iterable");

    auto prev = begin++;
    while (begin != end) {
        prev = begin;
        ++begin;
    }
    return *prev;
}

#endif

} // namespace detail
} // namespace lz

#endif
