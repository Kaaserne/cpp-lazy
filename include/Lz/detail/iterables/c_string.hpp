#pragma once

#ifndef LZ_C_STRING_ITERABLE_HPP
#define LZ_C_STRING_ITERABLE_HPP

#include <Lz/detail/iterators/c_string.hpp>

namespace lz {
namespace detail {
template<class C>
class c_string_iterable : public lazy_view {
    C* _begin{};

public:
    using iterator = c_string_iterator<C>;
    using sentinel = typename c_string_iterator<C>::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr c_string_iterable() noexcept = default;

    constexpr c_string_iterable(C* begin) noexcept : _begin{ begin } {
    }

    LZ_NODISCARD constexpr iterator begin() const noexcept {
        return { _begin };
    }

    LZ_NODISCARD constexpr default_sentinel end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_C_STRING_ITERABLE_HPP