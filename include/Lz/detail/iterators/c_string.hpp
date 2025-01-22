#pragma once

#ifndef LZ_C_STRING_ITERATOR_HPP
#define LZ_C_STRING_ITERATOR_HPP

#include "Lz/detail/traits.hpp"
#include "Lz/iterator_base.hpp"

namespace lz {
namespace detail {

template<class C>
class c_string_iterator : public iter_base<c_string_iterator<C>, decltype(*std::declval<C>()), C, std::ptrdiff_t,
                                           std::forward_iterator_tag, default_sentinel> {

    C _it;

    using ptr_removed = typename std::remove_pointer<C>::type;

public:
    using value_type = typename std::remove_const<ptr_removed>::type;
    using difference_type = std::ptrdiff_t;
    using pointer = C;
    using reference = decltype(*std::declval<C>());

    constexpr c_string_iterator(C it) noexcept : _it{ it } {
    }

    constexpr c_string_iterator() = default;

    constexpr reference dereference() const noexcept {
        return *_it;
    }

    constexpr pointer arrow() const noexcept {
        return _it;
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        ++_it;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const c_string_iterator& b) const noexcept {
        if (b._it == nullptr) {
            if (_it == nullptr) {
                return true;
            }
            return *_it == '\0';
        }
        return _it == b._it;
    }

    constexpr bool eq(default_sentinel) const noexcept {
        return _it != nullptr && *_it == '\0';
    }

    constexpr explicit operator bool() const noexcept {
        return _it != nullptr && *_it != '\0';
    }
};

template<class C>
class c_string_iterable {

    static_assert(std::is_pointer<C>::value, "C must be a pointer");

    C _begin;

public:
    using iterator = c_string_iterator<C>;
    using sentinel = typename c_string_iterator<C>::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr c_string_iterable(C begin) noexcept : _begin(begin) {
    }

    constexpr c_string_iterable() = default;

    LZ_NODISCARD constexpr iterator begin() const noexcept {
        return { _begin };
    }

    LZ_NODISCARD constexpr sentinel end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_C_STRING_ITERATOR_HPP