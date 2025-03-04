#pragma once

#ifndef LZ_TAKE_EVERY_ITERATOR_HPP
#define LZ_TAKE_EVERY_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterator, class S, class = void>
class take_every_iterator;

template<class Iterator, class S>
class take_every_iterator<Iterator, S, enable_if<!is_bidi<Iterator>::value>>
    : public iterator<take_every_iterator<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      std::forward_iterator_tag, default_sentinel> {
    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    Iterator _iterator;
    S _end;
    difference_type _offset;

public:
    constexpr take_every_iterator(Iterator iterator, S end, const difference_type offset) :
        _iterator{ std::move(iterator) },
        _end{ std::move(end) },
        _offset{ offset } {
    }

    constexpr take_every_iterator() = default;

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        for (difference_type count = 0; _iterator != _end && count < _offset; ++_iterator, ++count) {
        }
    }

    constexpr bool eq(const take_every_iterator& b) const noexcept {
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return _iterator == _end;
    }
};

template<class Iterator, class S>
class take_every_iterator<Iterator, S, enable_if<is_bidi<Iterator>::value>>
    : public iterator<take_every_iterator<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      iter_cat_t<Iterator>> {
    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using iterator_category = typename traits::iterator_category;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    Iterator _begin;
    Iterator _iterator;
    S _end;
    difference_type _offset{};

#ifdef __cpp_if_constexpr
    LZ_CONSTEXPR_CXX_14 void advance() noexcept {
        if constexpr (!is_ra<Iterator>::value) {
            for (difference_type count = 0; _iterator != _end && count < _offset; ++_iterator, ++count) {
            }
        }
        else {
            const auto distance = _end - _iterator;
            if (_offset >= distance) {
                _iterator = _end;
            }
            else {
                _iterator += _offset;
            }
        }
    }

    LZ_CONSTEXPR_CXX_14 void previous() noexcept {
        if constexpr (!is_ra<Iterator>::value) {
            for (difference_type count = _offset; _iterator != _begin && count >= 0; --_iterator, --count) {
            }
        }
        else {
            const auto distance = _iterator - _begin;
            if (_offset >= distance) {
                _iterator = _begin;
            }
            else {
                _iterator -= _offset;
            }
        }
    }
#else
    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_14 enable_if<is_ra<I>::value, void> advance() {
        const auto distance = _end - _iterator;
        if (_offset >= distance) {
            _iterator = _end;
        }
        else {
            _iterator += _offset;
        }
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_14 enable_if<!is_ra<I>::value> advance() {
        for (difference_type count = 0; _iterator != _end && count < _offset; ++_iterator, ++count) {
        }
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_14 enable_if<is_ra<I>::value> previous() {
        const auto distance = _iterator - _begin;
        if (_offset >= distance) {
            _iterator = _begin;
        }
        else {
            _iterator -= _offset;
        }
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_14 enable_if<!is_ra<I>::value> previous() {
        for (difference_type count = _offset; _iterator != _begin && count >= 0; --_iterator, --count) {
        }
    }
#endif

public:
    constexpr take_every_iterator(Iterator iterator, Iterator begin, S end, const difference_type offset) :
        _begin{ std::move(begin) },
        _iterator{ std::move(iterator) },
        _end{ std::move(end) },
        _offset{ offset } {
    }

    constexpr take_every_iterator() = default;

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        advance();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        if (_iterator == _end) {
            --_iterator;
        }
        else {
            previous();
        }
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        difference_type extra = 0;
        if (_iterator == _end && offset < 0) {
            --_iterator;
            if (offset == -1) {
                return;
            }
            extra = _offset;
        }

        const auto to_add = _offset * offset + extra;
        if (to_add > 0) {
            const auto remaining = _end - _iterator;
            _iterator += to_add >= remaining ? remaining : to_add;
            return;
        }

        const auto remaining = _begin - _iterator;
        const auto remaining_pos = -remaining;
        _iterator += -to_add >= remaining_pos ? remaining : to_add;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const take_every_iterator& other) const {
        if (_offset == 0) {
            return _iterator - other._iterator;
        }
        const auto raw_diff = _iterator - other._iterator;
        if (raw_diff == 0) {
            return 0;
        }
        return (raw_diff + (_offset - 1)) / _offset;
    }

    constexpr bool eq(const take_every_iterator& b) const noexcept {
        return _iterator == b._iterator;
    }
};
} // namespace detail
} // namespace lz

#endif