#pragma once

#ifndef LZ_ENUMERATE_ITERABLE_HPP
#define LZ_ENUMERATE_ITERABLE_HPP

#include <Lz/detail/iterators/enumerate.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {
template<class Iterable, class IntType>
class enumerate_iterable : public lazy_view {
    using iter = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;

    maybe_owned<Iterable> _iterable{};
    IntType _start{};

public:
    using iterator = enumerate_iterator<maybe_owned<Iterable>, IntType>;
    using const_iterator = iterator;
    using sentinel = sentinel_with<IntType>;
    using value_type = typename iterator::value_type;

private:
    static constexpr bool return_sentinel = !is_bidi_tag<iter_cat_t<iterator>>::value || has_sentinel<Iterable>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr enumerate_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr enumerate_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    constexpr enumerate_iterable(I&& iterable, const IntType start) : _iterable{ std::forward<I>(iterable) }, _start{ start } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterable>)
    {
        return static_cast<size_t>(lz::size(_iterable));
    }

#else

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<is_sized<I>::value, size_t> size() const {
        return static_cast<size_t>(lz::size(_iterable));
    }

#endif

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr iterator begin() const {
        if constexpr (is_bidi_v<iter>) {
            return { _iterable, _iterable.begin(), _start };
        }
        else {
            return { _iterable.begin(), _start };
        }
    }

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable, _iterable.end(), _start + static_cast<IntType>(lz::eager_size(_iterable)) };
        }
        else if (is_bidi_v<iter>) {
            return sentinel{ _iterable.end() };
        }
        else {
            return sentinel{ _start };
        }
    }

#else
    // TODO write tests for these permutations
    template<class it = iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<is_bidi<it>::value, iterator> begin() const {
        return { _iterable, _iterable.begin(), _start };
    }

    template<class it = iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!is_bidi<it>::value, iterator> begin() const {
        return { _iterable.begin(), _start };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!R, iterator> end() const {
        return { _iterable, _iterable.end(), _start + static_cast<IntType>(lz::eager_size(_iterable)) };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R && is_bidi<iter>::value, sentinel> end() const {
        return sentinel{ _start };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R && !is_bidi<iter>::value, sentinel> end() const {
        return sentinel{ _iterable.end() };
    }

#endif
};
} // namespace detail
} // namespace lz
#endif
