#pragma once

#ifndef LZ_DETAIL_ALGORITHM_BACK_HPP
#define LZ_DETAIL_ALGORITHM_BACK_HPP

#include <Lz/detail/procs/assert.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

namespace lz {
namespace detail {
namespace algorithm {

template<class Iterator, class S>
constexpr ref_t<Iterator> back(Iterator begin, S end) {
    LZ_ASSERT(begin != end, "Cannot get back of empty iterable");

    if constexpr (is_ra_v<Iterator>) { // TODO enable_if
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

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif
