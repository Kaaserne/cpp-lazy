#pragma once

#ifndef LZ_GROUP_BY_ITERATOR_HPP
#define LZ_GROUP_BY_ITERATOR_HPP

#include <Lz/algorithm/find_if.hpp>
#include <Lz/basic_iterable.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/remove_ref.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/util/default_sentinel.hpp>

namespace lz {
namespace detail {

template<class Iterable, class BinaryPredicate>
class group_by_iterator : public iterator<group_by_iterator<Iterable, BinaryPredicate>,
                                          std::pair<ref_t<iter_t<Iterable>>, basic_iterable<iter_t<Iterable>>>,
                                          fake_ptr_proxy<std::pair<ref_t<iter_t<Iterable>>, basic_iterable<iter_t<Iterable>>>>,
                                          ptrdiff_t, bidi_strongest_cat<iter_cat_t<iter_t<Iterable>>>, default_sentinel_t> {
    using it = iter_t<Iterable>;

    it _sub_range_end{};
    it _sub_range_begin{};
    Iterable _iterable{};
    mutable BinaryPredicate _comparer{};

    using ref_type = ref_t<it>;

    LZ_CONSTEXPR_CXX_14 void find_next(ref_type last_seen) {
        _sub_range_end = detail::find_if(std::move(_sub_range_end), _iterable.end(),
                                         [this, &last_seen](ref_type v) { return !_comparer(v, last_seen); });
    }

    LZ_CONSTEXPR_CXX_14 void advance() {
        if (_sub_range_end == _iterable.end()) {
            return;
        }
        ref_type last_seen = *_sub_range_end;
        ++_sub_range_end;
        find_next(last_seen);
    }

public:
    using value_type = std::pair<remove_cvref_t<ref_type>, basic_iterable<it>>;
    using reference = std::pair<ref_type, basic_iterable<it>>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = std::ptrdiff_t;

    constexpr group_by_iterator(const group_by_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 group_by_iterator& operator=(const group_by_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr group_by_iterator()
        requires(std::default_initializable<it> && std::default_initializable<Iterable> &&
                 std::default_initializable<BinaryPredicate>)
    = default;

#else

    template<class I = it,
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value &&
                                 std::is_default_constructible<BinaryPredicate>::value>>
    constexpr group_by_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                           std::is_nothrow_default_constructible<Iterable>::value &&
                                           std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 group_by_iterator(I&& iterable, it iter, BinaryPredicate binary_predicate) :
        _sub_range_end{ iter },
        _sub_range_begin{ std::move(iter) },
        _iterable{ std::forward<I>(iterable) },
        _comparer{ std::move(binary_predicate) } {

        advance();
    }

    LZ_CONSTEXPR_CXX_14 group_by_iterator& operator=(default_sentinel_t) {
        _sub_range_begin = _iterable.end();
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        return { *_sub_range_begin, { _sub_range_begin, _sub_range_end } };
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        _sub_range_begin = _sub_range_end;
        advance();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT_DECREMENTABLE(_sub_range_begin != _iterable.begin());
        _sub_range_end = _sub_range_begin;
        auto prev = --_sub_range_begin;

        ref_type last_seen = *_sub_range_begin;
        while (_sub_range_begin != _iterable.begin() && _comparer(*_sub_range_begin, last_seen)) {
            prev = _sub_range_begin;
            --_sub_range_begin;
        }

        if (_sub_range_begin == _iterable.begin() && _comparer(*_sub_range_begin, last_seen)) {
            return;
        }
        _sub_range_begin = std::move(prev);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const group_by_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_iterable.end() == other._iterable.end() && _iterable.begin() == other._iterable.begin());
        return _sub_range_begin == other._sub_range_begin;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _sub_range_begin == _iterable.end();
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_GROUP_BY_ITERATOR_HPP
