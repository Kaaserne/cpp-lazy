#pragma once

#ifndef LZ_INTERLEAVE_LONGEST_ITERATOR_HPP
#define LZ_INTERLEAVE_LONGEST_ITERATOR_HPP

#include <Lz/detail/iterator.hpp>
#include <Lz/detail/tuple_helpers.hpp>

namespace lz {
namespace detail {

template<class T>
class interleave_longest_sentinel {
    T value{};

    template<class, class>
    friend class interleave_longest_iterator;

    template<class...>
    friend class interleave_longest_iterable;

    explicit constexpr interleave_longest_sentinel(T v) noexcept(std::is_nothrow_move_constructible<T>::value) :
        value{ std::move(v) } {
    }

public:
#ifdef LZ_HAS_CXX_20
    constexpr interleave_longest_sentinel()
        requires(std::default_initializable<T>)
    = default;
#else
    template<class I = T, class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr interleave_longest_sentinel() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }
#endif
};

template<class IterTuple, class STuple>
class interleave_longest_iterator
    : public iterator<interleave_longest_iterator<IterTuple, STuple>, iter_tuple_common_ref_t<IterTuple>,
                      fake_ptr_proxy<iter_tuple_common_ref_t<IterTuple>>, iter_tuple_diff_type_t<IterTuple>,
                      iter_tuple_iter_cat_t<IterTuple>, interleave_longest_sentinel<STuple>> {

    using traits = std::iterator_traits<first_it_t<IterTuple>>;

public:
    using value_type = typename traits::value_type;
    using difference_type = iter_tuple_diff_type_t<IterTuple>;
    using reference = iter_tuple_common_ref_t<IterTuple>;
    using pointer = fake_ptr_proxy<reference>;

private:
    IterTuple _iterators{};
    // Using uint_least8_t because generally the max number of function parameters is 256
    std::uint_least8_t _index{};
    STuple _sentinels{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr interleave_longest_iterator()
        requires(std::default_initializable<STuple>)
    = default;

#else

    template<class I = Iterators, class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr interleave_longest_iterator() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif
    static constexpr auto tup_size = static_cast<std::uint_least8_t>(tuple_size<IterTuple>::value);

    static_assert(tup_size > 1, "interleaved_iterator must have at least two iterators");
    static_assert(tuple_size<IterTuple>::value <= std::numeric_limits<std::uint_least8_t>::max(),
                  "interleave_iterator tuple size exceeds uint_least8_t. This is not supported.");

    constexpr interleave_longest_iterator(IterTuple iterators, STuple sentinels) :
        _iterators{ std::move(iterators) },
        _sentinels{ std::move(sentinels) } {
    }
};
} // namespace detail
} // namespace lz

#endif
