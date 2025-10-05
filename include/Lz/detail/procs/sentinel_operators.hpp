#pragma once

#ifndef LZ_DETAIL_PROCS_SENTINEL_OPERATORS_HPP
#define LZ_DETAIL_PROCS_SENTINEL_OPERATORS_HPP

#include <Lz/detail/compiler_checks.hpp>

namespace lz {
namespace detail {

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class S>
struct iterator;

} // namespace detail
} // namespace lz

LZ_MODULE_EXPORT namespace lz {

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class Sentinel>
constexpr bool
operator==(const Sentinel& sent, const detail::iterator<Derived, Reference, Pointer, DifferenceType, IterCat, Sentinel>& it) {
    return it.operator==(sent);
}

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class Sentinel>
constexpr bool
operator!=(const Sentinel& sent, const detail::iterator<Derived, Reference, Pointer, DifferenceType, IterCat, Sentinel>& it) {
    return !(it == sent);
}

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class Sentinel>
LZ_NODISCARD constexpr bool
operator<(const Sentinel& sent, const detail::iterator<Derived, Reference, Pointer, DifferenceType, IterCat, Sentinel>& it) {
    return sent - it < 0;
}

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class Sentinel>
LZ_NODISCARD constexpr bool
operator>(const Sentinel& sent, const detail::iterator<Derived, Reference, Pointer, DifferenceType, IterCat, Sentinel>& it) {
    return it < sent;
}

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class Sentinel>
LZ_NODISCARD constexpr bool
operator<=(const Sentinel& sent, const detail::iterator<Derived, Reference, Pointer, DifferenceType, IterCat, Sentinel>& it) {
    return !(it < sent);
}

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class Sentinel>
LZ_NODISCARD constexpr bool
operator>=(const Sentinel& sent, const detail::iterator<Derived, Reference, Pointer, DifferenceType, IterCat, Sentinel>& it) {
    return !(sent < it);
}

template<class Derived, class Reference, class Pointer, class DifferenceType, class IterCat, class Sentinel>
LZ_NODISCARD constexpr typename Derived::difference_type
operator-(const Sentinel& sent, const detail::iterator<Derived, Reference, Pointer, DifferenceType, IterCat, Sentinel>& it) {
    return -(it - sent);
}

} // namespace lz

#endif // LZ_DETAIL_PROCS_SENTINEL_OPERATORS_HPP
