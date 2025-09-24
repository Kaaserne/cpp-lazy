#pragma once

#ifndef LZ_SENTINEL_WITH_HPP
#define LZ_SENTINEL_WITH_HPP

namespace lz {
namespace detail {

// To prevent sentinels that are decrementable (or incrementable) we create a wrapper around them that does not
// have any increment or decrement operators. For instance: we have a sentinelled bidirectional filter iterator. begin() returns
// the actual iterator, end() returns the sentinel, that for some reason is decrementable. Now if we do --end, the sentinel is
// decremented, not taking into account the filter predicate. Example:
// ```cpp
// auto lst = {0, 1, 2, 3, 4, 5};
// auto bidi_sent_filter = filter(lst, is_even) // only take even numbers -> {0, 2, 4}. Let's assume end() returns a sentinel that
// is decrementable
//
// auto it = bidi_sent_filter.begin();
// auto end = bidi_sent_filter.end(); // end is decrementable, for some reason, but is still sentinelled. type(it) != type(end)
// --end; // end is decremented, not taking into account the filter predicate. End is now at 5 because end is not a filter
// // iterator, but a sentinel that is decrementable. If we do while (it != end) { ++it; } we get an infinite loop, because it
// // will never reach 5, because it only produces even numbers
// ```
template<class T>
class sentinel_with {
    T value{};

    template<class, class>
    friend class enumerate_iterable;

    template<class, class, class>
    friend class enumerate_iterator;

    template<class, bool>
    friend class loop_iterable;

    template<class, bool>
    friend class loop_iterator;

    template<class...>
    friend class zip_iterator;

    template<class...>
    friend class zip_iterable;

    template<class, class, class>
    friend class as_iterator_iterator;

    template<class, class>
    friend class as_iterator_iterable;

    template<class, class>
    friend class interleave_iterator;

    template<class...>
    friend class interleave_iterable;

    template<class, class>
    friend class regex_split_iterator;

    template<class, class>
    friend class regex_split_iterable;

    explicit constexpr sentinel_with(T v) noexcept(std::is_nothrow_move_constructible<T>::value) : value{ std::move(v) } {
    }

public:
    constexpr sentinel_with() = default;
};
} // namespace detail
} // namespace lz

#endif
