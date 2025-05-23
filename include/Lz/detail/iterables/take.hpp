#pragma once

#include "Lz/detail/compiler_checks.hpp"
#include "Lz/detail/traits.hpp"
#include "Lz/iterator_base.hpp"
#ifndef LZ_TAKE_ITERABLE_HPP
#define LZ_TAKE_ITERABLE_HPP

#include <Lz/detail/iterators/take.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
// TODO take for iterator too, add benchmark too
// TODO also add next_fast for implementations

template<class Iterable, class = void>
class take_iterable;

template<class Iterator>
class take_iterable<Iterator, enable_if<!is_iterable<Iterator>::value>> : public lazy_view {
public:
    using iterator = n_take_iterator<Iterator>;
    using const_iterator = Iterator;
    using value_type = typename iterator::value_type;

private:
    using diff = typename iterator::difference_type;

    Iterator _iterator;
    diff _n;

public:
    constexpr take_iterable(Iterator it, const diff n) : _iterator{ std::move(it) }, _n{ n } {
    }

    LZ_NODISCARD constexpr std::size_t size() const {
        return static_cast<std::size_t>(_n);
    }

    LZ_NODISCARD constexpr iterator begin() const& {
        return { _iterator, _n };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        return { std::move(_iterator), _n };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if<is_bidi_tag<I>::value, iterator> end() const {
        return { std::next(_iterator, _n), 0 };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if<!is_bidi_tag<I>::value, default_sentinel> end() const {
        return {};
    }
};

template<class Iterable>
class take_iterable<Iterable, enable_if<is_iterable<Iterable>::value>> : public lazy_view {
    ref_or_view<Iterable> _iterable;
    diff_iterable_t<Iterable> _n;

    using inner_iter = iter_t<Iterable>;
    using inner_sentinel = sentinel_t<Iterable>;

    constexpr std::size_t size_impl() const {
        return std::min(static_cast<std::size_t>(_n), static_cast<std::size_t>(lz::eager_size(_iterable)));
    }

public:
    using iterator = bounded_take_iterator<inner_iter, inner_sentinel>;
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
    LZ_NODISCARD constexpr enable_if<is_ra_tag<I>::value, iterator> end() const {
        return { std::begin(_iterable) + static_cast<typename iterator::difference_type>(size_impl()) };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if<!is_ra_tag<I>::value, inner_iter> end() const {
        return { next_fast(_iterable, static_cast<typename iterator::difference_type>(size_impl())) };
    }
};
} // namespace detail
} // namespace lz
#endif