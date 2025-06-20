#pragma once

#ifndef LZ_REVERSE_ITERABLE_HPP
#define LZ_REVERSE_ITERABLE_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterators/cached_reverse.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {

template<class Iterable, bool Cached>
class reverse_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;

    using inner_iter = iter_t<Iterable>;

public:
    using iterator = conditional<Cached, cached_reverse_iterator<inner_iter>, std::reverse_iterator<inner_iter>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr reverse_iterable()
        requires std::default_initializable<Iterable>
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr reverse_iterable() {
    }

#endif

    template<class I>
    constexpr reverse_iterable(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(lz::size(_iterable));
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr iterator begin() const& {
        if constexpr (Cached) {
            return { std::end(_iterable), std::begin(_iterable), std::end(_iterable) };
        }
        else {
            return iterator{ std::end(_iterable) };
        }
    }

    [[nodiscard]] constexpr iterator begin() && {
        if constexpr (Cached) {
            return { std::end(_iterable), std::begin(_iterable), std::end(_iterable) };
        }
        else {
            return iterator{ detail::end(std::move(_iterable)) };
        }
    }

    [[nodiscard]] constexpr iterator end() const& {
        if constexpr (Cached) {
            return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable) };
        }
        else {
            return iterator{ std::begin(_iterable) };
        }
    }

    [[nodiscard]] constexpr iterator end() && {
        if constexpr (Cached) {
            return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable) };
        }
        else {
            return iterator{ detail::begin(std::move(_iterable)) };
        }
    }

#else

    template<bool C = Cached>
    LZ_NODISCARD constexpr enable_if<C, iterator> begin() const& {
        return { std::end(_iterable), std::begin(_iterable), std::end(_iterable) };
    }

    // Using constexpr 17 here because cxx 17 has constexpr reverse_iterator
    template<bool C = Cached>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 enable_if<!C, iterator> begin() const& {
        return iterator{ std::end(_iterable) };
    }

    template<bool C = Cached>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 enable_if<!C, iterator> begin() && {
        return iterator{ detail::end(std::move(_iterable)) };
    }

    template<bool C = Cached>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 enable_if<!C, iterator> end() const& {
        return iterator{ std::begin(_iterable) };
    }

    template<bool C = Cached>
    LZ_NODISCARD constexpr enable_if<C, iterator> end() const& {
        return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable) };
    }

    template<bool C = Cached>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 enable_if<!C, iterator> end() && {
        return iterator{ detail::begin(std::move(_iterable)) };
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_REVERSE_ITERABLE_HPP
