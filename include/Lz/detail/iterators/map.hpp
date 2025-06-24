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
                      fake_ptr_proxy<func_ret_type_iter<UnaryOp, Iterator>>, diff_type<Iterator>, iter_cat_t<Iterator>, S> {
    Iterator _iterator;
    mutable UnaryOp _unary_op;

    using traits = std::iterator_traits<Iterator>;

public:
    using reference = decltype(_unary_op(*_iterator));
    using value_type = decay_t<reference>;
    using iterator_category = typename traits::iterator_category;
    using difference_type = typename traits::difference_type;
    using pointer = fake_ptr_proxy<reference>;

#ifdef LZ_HAS_CONCEPTS

    constexpr map_iterator()
        requires std::default_initializable<Iterator> && std::default_initializable<UnaryOp>
    = default;

#else

    template<class I = Iterator,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<UnaryOp>::value>>
    constexpr map_iterator() noexcept(std::is_nothrow_default_constructible<Iterator>::value &&
                                      std::is_nothrow_default_constructible<UnaryOp>::value) {
    }

#endif

    constexpr map_iterator(Iterator it, UnaryOp unary_op) :
        _iterator{ std::move(it) },
        _unary_op{ std::move(unary_op) } {
    }

    LZ_CONSTEXPR_CXX_14 map_iterator& operator=(const S& s) {
        _iterator = s;
        return *this;
    }

    constexpr reference dereference() const {
        return _unary_op(*_iterator);
    }

    constexpr pointer arrow() const {
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

    constexpr bool eq(const map_iterator& b) const {
        return _iterator == b._iterator;
    }

    constexpr bool eq(const S& s) const {
        return _iterator == s;
    }
};
} // namespace detail
} // namespace lz

#endif