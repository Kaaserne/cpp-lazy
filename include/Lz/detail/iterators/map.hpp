#pragma once

#ifndef LZ_MAP_ITERATOR_HPP
#define LZ_MAP_ITERATOR_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterator, class S, class UnaryOp>
class map_iterator
    : public iterator<map_iterator<Iterator, S, UnaryOp>, func_ret_type_iter<UnaryOp, Iterator>,
                      fake_ptr_proxy<func_ret_type_iter<UnaryOp, Iterator>>, diff_type<Iterator>, iter_cat_t<Iterator>,
                      sentinel_selector<iter_cat_t<Iterator>, map_iterator<Iterator, S, UnaryOp>, S>> {
    Iterator _iterator{};
    UnaryOp _unary_op{};

    using traits = std::iterator_traits<Iterator>;

public:
    using reference = decltype(_unary_op(*_iterator));
    using value_type = decay_t<reference>;
    using iterator_category = typename traits::iterator_category;
    using difference_type = typename traits::difference_type;
    using pointer = fake_ptr_proxy<reference>;

    constexpr map_iterator(Iterator iterator, UnaryOp unary_op) :
        _iterator{ std::move(iterator) },
        _unary_op{ std::move(unary_op) } {
    }

    constexpr map_iterator() = default;

    constexpr reference dereference() const {
        return _unary_op(*_iterator);
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_iterator;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        _iterator += offset;
    }

    constexpr difference_type difference(const map_iterator& b) const {
        return _iterator - b._iterator;
    }

    constexpr bool eq(const map_iterator& b) const noexcept {
        return _iterator == b._iterator;
    }

    constexpr bool eq(const S& s) const noexcept {
        return _iterator == s;
    }
};
} // namespace detail
} // namespace lz

#endif