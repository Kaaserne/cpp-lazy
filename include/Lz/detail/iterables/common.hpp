#pragma once

#ifndef LZ_COMMON_ITERABLE_HPP
#define LZ_COMMON_ITERABLE_HPP

#include <Lz/detail/iterators/common.hpp>

namespace lz {
namespace detail {

template<class Iterable>
class common_iterable {
    iterable_ref<Iterable> _iterable;

public:
    using iterator = common_iterator<iter_t<Iterable>, sentinel_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    common_iterable() = default;

    template<class I>
    constexpr common_iterable(I&& iterable) : _iterable{ iterable } {
    }

    LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { std::move(_iterable.get()).begin() };
    }

    LZ_CONSTEXPR_CXX_14 iterator end() && {
        return { std::move(_iterable.get()).end() };
    }

    LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable.get()) };
    }

    LZ_CONSTEXPR_CXX_14 iterator end() const& {
        return { std::end(_iterable.get()) };
    }
};
} // namespace detail
} // namespace lz

#endif