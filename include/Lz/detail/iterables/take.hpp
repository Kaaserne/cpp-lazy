#pragma once

#ifndef LZ_TAKE_ITERABLE_HPP
#define LZ_TAKE_ITERABLE_HPP

#include <Lz/detail/iterators/take.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class take_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    diff_iterable_t<Iterable> _n;

    using inner_iter = iter_t<Iterable>;
    using inner_sentinel = sentinel_t<Iterable>;

    constexpr std::size_t size_impl() const {
        return std::min(static_cast<std::size_t>(_n), static_cast<std::size_t>(lz::eager_size(_iterable)));
    }

public:
    using iterator = take_iterator<inner_iter, inner_sentinel>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I>
    constexpr take_iterable(I&& iterable, typename iterator::difference_type n) :
        _iterable{ std::forward<I>(iterable) },
        _n{ n } {
    }

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const {
        return size_impl();
    }

    LZ_NODISCARD constexpr iterator begin() const {
        return { std::begin(_iterable) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if<is_bidi_tag<I>::value, iterator> end() const {
        return { std::begin(_iterable) + static_cast<typename iterator::difference_type>(size_impl()) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if<!is_bidi_tag<I>::value, inner_iter> end() const {
        return std::next(std::begin(_iterable), static_cast<typename iterator::difference_type>(size_impl()));
    }
};
} // namespace detail
} // namespace lz
#endif