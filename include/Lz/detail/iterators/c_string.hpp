#pragma once

#ifndef LZ_C_STRING_ITERATOR_HPP
#define LZ_C_STRING_ITERATOR_HPP

#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class C>
class c_string_iterator : public iterator<c_string_iterator<C>, C&, C*, std::ptrdiff_t,
                                          std::forward_iterator_tag, default_sentinel> {

    C* _it{};

public:
    using value_type = typename std::remove_const<C>::type;
    using difference_type = std::ptrdiff_t;
    using pointer = C*;
    using reference = C&;

    constexpr c_string_iterator(C* it) noexcept : _it{ it } {
    }

    constexpr c_string_iterator() = default;

    constexpr reference dereference() const noexcept {
        LZ_ASSERT(_it != nullptr, "Cannot dereference a nullptr");
        return *_it;
    }

    constexpr pointer arrow() const noexcept {
        LZ_ASSERT(_it != nullptr, "Cannot dereference a nullptr");
        return _it;
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        LZ_ASSERT(_it != nullptr, "Cannot increment a nullptr");
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
} // namespace detail
} // namespace lz

#endif // LZ_C_STRING_ITERATOR_HPP