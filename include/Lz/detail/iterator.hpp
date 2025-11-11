#pragma once

#ifndef LZ_DETAIL_ITERATOR_HPP
#define LZ_DETAIL_ITERATOR_HPP

#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/procs/sentinel_operators.hpp>
#include <iterator>

namespace lz {
namespace detail {

template<class Derived, class, class, class, class, class S = Derived>
struct iterator;

template<class Derived, class Reference, class Pointer, class DifferenceType, class S>
struct iterator<Derived, Reference, Pointer, DifferenceType, std::input_iterator_tag, S> {
    using iterator_category = std::input_iterator_tag;
    using sentinel = S;

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#endif

    LZ_CONSTEXPR_CXX_14 Derived& operator++() {
        static_cast<Derived&>(*this).increment();
        return static_cast<Derived&>(*this);
    }

    LZ_CONSTEXPR_CXX_14 Derived operator++(int) {
        Derived copy = static_cast<Derived&>(*this);
        static_cast<Derived&>(*this).increment();
        return copy;
    }

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

    LZ_NODISCARD constexpr Reference operator*() const {
        return static_cast<const Derived&>(*this).dereference();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 Reference operator*() {
        return static_cast<Derived&>(*this).dereference();
    }

    LZ_NODISCARD constexpr Pointer operator->() const {
        return static_cast<const Derived&>(*this).arrow();
    }

    LZ_NODISCARD constexpr bool operator==(const sentinel& s) const {
        return static_cast<const Derived&>(*this).eq(s);
    }

    LZ_NODISCARD constexpr bool operator!=(const sentinel& s) const {
        return !(*this == s);
    }

    LZ_NODISCARD friend constexpr bool operator==(const Derived& a, const Derived& d) {
        return a.eq(d);
    }

    LZ_NODISCARD friend constexpr bool operator!=(const Derived& a, const Derived& d) {
        return !(a == d);
    }
};

template<class Derived, class Reference, class Pointer, class DifferenceType, class S>
struct iterator<Derived, Reference, Pointer, DifferenceType, std::forward_iterator_tag, S>
    : public iterator<Derived, Reference, Pointer, DifferenceType, std::input_iterator_tag, S> {
    using iterator_category = std::forward_iterator_tag;
};

template<class Derived, class Reference, class Pointer, class DifferenceType, class S>
struct iterator<Derived, Reference, Pointer, DifferenceType, std::bidirectional_iterator_tag, S>
    : public iterator<Derived, Reference, Pointer, DifferenceType, std::forward_iterator_tag, S> {
    using iterator_category = std::bidirectional_iterator_tag;

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#endif

    LZ_CONSTEXPR_CXX_14 Derived& operator--() {
        static_cast<Derived&>(*this).decrement();
        return static_cast<Derived&>(*this);
    }

    LZ_CONSTEXPR_CXX_14 Derived operator--(int) {
        Derived copy = static_cast<Derived&>(*this);
        static_cast<Derived&>(*this).decrement();
        return copy;
    }

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
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

    LZ_NODISCARD constexpr friend DifferenceType operator-(const Derived& a, const Derived& d) {
        return a.difference(d);
    }

    LZ_NODISCARD constexpr DifferenceType operator-(const S& s) const {
        return static_cast<const Derived&>(*this).difference(s);
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

    LZ_NODISCARD constexpr Reference operator[](DifferenceType n) const {
        return *(*this + n);
    }

    LZ_NODISCARD constexpr friend bool operator<(const Derived& a, const Derived& d) {
        return d - a > 0;
    }

    LZ_NODISCARD constexpr friend bool operator>(const Derived& a, const Derived& d) {
        return d < a;
    }

    LZ_NODISCARD constexpr friend bool operator<=(const Derived& a, const Derived& d) {
        return !(d < a);
    }

    LZ_NODISCARD constexpr friend bool operator>=(const Derived& a, const Derived& d) {
        return !(a < d);
    }

    LZ_NODISCARD constexpr bool operator<(const S& s) const {
        return (*this - s) < 0;
    }

    LZ_NODISCARD constexpr bool operator>(const S& s) const {
        return s < *this;
    }

    LZ_NODISCARD constexpr bool operator<=(const S& s) const {
        return !(s < *this);
    }

    LZ_NODISCARD constexpr bool operator>=(const S& s) const {
        return !(*this < s);
    }
};

#ifdef LZ_HAS_CXX_20

template<class Derived, class Reference, class Pointer, class DifferenceType, class S>
struct iterator<Derived, Reference, Pointer, DifferenceType, std::contiguous_iterator_tag, S>
    : public iterator<Derived, Reference, Pointer, DifferenceType, std::random_access_iterator_tag, S> {
    using iterator_category = std::contiguous_iterator_tag;
};

#endif
} // namespace detail
} // namespace lz
#endif // LZ_ITERATOR_HPP
