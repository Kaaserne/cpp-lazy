#pragma once

#ifndef LZ_CACHED_SIZE_ITERABLE_HPP
#define LZ_CACHED_SIZE_ITERABLE_HPP

#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class cached_size_iterable : public lazy_view {
    maybe_owned<Iterable> _iterable{};
    size_t _size{};

public:
    using iterator = iter_t<Iterable>;
    using const_iterator = iterator;
    using sentinel = sentinel_t<Iterable>;

#ifdef LZ_HAS_CONCEPTS

    constexpr cached_size_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr cached_size_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    explicit constexpr cached_size_iterable(I&& iterable) :
        _iterable{ std::forward<I>(iterable) },
        _size{ static_cast<size_t>(lz::eager_size(_iterable)) } {
    }

    LZ_NODISCARD constexpr size_t size() const noexcept {
        return _size;
    }

    LZ_NODISCARD constexpr iterator begin() const {
        return _iterable.begin();
    }

    LZ_NODISCARD constexpr sentinel end() const {
        return _iterable.end();
    }
};
} // namespace detail
} // namespace lz

#endif
