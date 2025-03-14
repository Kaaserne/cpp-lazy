#pragma once

#ifndef LZ_CACHED_SIZE_ITERABLE_HPP
#define LZ_CACHED_SIZE_ITERABLE_HPP

#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class cached_size_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    std::size_t _size;

public:
    using iterator = iter_t<Iterable>;
    using const_iterator = iterator;
    using sentinel = sentinel_t<Iterable>;

    constexpr cached_size_iterable() = default;

    template<class I>
    constexpr cached_size_iterable(I&& iterable) :
        _iterable{ std::forward<I>(iterable) },
        _size{ static_cast<std::size_t>(lz::eager_size(_iterable)) } {
    }

    LZ_NODISCARD constexpr std::size_t size() const noexcept {
        return _size;
    }

    LZ_NODISCARD constexpr iterator begin() const& {
        return _iterable.begin();
    }

    LZ_NODISCARD constexpr sentinel end() const& {
        return _iterable.end();
    }

    LZ_NODISCARD constexpr iterator begin() && {
        return detail::begin(std::move(_iterable));
    }

    LZ_NODISCARD constexpr sentinel end() && {
        return detail::end(std::move(_iterable));
    }
};
} // namespace detail
} // namespace lz

#endif
