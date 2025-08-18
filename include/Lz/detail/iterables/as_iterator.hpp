#pragma once

#ifndef LZ_AS_ITERATOR_ITERABLE_HPP
#define LZ_AS_ITERATOR_ITERABLE_HPP

#include <Lz/detail/iterators/as_iterator.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {

template<class Iterable, class IterCat>
class as_iterator_iterable : public lazy_view {
    maybe_owned<Iterable> _iterable;

    using sentinel = sentinel_t<Iterable>;

public:
    using value_type = iter_t<Iterable>;
    using iterator = as_iterator_iterator<value_type, sentinel_t<Iterable>, IterCat>;
    using const_iterator = iterator;

#ifdef LZ_HAS_CONCEPTS

    constexpr as_iterator_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr as_iterator_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    explicit constexpr as_iterator_iterable(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterable>)
    {
        return static_cast<size_t>(lz::size(_iterable));
    }
#else

    template<class T = is_sized<Iterable>>
    LZ_NODISCARD constexpr enable_if_t<T::value, size_t> size() const {
        return static_cast<size_t>(lz::size(_iterable));
    }

#endif

    LZ_NODISCARD constexpr iterator begin() const {
        return iterator{ _iterable.begin() };
    }

#ifdef LZ_HAS_CXX_17

    LZ_NODISCARD constexpr auto end() const {
        if constexpr (!is_sentinel_v<value_type, sentinel>) {
            return iterator{ _iterable.end() };
        }
        else {
            return _iterable.end();
        }
    }

#else

    template<class I = value_type>
    LZ_NODISCARD constexpr enable_if_t<!is_sentinel<I, sentinel>::value, iterator> end() const& {
        return iterator{ _iterable.end() };
    }

    template<class I = value_type>
    LZ_NODISCARD constexpr enable_if_t<is_sentinel<I, sentinel>::value, sentinel> end() const& {
        return _iterable.end();
    }

#endif // LZ_HAS_CXX_17
};

} // namespace detail
} // namespace lz

#endif // LZ_AS_ITERATOR_ITERABLE_HPP
