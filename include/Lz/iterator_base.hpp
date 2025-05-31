#pragma once

#ifndef LZ_ITERATOR_HPP
#define LZ_ITERATOR_HPP

#include <Lz/detail/traits.hpp>
#include <iterator>

namespace lz {

template<class Derived, class Reference, class Pointer, class DifferenceType, class S>
struct iterator<Derived, Reference, Pointer, DifferenceType, std::forward_iterator_tag, S> {
    using iterator_category = std::forward_iterator_tag;
    using sentinel = S;

    LZ_CONSTEXPR_CXX_14 Derived& operator++() {
        static_cast<Derived&>(*this).increment();
        return static_cast<Derived&>(*this);
    }

    LZ_CONSTEXPR_CXX_14 Derived operator++(int) {
        Derived copy = static_cast<Derived&>(*this);
        static_cast<Derived&>(*this).increment();
        return copy;
    }

    LZ_NODISCARD constexpr Reference operator*() const {
        return static_cast<const Derived&>(*this).dereference();
    }

#ifdef LZ_HAS_CXX_17

    LZ_NODISCARD constexpr decltype(auto) operator*() {
        if constexpr (std::is_lvalue_reference<Reference>::value) {
            return const_cast<Reference>(static_cast<const iterator&>(*this).operator*());
        }
        else {
            return static_cast<const iterator&>(*this).operator*();
        }
    }

#else

    template<class T = Reference>
    LZ_NODISCARD constexpr detail::enable_if<std::is_lvalue_reference<T>::value, T> operator*() {
        return const_cast<T>(static_cast<const iterator&>(*this).operator*());
    }

    template<class T = Reference>
    LZ_NODISCARD constexpr detail::enable_if<!std::is_lvalue_reference<T>::value, T> operator*() {
        return static_cast<const iterator&>(*this).operator*();
    }

#endif

    LZ_NODISCARD constexpr Pointer operator->() const {
        return static_cast<const Derived&>(*this).arrow();
    }

    LZ_NODISCARD constexpr bool operator==(const sentinel& b) const {
        return static_cast<const Derived&>(*this).eq(b);
    }

    LZ_NODISCARD constexpr bool operator!=(const sentinel& b) const {
        return !(*this == b);
    }

    friend constexpr bool operator==(const Derived& a, const Derived& b) {
        return a.eq(b);
    }

    friend constexpr bool operator!=(const Derived& a, const Derived& b) {
        return !(a == b);
    }
};

template<class Derived, class Reference, class Pointer, class DifferenceType, class S>
struct iterator<Derived, Reference, Pointer, DifferenceType, std::bidirectional_iterator_tag, S>
    : public iterator<Derived, Reference, Pointer, DifferenceType, std::forward_iterator_tag, S> {
    using iterator_category = std::bidirectional_iterator_tag;

    LZ_CONSTEXPR_CXX_14 Derived& operator--() {
        static_cast<Derived&>(*this).decrement();
        return static_cast<Derived&>(*this);
    }

    LZ_CONSTEXPR_CXX_14 Derived operator--(int) {
        Derived copy = static_cast<Derived&>(*this);
        static_cast<Derived&>(*this).decrement();
        return copy;
    }
};

template<class Derived, class Reference, class Pointer, class DifferenceType, class S>
struct iterator<Derived, Reference, Pointer, DifferenceType, std::random_access_iterator_tag, S>
    : public iterator<Derived, Reference, Pointer, DifferenceType, std::bidirectional_iterator_tag, S> {
    using iterator_category = std::random_access_iterator_tag;

    LZ_CONSTEXPR_CXX_14 Derived& operator+=(const DifferenceType n) {
        static_cast<Derived&>(*this).plus_is(n);
        return static_cast<Derived&>(*this);
    }

    LZ_CONSTEXPR_CXX_14 Derived operator+(const DifferenceType n) const {
        auto tmp = static_cast<const Derived&>(*this);
        tmp += n;
        return tmp;
    }

    LZ_NODISCARD constexpr friend DifferenceType operator-(const Derived& a, const Derived& b) {
        return a.difference(b);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 Derived operator-(const DifferenceType n) const {
        Derived temp = static_cast<const Derived&>(*this);
        temp -= n;
        return temp;
    }

    LZ_CONSTEXPR_CXX_14 Derived& operator-=(const DifferenceType n) {
        *this += (-n);
        return *static_cast<Derived*>(this);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 Reference operator[](DifferenceType n) const {
        return *(*this + n);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator<(const Derived& a, const Derived& b) {
        return b - a > 0;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator>(const Derived& a, const Derived& b) {
        return b < a;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator<=(const Derived& a, const Derived& b) {
        return !(b < a);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 friend bool operator>=(const Derived& a, const Derived& b) {
        return !(a < b);
    }
};
} // namespace lz
#endif // LZ_ITERATOR_HPP