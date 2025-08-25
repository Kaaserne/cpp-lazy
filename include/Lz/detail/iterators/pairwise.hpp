#pragma once

#ifndef LZ_PAIRWISE_ITERATOR_HPP
#define LZ_PAIRWISE_ITERATOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/procs.hpp>
#include <limits>

namespace lz {
namespace detail {

template<class Iterable>
class ra_pairwise_iterator : public iterator<ra_pairwise_iterator<Iterable>, basic_iterable<iter_t<Iterable>>,
                                             fake_ptr_proxy<basic_iterable<iter_t<Iterable>>>, diff_iterable_t<Iterable>,
                                             iter_cat_t<iter_t<Iterable>>, default_sentinel_t> {

    using iter = iter_t<Iterable>;
    using traits = std::iterator_traits<iter>;

    iter _sub_begin{};
    Iterable _iterable{};
    size_t _pair_size{};

public:
    using value_type = basic_iterable<iter>;
    using reference = value_type;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

    constexpr ra_pairwise_iterator(const ra_pairwise_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 ra_pairwise_iterator& operator=(const ra_pairwise_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr ra_pairwise_iterator()
        requires(std::default_initializable<iter> && std::default_initializable<Iterable>)
    = default;

#else

    template<class I = iter,
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value>>
    constexpr ra_pairwise_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                              std::is_nothrow_default_constructible<Iterable>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 ra_pairwise_iterator(I&& iterable, iter it, const size_t pair_size) :
        _sub_begin{ std::move(it) },
        _iterable{ std::forward<I>(iterable) },
        _pair_size{ pair_size } {
        LZ_ASSERT(_pair_size != 0, "Size must be greater than zero");
    }

    LZ_CONSTEXPR_CXX_14 ra_pairwise_iterator& operator=(default_sentinel_t) {
        const auto size = _iterable.end() - _iterable.begin();
        _sub_begin = _iterable.begin() + (size - static_cast<difference_type>(_pair_size) + 1);
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return { _sub_begin, _sub_begin + static_cast<difference_type>(_pair_size) };
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        ++_sub_begin;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT_DECREMENTABLE(_sub_begin != _iterable.begin());
        --_sub_begin;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const ra_pairwise_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.end() == other._iterable.end() && _iterable.begin() == other._iterable.begin() &&
                             _pair_size == other._pair_size);
        return _sub_begin == other._sub_begin;
    }

    constexpr bool eq(default_sentinel_t) const {
        return (_sub_begin + std::min(static_cast<difference_type>(_pair_size) - 1, _iterable.end() - _iterable.begin())) ==
               std::end(_iterable);
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        LZ_ASSERT_SUB_ADDABLE(offset < 0
                                  ? -offset <= (_sub_begin - _iterable.begin())
                                  : offset <= (_iterable.end() - _sub_begin) - (static_cast<difference_type>(_pair_size - 1)));
        _sub_begin += offset;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const ra_pairwise_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.end() == other._iterable.end() && _pair_size == other._pair_size);
        return _sub_begin - other._sub_begin;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(default_sentinel_t) const {
        return (_sub_begin - _iterable.end()) + (static_cast<difference_type>(_pair_size) - 1);
    }
};

template<class Iterable>
class bidi_pairwise_iterator : public iterator<bidi_pairwise_iterator<Iterable>, basic_iterable<iter_t<Iterable>>,
                                               fake_ptr_proxy<basic_iterable<iter_t<Iterable>>>, diff_iterable_t<Iterable>,
                                               iter_cat_iterable_t<Iterable>, default_sentinel_t> {
    using iter = iter_t<Iterable>;
    using traits = std::iterator_traits<iter>;

    iter _sub_begin{};
    iter _sub_end{};
    Iterable _iterable{};
    size_t _pair_size{};

public:
    using value_type = basic_iterable<iter>;
    using reference = value_type;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

    constexpr bidi_pairwise_iterator(const bidi_pairwise_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 bidi_pairwise_iterator& operator=(const bidi_pairwise_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr bidi_pairwise_iterator()
        requires(std::default_initializable<iter>)
    = default;

#else

    template<class I = iter,
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value>>
    constexpr bidi_pairwise_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                                std::is_nothrow_default_constructible<Iterable>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 bidi_pairwise_iterator(I&& iterable, iter it, const size_t pair_size) :
        _sub_begin{ std::move(it) },
        _sub_end{ next_fast_safe(iterable, it == iterable.begin() ? static_cast<difference_type>(pair_size)
                                                                  : std::numeric_limits<difference_type>::max()) },
        _iterable{ std::forward<I>(iterable) },
        _pair_size{ pair_size } {
        LZ_ASSERT(_pair_size != 0, "Size must be greater than zero");
    }

    LZ_CONSTEXPR_CXX_14 bidi_pairwise_iterator& operator=(default_sentinel_t) {
        _sub_begin = _iterable.end();
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return { _sub_begin, _sub_end };
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        if (_sub_end == _iterable.end()) {
            _sub_begin = _iterable.end();
            return;
        }
        ++_sub_begin;
        ++_sub_end;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT_DECREMENTABLE(_sub_begin != _iterable.begin());
        if (_sub_begin == _iterable.end()) {
            _sub_begin = std::prev(_sub_begin, static_cast<difference_type>(_pair_size));
            return;
        }
        --_sub_begin;
        --_sub_end;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const bidi_pairwise_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_pair_size == other._pair_size && _iterable.end() == other._iterable.end() &&
                             _iterable.begin() == other._iterable.begin());
        return _sub_begin == other._sub_begin;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _sub_begin == _iterable.end();
    }
};
} // namespace detail
} // namespace lz

#endif
