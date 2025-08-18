#pragma once

#ifndef LZ_EXCLUDE_ITERATOR_HPP
#define LZ_EXCLUDE_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {
namespace detail {
template<class Iterator, class S>
class exclude_iterator : public iterator<exclude_iterator<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                                         diff_type<Iterator>, iter_cat_t<Iterator>, S> {

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterator _iterator;
    difference_type _index{};
    difference_type _from{};
    difference_type _to{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr exclude_iterator()
        requires(std::default_initializable<Iterator>)
    = default;

#else

    template<class I = Iterator, class = enable_if<std::is_default_constructible<I>::value>>
    constexpr exclude_iterator() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_17
    exclude_iterator(Iterator begin, S end, const difference_type from, const difference_type to,
                     const difference_type start_index) :
        _iterator{ std::move(begin) },
        _index{ start_index },
        _from{ from },
        _to{ to } {
        LZ_ASSERT(from <= to, "From must be less than to");
        if (_iterator != end && _from == 0 && _index == 0) {
            _iterator = std::next(std::move(_iterator), _to);
            _index = _to;
        }
    }

    LZ_CONSTEXPR_CXX_14 exclude_iterator& operator=(const S& end) {
        _iterator = end;
        return *this;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    constexpr pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
        ++_index;
        if (_index == _from) {
            _iterator = std::next(std::move(_iterator), _to - _from);
            _index = _to;
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        if (_index == _to) {
            _iterator = std::prev(std::move(_iterator), _to - _from);
            _index = _from;
        }
        --_iterator;
        --_index;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const exclude_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_from == other._from && _to == other._to);
        const auto diff = _index - other._index;

        if (_iterator > other._iterator) {
            return _index < _to || other._index >= _from ? diff : diff - (_to - _from);
        }
        return _index >= _from || other._index < _to ? diff : diff + (_to - _from);
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const S& s) const {
        return _iterator - s + (_index >= _from ? 0 : _to - _from);
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
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
        LZ_ASSERT_COMPATIBLE(_from == other._from && _to == other._to);
        return _iterator == other._iterator;
    }

    constexpr bool eq(const S& s) const {
        return _iterator == s;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_EXCLUDE_ITERATOR_HPP
