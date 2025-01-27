#pragma once

#ifndef LZ_TAKE_WHILE_ITERABLE_HPP
#define LZ_TAKE_WHILE_ITERABLE_HPP

#include <Lz/detail/iterators/take_while.hpp>

namespace lz {
namespace detail {
template<class Iterable, class UnaryPredicate>
class take_while_iterable {
    Iterable _iterable;
    UnaryPredicate _unary_predicate;

    using inner_iter = iter_t<Iterable>;
    using inner_sentinel = sentinel_t<Iterable>;

public:
    using iterator = take_while_iterator<inner_iter, inner_sentinel, UnaryPredicate>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr take_while_iterable() = default;

    template<class I>
    constexpr take_while_iterable(I&& iterable, UnaryPredicate unary_predicate) :
        _iterable{ std::forward<I>(iterable) },
        _unary_predicate{ std::move(unary_predicate) } {
    }

    template<class I = inner_iter>
    LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _unary_predicate };
    }

    template<class I = inner_iter>
    LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, iterator> begin() && {
        return { std::begin(_iterable), std::end(_iterable), std::move(_unary_predicate) };
    }

    template<class I = inner_iter>
    LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable), _unary_predicate };
    }

    template<class I = inner_iter>
    LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, iterator> end() const& {
        return { std::end(_iterable), std::begin(_iterable), std::end(_iterable), _unary_predicate };
    }

    template<class I = inner_iter>
    LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, sentinel> end() const& {
        return {};
    }
};
} // namespace detail
} // namespace lz
#endif