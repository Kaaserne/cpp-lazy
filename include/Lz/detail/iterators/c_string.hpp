#pragma once

#ifndef LZ_C_STRING_ITERATOR_HPP
#define LZ_C_STRING_ITERATOR_HPP

#include "Lz/detail/traits.hpp"
#include "Lz/iterator_base.hpp"

namespace lz {
namespace detail {

template<class C, class Tag>
class c_string_iterator : public iter_base<c_string_iterator<C, Tag>, decltype(*std::declval<C>()), C, std::ptrdiff_t, Tag,
                                           sentinel_selector<Tag, c_string_iterator<C, Tag>>> {

    C _it;

    using ptr_removed = typename std::remove_pointer<C>::type;

public:
    using value_type = typename std::remove_const<ptr_removed>::type;
    using difference_type = std::ptrdiff_t;
    using pointer = C;
    using reference = decltype(*std::declval<C>());

    constexpr c_string_iterator(C it) noexcept : _it(it) {
    }

    constexpr c_string_iterator() = default;

    LZ_NODISCARD constexpr reference dereference() const noexcept {
        return *_it;
    }

    LZ_NODISCARD constexpr pointer arrow() const noexcept {
        return _it;
    }

    LZ_CONSTEXPR_CXX_14 void increment() noexcept {
        ++_it;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(const c_string_iterator& b) const noexcept {
        if (b._it == nullptr) {
            if (_it == nullptr) {
                return true;
            }
            return *_it == '\0';
        }
        return _it == b._it;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(default_sentinel) const noexcept {
        return _it != nullptr && *_it == '\0';
    }

    LZ_CONSTEXPR_CXX_14 void decrement() noexcept {
        --_it;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) noexcept {
        _it += offset;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 difference_type difference(const c_string_iterator& b) const noexcept {
        LZ_ASSERT(_it != nullptr && b._it != nullptr, "Incompatible iterator types");
        return _it - b._it;
    }

    LZ_NODISCARD constexpr explicit operator bool() const noexcept {
        return _it != nullptr && *_it != '\0';
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_C_STRING_ITERATOR_HPP