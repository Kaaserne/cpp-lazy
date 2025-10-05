#pragma once

#ifndef LZ_DETAIL_PROCS_OPERATORS_HPP
#define LZ_DETAIL_PROCS_OPERATORS_HPP

#include <Lz/detail/compiler_checks.hpp>

namespace lz {
namespace detail {

struct less {
    template<class T, class U>
    LZ_NODISCARD constexpr auto operator()(T&& lhs, U&& rhs) const
        noexcept(noexcept(static_cast<T&&>(lhs) < static_cast<U&&>(rhs))) -> decltype(static_cast<T&&>(lhs) <
                                                                                      static_cast<U&&>(rhs)) {
        return static_cast<T&&>(lhs) < static_cast<U&&>(rhs);
    }
};

struct equal_to {
    template<class T, class U>
    LZ_NODISCARD constexpr auto operator()(T&& lhs, U&& rhs) const
        noexcept(noexcept(static_cast<T&&>(lhs) == static_cast<U&&>(rhs))) -> decltype(static_cast<T&&>(lhs) ==
                                                                                       static_cast<U&&>(rhs)) {
        return static_cast<T&&>(lhs) == static_cast<U&&>(rhs);
    }
};

struct plus {
    template<class T, class U>
    LZ_NODISCARD constexpr auto operator()(T&& lhs, U&& rhs) const
        noexcept(noexcept(static_cast<T&&>(lhs) + static_cast<U&&>(rhs))) -> decltype(static_cast<T&&>(lhs) +
                                                                                      static_cast<U&&>(rhs)) {
        return static_cast<T&&>(lhs) + static_cast<U&&>(rhs);
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_PROCS_OPERATORS_HPP
