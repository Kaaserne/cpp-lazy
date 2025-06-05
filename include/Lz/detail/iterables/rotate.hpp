#pragma once

#ifndef LZ_ROTATE_ITERABLE_HPP
#define LZ_ROTATE_ITERABLE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/iterators/rotate.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class rotate_iterable : public lazy_view {
    using inner_iter = iter_t<Iterable>;

    ref_or_view<Iterable> _iterable;
    inner_iter _start_iter;

public:
    using iterator = rotate_iterator<inner_iter, sentinel_t<Iterable>>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr rotate_iterable()
        requires std::default_initializable<Iterable> && std::default_initializable<inner_iter>
    = default;

#else

    template<class I = Iterable,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<inner_iter>::value>>
    constexpr rotate_iterable() {
    }

#endif

    template<class I>
    constexpr rotate_iterable(I&& iterable, const diff_type<inner_iter> start) :
        _iterable{ std::forward<I>(iterable) },
        _start_iter{ next_fast(_iterable, start) } {
    }

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(lz::size(_iterable));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _start_iter, 0 };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        return { detail::begin(std::move(_iterable)), detail::end(std::move(_iterable)), _start_iter, 0 };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return iterator{ std::begin(_iterable), std::end(_iterable), _start_iter, lz::eager_size(_iterable) };
        }
        else {
            return _start_iter;
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const {
        return { std::begin(_iterable), std::end(_iterable), _start_iter, lz::eager_size(_iterable) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, inner_iter> end() const {
        return _start_iter;
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_ROTATE_ITERABLE_HPP
