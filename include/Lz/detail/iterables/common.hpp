#pragma once

#ifndef LZ_COMMON_ITERABLE_HPP
#define LZ_COMMON_ITERABLE_HPP

#include <Lz/detail/iterators/common.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {

template<class Iterable>
class common_iterable : public lazy_view {
    maybe_owned<Iterable> _iterable{};

public:
    using iterator = common_iterator<iter_t<Iterable>, sentinel_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr common_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr common_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    explicit constexpr common_iterable(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

#ifdef LZ_HAS_CONCEPTS

    LZ_NODISCARD constexpr size_t size() const
        requires(sized<Iterable>)
    {
        return lz::size(_iterable);
    }

#else

    template<bool Sized = is_sized<Iterable>::value>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<Sized, size_t> size() const {
        return lz::size(_iterable);
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return iterator{ _iterable.begin() };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator end() const {
        return iterator{ _iterable.end() };
    }
};
} // namespace detail
} // namespace lz

#endif
