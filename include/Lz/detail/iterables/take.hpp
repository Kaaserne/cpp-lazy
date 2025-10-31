#pragma once

#ifndef LZ_TAKE_ITERABLE_HPP
#define LZ_TAKE_ITERABLE_HPP

#include <Lz/detail/iterators/take.hpp>
#include <Lz/detail/maybe_owned.hpp>
#include <Lz/detail/procs/next_fast.hpp>
#include <Lz/detail/traits/is_iterable.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/traits/lazy_view.hpp>
#include <Lz/util/default_sentinel.hpp>

namespace lz {
namespace detail {
template<class Iterable, class = void>
class take_iterable;

template<class Iterator>
class take_iterable<Iterator, enable_if_t<!is_iterable<Iterator>::value>> : public lazy_view {
public:
    using iterator = n_take_iterator<Iterator>;
    using const_iterator = Iterator;
    using value_type = typename iterator::value_type;

private:
    Iterator _iterator{};
    typename iterator::difference_type _n{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr take_iterable()
        requires(std::default_initializable<Iterator>)
    = default;

#else

    template<class I = Iterator, class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr take_iterable() {
    }

#endif

    constexpr take_iterable(Iterator it, const typename iterator::difference_type n) : _iterator{ std::move(it) }, _n{ n } {
    }

    LZ_NODISCARD constexpr size_t size() const noexcept {
        return static_cast<size_t>(_n);
    }

    LZ_NODISCARD constexpr iterator begin() const {
        return { _iterator, _n };
    }


#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (is_bidi_tag_v<typename iterator::iterator_category>) {
            return iterator{ std::next(_iterator, _n), 0 };
        }
        else {
            return lz::default_sentinel;
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if_t<is_bidi_tag<I>::value, iterator> end() const {
        return { std::next(_iterator, _n), 0 };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD constexpr enable_if_t<!is_bidi_tag<I>::value, default_sentinel_t> end() const {
        return {};
    }

#endif
};

template<class Iterable>
class take_iterable<Iterable, enable_if_t<is_iterable<Iterable>::value>> : public lazy_view {
public:
    using iterator = iter_t<Iterable>;
    using const_iterator = iterator;
    using value_type = val_t<iterator>;

private:
    maybe_owned<Iterable> _iterable{};
    diff_type<iterator> _n{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr take_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr take_iterable() {
    }

#endif

    template<class I>
    constexpr take_iterable(I&& iterable, const diff_type<iterator> n) : _iterable{ std::forward<I>(iterable) }, _n{ n } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterable>)
    {
        return std::min(static_cast<size_t>(_n), static_cast<size_t>(lz::size(_iterable)));
    }

#else

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if_t<is_sized<I>::value, size_t> size() const {
        return std::min(static_cast<size_t>(_n), static_cast<size_t>(lz::size(_iterable)));
    }

#endif

    LZ_NODISCARD constexpr iterator begin() const {
        return _iterable.begin();
    }

    LZ_NODISCARD constexpr iterator end() const {
        return next_fast_safe(_iterable, static_cast<diff_type<iterator>>(_n));
    }
};
} // namespace detail
} // namespace lz
#endif
