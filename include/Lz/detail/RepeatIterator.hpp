#pragma once

#ifndef LZ_REPEAT_ITERATOR_HPP
#define LZ_REPEAT_ITERATOR_HPP

#include "LzTools.hpp"

#include <limits>

namespace lz {
namespace internal {
template<class T>
class RepeatIterator {
    mutable T _toRepeat{};
    std::size_t _amount{};
    std::size_t _iterator{};

public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    constexpr RepeatIterator(T toRepeat, const std::size_t start, const std::size_t amount) :
        _toRepeat(std::move(toRepeat)),
        _amount(amount),
        _iterator(start) {
    }

    constexpr RepeatIterator() = default;

    LZ_NODISCARD constexpr reference operator*() const noexcept {
        return _toRepeat;
    }

    LZ_NODISCARD constexpr pointer operator->() const noexcept {
        return &_toRepeat;
    }

    LZ_CONSTEXPR_CXX_14 RepeatIterator& operator++() noexcept {
        if (_amount != (std::numeric_limits<std::size_t>::max)()) {
            ++_iterator;
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 RepeatIterator operator++(int) noexcept {
        RepeatIterator tmp(*this);
        ++*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_14 RepeatIterator& operator--() noexcept {
        if (_amount != (std::numeric_limits<std::size_t>::max)()) {
            --_iterator;
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 RepeatIterator operator--(int) noexcept {
        RepeatIterator tmp(*this);
        --*this;
        return tmp;
    }

    LZ_CONSTEXPR_CXX_14 RepeatIterator& operator+=(const difference_type offset) noexcept {
        if (_amount != (std::numeric_limits<std::size_t>::max)()) {
            _iterator += static_cast<std::size_t>(offset);
        }
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 RepeatIterator& operator-=(const difference_type offset) noexcept {
        if (_amount != (std::numeric_limits<std::size_t>::max)()) {
            _iterator -= static_cast<std::size_t>(offset);
        }
        return *this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 RepeatIterator operator+(const difference_type offset) const noexcept {
        RepeatIterator tmp(*this);
        tmp += offset;
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 RepeatIterator operator-(const difference_type offset) const noexcept {
        RepeatIterator tmp(*this);
        tmp -= static_cast<std::size_t>(offset);
        return tmp;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend difference_type operator-(const RepeatIterator& a, const RepeatIterator& b) noexcept {
        LZ_ASSERT(a._amount == b._amount, "incompatible iterator types: amount of times to repeat not the same");
        return static_cast<difference_type>(a._iterator - b._iterator);
    }

    LZ_NODISCARD constexpr reference operator[](const difference_type) const noexcept {
        return **this;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator!=(const RepeatIterator& a, const RepeatIterator& b) noexcept {
        LZ_ASSERT(a._amount == b._amount, "incompatible iterator types: amount of times to repeat not the same");
        return a._iterator != b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator==(const RepeatIterator& a, const RepeatIterator& b) noexcept {
        return !(a != b); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator<(const RepeatIterator& a, const RepeatIterator& b) noexcept {
        LZ_ASSERT(a._amount == b._amount, "incompatible iterator types: amount of times to repeat not the same");
        return a._iterator < b._iterator;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator>(const RepeatIterator& a, const RepeatIterator& b) noexcept {
        return b < a;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator<=(const RepeatIterator& a, const RepeatIterator& b) noexcept {
        return !(b < a); // NOLINT
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator>=(const RepeatIterator& a, const RepeatIterator& b) noexcept {
        return !(a < b); // NOLINT
    }
};
} // namespace internal
} // namespace lz

#endif