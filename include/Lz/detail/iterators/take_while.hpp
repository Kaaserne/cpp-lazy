#pragma once

#ifndef LZ_TAKE_WHILE_ITERATOR_HPP
#define LZ_TAKE_WHILE_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/common.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterator, class S, class UnaryPredicate>
class take_while_iterator
    : public iter_base<take_while_iterator<Iterator, S, UnaryPredicate>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                       diff_type<Iterator>, std::forward_iterator_tag, default_sentinel> {

    common_iterator<Iterator, S> _iterator;
    S _end;
    func_container<UnaryPredicate> _unary_predicate;

    using traits = std::iterator_traits<Iterator>;

    LZ_CONSTEXPR_CXX_14 void incremented_check() {
        if (_iterator != _end && !_unary_predicate(*_iterator)) {
            _iterator = _end;
        }
    }

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    constexpr take_while_iterator() = default;

    LZ_CONSTEXPR_CXX_14 take_while_iterator(Iterator iterator, S end, UnaryPredicate unary_predicate) :
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _unary_predicate(std::move(unary_predicate)) {
        incremented_check();
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
        incremented_check();
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    constexpr bool eq(const take_while_iterator& b) const noexcept {
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return _iterator == _end;
    }
};

template<class Iterator, class UnaryPredicate>
class take_while_iterator<Iterator, Iterator, UnaryPredicate>
    : public iter_base<take_while_iterator<Iterator, Iterator, UnaryPredicate>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                       diff_type<Iterator>, std::bidirectional_iterator_tag> {

    Iterator _begin;
    Iterator _iterator;
    Iterator _end;
    func_container<UnaryPredicate> _unary_predicate;

    using traits = std::iterator_traits<Iterator>;

    void incremented_check() {
        if (_iterator != _end && !_unary_predicate(*_iterator)) {
            _iterator = _end;
        }
    }

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

    constexpr take_while_iterator() = default;

    LZ_CONSTEXPR_CXX_14 take_while_iterator(Iterator iterator, Iterator begin, Iterator end, UnaryPredicate unary_predicate) :
        _begin(std::move(begin)),
        _iterator(std::move(iterator)),
        _end(std::move(end)),
        _unary_predicate(std::move(unary_predicate)) {
        if (iterator == begin) {
            incremented_check();
        }
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        ++_iterator;
        incremented_check();
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        if (_iterator != _end) {
            --_iterator;
            return;
        }
        do {
            --_iterator;
        } while (_iterator != _begin && !_unary_predicate(*_iterator));
    }

    LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const take_while_iterator& b) const noexcept {
        return _iterator == b._iterator;
    }

    LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const noexcept {
        return _iterator == _end;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_TAKE_WHILE_ITERATOR_HPP