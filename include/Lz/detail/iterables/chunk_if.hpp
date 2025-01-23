#pragma once

#ifndef LZ_CHUNK_IF_ITERABLE_HPP
#define LZ_CHUNK_IF_ITERABLE_HPP

#include <Lz/detail/iterators/chunk_if.hpp>

namespace lz {
namespace detail {

template<class ValueType, class Iterable, class UnaryPredicate>
class chunk_if_iterable {
    Iterable _iterable;
    UnaryPredicate _predicate;

public:
    using iterator = chunk_if_iterator<ValueType, iter_t<Iterable>, sentinel_t<Iterable>, UnaryPredicate>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I, class Up>
    constexpr chunk_if_iterable(I&& iterable, Up&& predicate) :
        _iterable{ std::forward<I>(iterable) },
        _predicate{ std::forward<Up>(predicate) } {
    }

    constexpr chunk_if_iterable() = default;

    iterator begin() && {
        auto begin = std::move(_iterable).begin();
        auto end = std::move(_iterable).end();
        const auto is_end = end == begin;
        return { begin, end, is_end };
    }

    iterator begin() const& {
        const auto is_end = std::end(_iterable) == std::begin(_iterable);
        return { std::begin(_iterable), std::end(_iterable), _predicate, is_end };
    }

    constexpr default_sentinel end() const& {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif