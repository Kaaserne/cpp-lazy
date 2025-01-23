#pragma once

#ifndef LZ_CHUNK_IF_ITERABLE_HPP
#define LZ_CHUNK_IF_ITERABLE_HPP

#include <Lz/detail/iterators/chunk_if.hpp>

namespace lz {
namespace detail {

template<class ValueType, class Iterable, class UnaryPredicate>
class chunk_if_iterable {
    iterable_ref<Iterable> _iterable;
    UnaryPredicate _predicate;

public:
    using iterator = chunk_if_iterator<ValueType, iter_t<Iterable>, sentinel_t<Iterable>, UnaryPredicate>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I>
    constexpr chunk_if_iterable(I&& iterable, UnaryPredicate predicate) :
        _iterable{ iterable },
        _predicate{ std::move(predicate) } {
    }

    constexpr chunk_if_iterable() = default;

    iterator begin() && {
        auto begin = std::move(_iterable.get()).begin();
        auto end = std::move(_iterable.get()).end();
        const auto is_end = end == begin;
        return { begin, end, is_end };
    }

    iterator begin() const& {
        const auto is_end = std::end(_iterable.get()) == std::begin(_iterable.get());
        return { std::begin(_iterable.get()), std::end(_iterable.get()), _predicate, is_end };
    }

    constexpr default_sentinel end() const {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif