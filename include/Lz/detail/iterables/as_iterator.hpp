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

    template<class I>
    constexpr as_iterator_iterable(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

    template<class T = sized<Iterable>>
    LZ_NODISCARD constexpr enable_if<T::value, std::size_t> size() const {
        return static_cast<std::size_t>(lz::size(_iterable));
    }

    LZ_NODISCARD constexpr iterator begin() const& {
        return { std::begin(_iterable) };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { detail::begin(std::move(_iterable)) };
    }

#ifdef LZ_HAS_CXX_17

    LZ_NODISCARD constexpr auto end() const& {
        if constexpr (std::is_same_v<value_type, sentinel>) {
            return iterator{ std::end(_iterable) };
        }
        else {
            return std::end(_iterable);
        }
    }

    LZ_NODISCARD constexpr auto end() && {
        if constexpr (std::is_same_v<value_type, sentinel>) {
            return iterator{ std::end(std::move(_iterable)) };
        }
        else {
            return std::end(std::move(_iterable));
        }
    }

#else

    template<class I = value_type>
    LZ_NODISCARD constexpr enable_if<std::is_same<I, sentinel>::value, iterator> end() const& {
        return { std::end(_iterable) };
    }

    template<class I = value_type>
    LZ_NODISCARD constexpr enable_if<!std::is_same<I, sentinel>::value, sentinel> end() const& {
        return std::end(_iterable);
    }

    template<class I = value_type>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<std::is_same<I, sentinel>::value, iterator> end() && {
        return { detail::end(std::move(_iterable)) };
    }

    template<class I = value_type>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!std::is_same<I, sentinel>::value, sentinel> end() && {
        return std::end(std::move(_iterable));
    }

#endif // LZ_HAS_CXX_17
};

} // namespace detail
} // namespace lz

#endif // LZ_AS_ITERATOR_ITERABLE_HPP
