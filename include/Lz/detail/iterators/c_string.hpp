#pragma once

#ifndef LZ_C_STRING_ITERATOR_HPP
#define LZ_C_STRING_ITERATOR_HPP

#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class C>
class c_string_iterator
    : public iterator<c_string_iterator<C>, C&, C*, std::ptrdiff_t, std::forward_iterator_tag, default_sentinel_t> {

    C* _it{ nullptr };

public:
    using value_type = typename std::remove_const<C>::type;
    using difference_type = std::ptrdiff_t;
    using pointer = C*;
    using reference = C&;

    constexpr c_string_iterator() noexcept = default;

    explicit constexpr c_string_iterator(C* it) noexcept : _it{ it } {
    }

    LZ_CONSTEXPR_CXX_14 c_string_iterator& operator=(default_sentinel_t) noexcept {
        _it = nullptr;
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const noexcept {
        LZ_ASSERT(_it != nullptr, "Cannot dereference nullptr");
        return *_it;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const noexcept {
        LZ_ASSERT(_it != nullptr, "Cannot dereference nullptr");
        return _it;
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        LZ_ASSERT(_it != nullptr, "Cannot increment nullptr");
        ++_it;
    }

    constexpr bool eq(const c_string_iterator& other) const noexcept {
        return other._it == nullptr ? _it == nullptr || *_it == '\0' : _it == other._it;
    }

    constexpr bool eq(default_sentinel_t) const noexcept {
        return _it == nullptr || *_it == '\0';
    }

    constexpr explicit operator bool() const noexcept {
        return _it != nullptr && *_it != '\0';
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_C_STRING_ITERATOR_HPP