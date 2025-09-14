#pragma once

#ifndef LZ_EXCLUDE_ITERATOR_HPP
#define LZ_EXCLUDE_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class exclude_iterator
    : public iterator<exclude_iterator<Iterable>, ref_t<iter_t<Iterable>>, fake_ptr_proxy<ref_t<iter_t<Iterable>>>,
                      diff_type<iter_t<Iterable>>, iter_cat_t<iter_t<Iterable>>, default_sentinel_t> {

    using iter = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;
    using traits = std::iterator_traits<iter>;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterable _iterable{};
    iter _iterator{};
    difference_type _index{};
    difference_type _from{};
    difference_type _to{};

public:
    constexpr exclude_iterator(const exclude_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 exclude_iterator& operator=(const exclude_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr exclude_iterator()
        requires(std::default_initializable<Iterator>)
    = default;

#else

    template<class I = iter, class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr exclude_iterator() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_17 exclude_iterator(I&& iterable, iter begin, const difference_type from, const difference_type to,
                                         const difference_type start_index) :
        _iterable{ std::forward<I>(iterable) },
        _iterator{ std::move(begin) },
        _index{ start_index },
        _from{ from },
        _to{ to } {
        LZ_ASSERT(from <= to, "From must be less than to");
        if (_iterator != _iterable.end() && _from == 0 && _index == 0) {
            _iterator = next_fast_safe(_iterable, _to);
            _index = _to;
        }
    }

#ifdef LZ_HAS_CXX_17

    LZ_CONSTEXPR_CXX_14 exclude_iterator& operator=(default_sentinel_t) {
        _iterator = _iterable.end();
        if constepxr (is_bidi_v<iter>) {
            _index = static_cast<difference_type>(lz::eager_size(_iterable));
        }
        return *this;
    }

#else

    template<class I = iter>
    LZ_CONSTEXPR_CXX_14 enable_if_t<is_bidi<I>::value, exclude_iterator&> operator=(default_sentinel_t) {
        _iterator = _iterable.end();
        _index = static_cast<difference_type>(lz::eager_size(_iterable));
        return *this;
    }

    template<class I = iter>
    LZ_CONSTEXPR_CXX_14 enable_if_t<!is_bidi<I>::value, exclude_iterator&> operator=(default_sentinel_t) {
        _iterator = _iterable.end();
        return *this;
    }

#endif

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        ++_iterator;
        ++_index;
        if (_index == _from) {
            _iterator = std::next(std::move(_iterator), _to - _from);
            _index = _to;
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT_DECREMENTABLE(_iterator != _iterable.begin());
        if (_index == _to) {
            _iterator = std::prev(std::move(_iterator), _to - _from);
            _index = _from;
        }
        --_iterator;
        --_index;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const exclude_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_from == other._from && _to == other._to && _iterable.begin() == other._iterable.begin() &&
                             _iterable.end() == other._iterable.end());
        const auto diff = _index - other._index;

        if (_iterator > other._iterator) {
            return _index < _to || other._index >= _from ? diff : diff - (_to - _from);
        }
        return _index >= _from || other._index < _to ? diff : diff + (_to - _from);
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(default_sentinel_t) const {
        return _iterator - _iterable.end() + (_index >= _from ? 0 : _to - _from);
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        // Avoid subtraction-based comparison to prevent -Wstrict-overflow; compare directly instead.
        LZ_ASSERT_ADDABLE((_iterable.end() - _iterable.begin()) >= (_to - _from));
        if (offset == 0) {
            return;
        }

        _iterator += offset;
        _index += offset;
        const auto is_positive_offset = offset > 0;

        if (((!is_positive_offset || _index < _from) && (is_positive_offset || _index >= _to)) || (_from == 0)) {
            return;
        }

        const auto to_skip = _to - _from;
        _iterator += is_positive_offset ? to_skip : -to_skip;
        _index += is_positive_offset ? to_skip : -to_skip;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const exclude_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_from == other._from && _to == other._to && _iterable.begin() == other._iterable.begin() &&
                             _iterable.end() == other._iterable.end());
        return _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator == _iterable.end();
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_EXCLUDE_ITERATOR_HPP
