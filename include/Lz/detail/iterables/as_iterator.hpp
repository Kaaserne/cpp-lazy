#pragma once

#ifndef LZ_AS_ITERATOR_ITERABLE_HPP
#define LZ_AS_ITERATOR_ITERABLE_HPP

#include <Lz/detail/iterators/as_iterator.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {

template<class Iterable, class IterCat>
class as_iterator_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;

    using sentinel = sentinel_t<Iterable>;

public:
    using value_type = iter_t<Iterable>;
    using iterator = as_iterator_iterator<value_type, sentinel_t<Iterable>, IterCat>;
    using const_iterator = iterator;

#ifdef LZ_HAS_CONCEPTS

    constexpr as_iterator_iterable()
        requires std::default_initializable<ref_or_view<Iterable>>
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr as_iterator_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    explicit constexpr as_iterator_iterable(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

    template<class T = sized<Iterable>>
    LZ_NODISCARD constexpr enable_if<T::value, std::size_t> size() const {
        return static_cast<std::size_t>(lz::size(_iterable));
    }

    LZ_NODISCARD constexpr iterator begin() const {
        return iterator{ std::begin(_iterable) };
    }

#ifdef LZ_HAS_CXX_17

    LZ_NODISCARD constexpr auto end() const {
        if constexpr (!is_sentinel<value_type, sentinel>::value) {
            return iterator{ std::end(_iterable) };
        }
        else {
            return std::end(_iterable);
        }
    }

#else

    template<class I = value_type>
    LZ_NODISCARD constexpr enable_if<!is_sentinel<I, sentinel>::value, iterator> end() const& {
        return iterator{ std::end(_iterable) };
    }

    template<class I = value_type>
    LZ_NODISCARD constexpr enable_if<is_sentinel<I, sentinel>::value, sentinel> end() const& {
        return std::end(_iterable);
    }

#endif // LZ_HAS_CXX_17
};

} // namespace detail
} // namespace lz

#endif // LZ_AS_ITERATOR_ITERABLE_HPP
