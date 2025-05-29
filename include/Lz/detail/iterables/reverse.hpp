#pragma once

#ifndef LZ_REVERSE_ITERABLE_HPP
#define LZ_REVERSE_ITERABLE_HPP

#include <Lz/detail/ref_or_view.hpp>

// TODO add more efficient reverse iterator implementation

namespace lz {
namespace detail {
template<class Iterable>
class reverse_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;

public:
    using iterator = std::reverse_iterator<iter_t<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I>
    constexpr reverse_iterable(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(lz::size(_iterable));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 iterator begin() const& {
        return iterator(std::end(_iterable));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 iterator end() const& {
        return iterator(std::begin(_iterable));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 iterator begin() && {
        return iterator(detail::end(std::move(_iterable)));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_17 iterator end() && {
        return iterator(detail::begin(std::move(_iterable)));
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_REVERSE_ITERABLE_HPP
