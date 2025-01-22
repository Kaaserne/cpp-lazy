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
    : public iter_base<map_iterator<Iterator, S, UnaryOp>, func_container_ret_type<UnaryOp, Iterator>,
                       fake_ptr_proxy<func_container_ret_type<UnaryOp, Iterator>>, diff_type<Iterator>, iter_cat_t<Iterator>,
                       sentinel_selector<iter_cat_t<Iterator>, map_iterator<Iterator, S, UnaryOp>, S>> {
    Iterator _iterator;
    func_container<UnaryOp> _unary_op;

    using traits = std::iterator_traits<Iterator>;

public:
    using reference = decltype(_unary_op(*_iterator));
    using value_type = decay_t<reference>;
    using iterator_category = typename traits::iterator_category;
    using difference_type = typename traits::difference_type;
    using pointer = fake_ptr_proxy<reference>;

    constexpr map_iterator(Iterator iterator, UnaryOp unary_op) noexcept :
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

template<class Iterable, class UnaryOp>
class map_iterable {
    Iterable _iterable;
    UnaryOp _unary_op;

public:
    using iterator = map_iterator<iter_t<Iterable>, sentinel_t<Iterable>, UnaryOp>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I, class Uo>
    constexpr map_iterable(I&& iterable, Uo&& unary_op) :
        _iterable{ std::forward<I>(iterable) },
        _unary_op{ std::forward<Uo>(unary_op) } {
    }

    constexpr map_iterable() = default;

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const noexcept {
        return static_cast<std::size_t>(_iterable.size());
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, iterator> begin() && {
        return { std::move(_iterable).begin(), _unary_op };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, iterator> begin() && {
        return { std::move(_iterable).begin(), std::move(_unary_op) };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), _unary_op };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, I> end() && {
        return { std::move(_iterable).end(), _unary_op };
    }

    template<class I = iterator>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, I> end() const& {
        return { std::end(_iterable), _unary_op };
    }

    template<class I = iterator>
    LZ_NODISCARD constexpr enable_if<!is_bidi<I>::value, sentinel> end() const& {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif