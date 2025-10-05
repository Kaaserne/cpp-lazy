#pragma once

#ifndef LZ_DETAIL_PROCS_NEXT_FAST_HPP
#define LZ_DETAIL_PROCS_NEXT_FAST_HPP

#include <Lz/detail/procs/begin_end.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>
#include <Lz/detail/traits/is_sized.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

#ifndef LZ_HAS_CXX_17

#include <Lz/detail/traits/enable_if.hpp>

#endif

// next_fast(_safe): check whether it's faster to go forward or backward when incrementing n steps
// Only relevant for sized (requesting size is then O(1)) bidirectional iterators because
// for random access iterators this is O(1) anyway. For forward iterators this is not O(1),
// but we can't go any other way than forward. Random access iterators will use the same
// implementation as the forward non-sized iterables so we can skip that if statement.

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class I>
[[nodiscard]] constexpr iter_t<I> next_fast(I&& iterable, diff_iterable_t<I> n) {
    using iter = iter_t<I>;

    if constexpr (is_sized_v<I> && !is_sentinel_v<iter, sentinel_t<I>> && is_bidi_v<iter>) {
        const auto size = lz::ssize(iterable);
        if (n > size / 2) {
            return std::prev(detail::end(iterable), size - n);
        }
    }
    return std::next(detail::begin(iterable), n);
}

template<class I>
[[nodiscard]] constexpr iter_t<I> next_fast_safe(I&& iterable, const diff_iterable_t<I> n) {

    auto begin = iterable.begin();
    auto end = iterable.end();

    if constexpr (is_sized_v<I> && is_bidi_v<iter_t<I>> && !is_sentinel_v<iter_t<I>, sentinel_t<I>>) {
        const auto size = lz::ssize(iterable);
        if (n >= size) {
            return end;
        }
        return next_fast(std::forward<I>(iterable), n);
    }
    else {
        using diff_type_u = std::make_unsigned_t<diff_iterable_t<I>>;
        for (diff_type_u i = 0; i < static_cast<diff_type_u>(n) && begin != end; ++i, ++begin) {
        }
        return begin;
    }
}

#else

template<class I>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17
    enable_if_t<is_sized<I>::value && !is_sentinel<iter_t<I>, sentinel_t<I>>::value && is_bidi<iter_t<I>>::value, iter_t<I>>
    next_fast(I&& iterable, const diff_iterable_t<I> n) {

    const auto size = lz::ssize(iterable);
    if (n > size / 2) {
        return std::prev(detail::end(iterable), size - n);
    }
    return std::next(detail::begin(iterable), n);
}

template<class I>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17
    enable_if_t<!is_sized<I>::value || is_sentinel<iter_t<I>, sentinel_t<I>>::value || !is_bidi<iter_t<I>>::value, iter_t<I>>
    next_fast(I&& iterable, diff_iterable_t<I> n) {
    return std::next(detail::begin(iterable), n);
}

template<class I>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17
    enable_if_t<is_sized<I>::value && !is_sentinel<iter_t<I>, sentinel_t<I>>::value && is_bidi<iter_t<I>>::value, iter_t<I>>
    next_fast_safe(I&& iterable, const diff_iterable_t<I> n) {

    const auto size = lz::ssize(iterable);
    if (n >= size) {
        return detail::end(iterable);
    }
    return next_fast(std::forward<I>(iterable), n);
}

template<class I>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14
    enable_if_t<!is_sized<I>::value || is_sentinel<iter_t<I>, sentinel_t<I>>::value || !is_bidi<iter_t<I>>::value, iter_t<I>>
    next_fast_safe(I&& iterable, const diff_iterable_t<I> n) {

    auto begin = detail::begin(iterable);
    const auto end = detail::end(iterable);

    using diff_type_u = typename std::make_unsigned<diff_iterable_t<I>>::type;
    for (diff_type_u i = 0; i < static_cast<diff_type_u>(n) && begin != end; ++i, ++begin) {
    }
    return begin;
}

template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator>::value, diff_type<Iterator>> distance_impl(Iterator begin, S end) {
    diff_type<Iterator> dist = 0;
    for (; begin != end; ++begin, ++dist) {
    }
    return dist;
}

template<class Iterator, class S>
constexpr enable_if_t<is_ra<Iterator>::value, diff_type<Iterator>> distance_impl(Iterator begin, S end) {
    return end - begin;
}

#endif

} // namespace detail
} // namespace lz

#endif
