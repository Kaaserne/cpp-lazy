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
    using iterator = loop_iterator<iter_t<Iterable>, sentinel_t<Iterable>, false>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I>
    constexpr loop_iterable(I&& iterable, const std::size_t amount) : _iterable{ std::forward<I>(iterable) }, _amount{ amount } {
    }

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const {
        return _amount * static_cast<std::size_t>(lz::size(_iterable));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        auto first = detail::begin(_iterable);
        auto last = detail::end(_iterable);
        if (_amount == 0) {
            first = last;
            return { first, first, last, _amount };
        }
        return { first, first, last, _amount - 1 };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        auto first = detail::begin(std::move(_iterable));
        auto last = detail::end(std::move(_iterable));
        if (_amount == 0) {
            first = last;
            return { first, first, last, _amount };
        }
        return { first, first, last, _amount - 1 };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            auto first = detail::begin(_iterable);
            auto last = detail::end(_iterable);
            if (_amount == 0) {
                first = last;
            }
            return iterator{ last, first, last, 0 };
        }
        else {
            return default_sentinel{};
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const {
        auto first = detail::begin(_iterable);
        auto last = detail::end(_iterable);
        if (_amount == 0) {
            first = last;
        }
        return { last, first, last, 0 };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if<!is_bidi_tag<I>::value, default_sentinel> end() const {
        return {};
    }

#endif
};

template<class Iterable>
class loop_iterable<Iterable, true /* is inf loop */> : public lazy_view {
    ref_or_view<Iterable> _iterable;

public:
    using iterator = loop_iterator<iter_t<Iterable>, sentinel_t<Iterable>, true>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I>
    constexpr loop_iterable(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), std::end(_iterable) };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { detail::begin(std::move(_iterable)), detail::end(std::move(_iterable)) };
    }

    LZ_NODISCARD constexpr default_sentinel end() const {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_LOOP_ITERALBE_HPP
