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
class loop_iterable<Iterable, false /* is inf loop */> {
    using inner_iter = iter_t<Iterable>;

    ref_or_view<Iterable> _iterable;
    std::size_t _amount{};

public:
    using iterator = loop_iterator<inner_iter, sentinel_t<Iterable>, false>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr loop_iterable() = default;

    template<class I>
    constexpr loop_iterable(I&& iterable, std::size_t amount) : _iterable{ std::forward<I>(iterable) }, _amount{ amount } {
    }

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const {
        return _amount * lz::size(_iterable);
    }

    LZ_NODISCARD constexpr iterator begin() const {
        auto first = std::begin(_iterable);
        auto last = std::end(_iterable);
        if (first == last || _amount == 0) {
            return { last, first, last, 0 };
        }
        return { first, first, last, _amount };
    }

    template<class I = inner_iter>
    LZ_NODISCARD constexpr enable_if<is_bidi<I>::value, iterator> end() const {
        return { std::end(_iterable), std::begin(_iterable), std::end(_iterable), 0 };
    }

    template<class I = inner_iter>
    LZ_NODISCARD constexpr enable_if<!is_bidi<I>::value, default_sentinel> end() const {
        return {};
    }
};

template<class Iterable>
class loop_iterable<Iterable, true /* is inf loop */> {
    using inner_iter = iter_t<Iterable>;

    ref_or_view<Iterable> _iterable;

public:
    using iterator = loop_iterator<inner_iter, sentinel_t<Iterable>, true>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr loop_iterable() = default;

    template<class I>
    constexpr loop_iterable(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

    LZ_NODISCARD constexpr iterator begin() const {
        return { std::begin(_iterable), std::end(_iterable) };
    }

    template<class I = inner_iter>
    LZ_NODISCARD constexpr default_sentinel end() const {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_LOOP_ITERALBE_HPP
