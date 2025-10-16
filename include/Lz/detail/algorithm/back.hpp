#pragma once

#ifndef LZ_DETAIL_ALGORITHM_BACK_HPP
#define LZ_DETAIL_ALGORITHM_BACK_HPP

#include <Lz/detail/procs/assert.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {
namespace algorithm {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S>
constexpr ref_t<Iterator> back(Iterator begin, S end) {
    LZ_ASSERT(begin != end, "Cannot get back of empty iterable");

    if constexpr (is_ra_v<Iterator> && is_sentinel_v<Iterator, S>) {
        return *(begin + (end - begin) - 1);
    }
    else if constexpr (is_bidi_v<Iterator> && !is_sentinel_v<Iterator, S>) {
        return *--end;
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
LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra<Iterator>::value && is_sentinel<Iterator, S>::value, ref_t<Iterator>> back(Iterator begin, S end) {
    LZ_ASSERT(begin != end, "Cannot get back of empty iterable");
    return *(begin + (end - begin) - 1);
}

template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14
    enable_if_t<is_bidi<Iterator>::value && !is_sentinel<Iterator, S>::value, ref_t<Iterator>>
    back(Iterator begin, S end) {
    LZ_ASSERT(begin != end, "Cannot get back of empty iterable");
    return *--end;
}

template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_bidi<Iterator>::value ||
                                    (!is_ra<Iterator>::value && is_bidi<Iterator>::value && is_sentinel<Iterator, S>::value),
                                ref_t<Iterator>>
back(Iterator begin, S end) {
    LZ_ASSERT(begin != end, "Cannot get back of empty iterable");

    auto prev = begin++;
    while (begin != end) {
        prev = begin;
        ++begin;
    }
    return *prev;
}

#endif

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif
