#pragma once

#ifndef LZ_LOOP_ITERALBE_HPP
#define LZ_LOOP_ITERALBE_HPP

#include <Lz/detail/iterators/loop.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class, bool /* is inf */>
class loop_iterable;

template<class Iterable>
class loop_iterable<Iterable, false /* is inf loop */> : public lazy_view {
    ref_or_view<Iterable> _iterable;
    std::size_t _amount{};

public:
    using iterator = loop_iterator<ref_or_view<Iterable>, false>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || is_sentinel<iter_t<Iterable>, sentinel_t<Iterable>>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr loop_iterable()
        requires std::default_initializable<Iterable>
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr loop_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    constexpr loop_iterable(I&& iterable, const std::size_t amount) : _iterable{ std::forward<I>(iterable) }, _amount{ amount } {
    }

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const {
        return _amount * static_cast<std::size_t>(lz::size(_iterable));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        auto first = std::begin(_iterable);
        if (_amount == 0) {
            first = std::end(_iterable);
            return { _iterable, first, _amount };
        }
        return { _iterable, first, _amount - 1 };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, iterator> begin() && {
        auto first = detail::begin(std::move(_iterable));
        if (_amount == 0) {
            first = detail::end(std::move(_iterable));
            return { _iterable, first, _amount };
        }
        return { _iterable, first, _amount - 1 };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable, std::end(_iterable), 0 };
        }
        else {
            return default_sentinel{};
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!R, iterator> end() const {
        return iterator{ _iterable, std::end(_iterable), 0 };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD constexpr enable_if<R, default_sentinel> end() const {
        return {};
    }

#endif
};

template<class Iterable>
class loop_iterable<Iterable, true /* is inf loop */> : public lazy_view {
    ref_or_view<Iterable> _iterable;

public:
    using iterator = loop_iterator<ref_or_view<Iterable>, true>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr loop_iterable()
        requires std::default_initializable<Iterable>
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr loop_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    constexpr loop_iterable(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { _iterable, std::begin(_iterable) };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { _iterable, detail::begin(std::move(_iterable)) };
    }

    LZ_NODISCARD constexpr default_sentinel end() const {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_LOOP_ITERALBE_HPP
