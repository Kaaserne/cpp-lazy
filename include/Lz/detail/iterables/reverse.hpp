#pragma once

#ifndef LZ_REVERSE_ITERABLE_HPP
#define LZ_REVERSE_ITERABLE_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterators/cached_reverse.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {

template<class Iterable, bool Cached>
class reverse_iterable : public lazy_view {
    maybe_owned<Iterable> _iterable;

    using inner_iter = iter_t<Iterable>;
    using inner_sent = sentinel_t<Iterable>;

public:
    using iterator = conditional_t<Cached, cached_reverse_iterator<inner_iter>, std::reverse_iterator<inner_iter>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr reverse_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr reverse_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    explicit constexpr reverse_iterable(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterable>)
    {
        return static_cast<size_t>(lz::size(_iterable));
    }

#else

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if_t<is_sized<I>::value, size_t> size() const {
        return static_cast<size_t>(lz::size(_iterable));
    }

#endif

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr iterator begin() const {
        if constexpr (Cached && std::is_same_v<inner_iter, inner_sent>) {
            return { _iterable.end(), _iterable.begin(), _iterable.end() };
        }
        else if constexpr (Cached && !std::is_same_v<inner_iter, inner_sent>) {
            const auto size = _iterable.end() - _iterable.begin();
            return { _iterable.begin() + size, _iterable.begin(), _iterable.end() };
        }
        else if constexpr (std::is_same_v<inner_iter, inner_sent>) {
            return iterator{ _iterable.end() };
        }
        else {
            static_assert(is_ra<inner_iter>::value, "Cannot get end for non random access iterators");
            const auto size = _iterable.end() - _iterable.begin();
            return iterator{ _iterable.begin() + size };
        }
    }

    [[nodiscard]] constexpr iterator end() const {
        if constexpr (Cached) {
            return { _iterable.begin(), _iterable.begin(), _iterable.end() };
        }
        else {
            return iterator{ _iterable.begin() };
        }
    }

#else

    template<bool C = Cached>
    LZ_NODISCARD constexpr enable_if_t<C && std::is_same<inner_iter, inner_sent>::value, iterator> begin() const {
        return { _iterable.end(), _iterable.begin(), _iterable.end() };
    }

    template<bool C = Cached>
    LZ_NODISCARD constexpr enable_if_t<C && !std::is_same<inner_iter, inner_sent>::value, iterator> begin() const {
        static_assert(is_ra<inner_iter>::value, "Cannot get end for non random access iterators");
        return { _iterable.begin() + (_iterable.end() - _iterable.begin()), _iterable.begin(), _iterable.end() };
    }

    // Using constexpr 17 here because cxx 17 has constexpr reverse_iterator
    template<bool C = Cached>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 enable_if_t<!C && std::is_same<inner_iter, inner_sent>::value, iterator> begin() const {
        return iterator{ _iterable.end() };
    }

    template<bool C = Cached>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 enable_if_t<!C && !std::is_same<inner_iter, inner_sent>::value, iterator> begin() const {
        static_assert(is_ra<inner_iter>::value, "Cannot get end for non random access iterators");
        const auto size = _iterable.end() - _iterable.begin();
        return iterator{ detail::begin(_iterable) + size };
    }

    template<bool C = Cached>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 enable_if_t<!C, iterator> end() const {
        return iterator{ _iterable.begin() };
    }

    template<bool C = Cached>
    LZ_NODISCARD constexpr enable_if_t<C, iterator> end() const {
        return { _iterable.begin(), _iterable.begin(), _iterable.end() };
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_REVERSE_ITERABLE_HPP
