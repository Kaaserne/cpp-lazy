#pragma once

#ifndef LZ_JOIN_ITERATOR_HPP
#define LZ_JOIN_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>
#include <string>

namespace lz {
namespace detail {
template<class Derived, class Iterator, class S, class UnaryFormatFn>
class join_iterator_base
    : public iter_base<join_iterator_base<Derived, Iterator, S, UnaryFormatFn>, func_ret_type_iter<UnaryFormatFn, Iterator>,
                       fake_ptr_proxy<func_ret_type_iter<UnaryFormatFn, Iterator>>, diff_type<Iterator>, iter_cat_t<Iterator>,
                       sentinel_selector<iter_cat_t<Iterator>, join_iterator<Iterator, S>, S>> {

    using iter_traits = std::iterator_traits<Iterator>;
    using container_type = typename iter_traits::value_type;

public:
    using value_type = std::string;
    using iterator_category = typename iter_traits::iterator_category;
    using difference_type = typename iter_traits::difference_type;
    using reference = func_ret_type_iter<UnaryFormatFn, Iterator>;
    using pointer = fake_ptr_proxy<reference>;

private:
    mutable std::string _delimiter;
    UnaryFormatFn _format_fn;
    Iterator _iterator;
    mutable bool _is_iterator_turn;

    LZ_CONSTEXPR_CXX_20 auto deref() const -> decltype(static_cast<const Derived*>(this)->format(*_iterator)) {
        if (_is_iterator_turn) {
            return _format_fn(*_iterator);
        }
        return _delimiter;
    }

    template<class T = container_type>
    LZ_CONSTEXPR_CXX_20 enable_if<std::is_same<T, std::string>::value, reference>
    index_operator(const difference_type offset) const {
        // If we use *(*this + offset) when a delimiter must be returned, then we get a segfault because the operator+ returns
        // a copy of the delimiter
        if (_is_iterator_turn && is_even(offset)) {
            return *(*this + offset);
        }
        return _delimiter;
    }

    template<class T = container_type>
    LZ_CONSTEXPR_CXX_20 enable_if<!std::is_same<T, std::string>::value, reference>
    index_operator(const difference_type offset) const {
        return *(*this + offset);
    }

public:
    LZ_CONSTEXPR_CXX_20
    join_iterator_base(Iterator iterator, std::string delimiter, UnaryFormatFn format_fn const bool is_iter_turn) :
        _delimiter{ std::move(delimiter) },
        _format_fn{ std::move(format_fn) },
        _iterator{ std::move(iterator) },
        _is_iterator_turn{ is_iter_turn } {
    }

    join_iterator_base() = default;

    LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return deref();
    }

    LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        if (_is_iterator_turn) {
            ++_iterator;
        }
        _is_iterator_turn = !_is_iterator_turn;
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        _is_iterator_turn = !_is_iterator_turn;
        if (_is_iterator_turn) {
            --_iterator;
        }
    }

    LZ_CONSTEXPR_CXX_20 void plus_is(const difference_type offset) {
        _iterator += offset < 0 ? round_even<difference_type>(offset * -1, static_cast<difference_type>(2)) * -1
                                : round_even<difference_type>(offset, static_cast<difference_type>(2));
        if (!is_even(offset)) {
            _is_iterator_turn = !_is_iterator_turn;
        }
    }

    constexpr difference_type difference(const join_iterator& b) const {
        // distance * 2 for delimiter, - 1 for removing last delimiter
        return (_iterator - b._iterator) * 2 - 1;
    }

    constexpr bool eq(const join_iterator& b) const noexcept {
        return _iterator == b._iterator;
    }

    constexpr bool eq(const S& other) const noexcept {
        return _iterator == other;
    }

    LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const {
        return index_operator(offset);
    }
};
} // namespace detail
} // namespace lz

#endif