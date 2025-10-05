#pragma once

#ifndef LZ_DETAIL_PROCS_BEGIN_END_HPP
#define LZ_DETAIL_PROCS_BEGIN_END_HPP

#include <Lz/detail/compiler_checks.hpp>

namespace lz {
namespace detail {

template<class C>
LZ_NODISCARD constexpr auto begin(C& c) -> decltype(c.begin()) {
    return c.begin();
}

template<class C>
LZ_NODISCARD constexpr auto end(C& c) -> decltype(c.end()) {
    return c.end();
}

template<class C, size_t N>
LZ_NODISCARD constexpr C* begin(C (&c)[N]) noexcept {
    return c;
}

template<class C, size_t N>
LZ_NODISCARD constexpr C* end(C (&c)[N]) noexcept {
    return c + N;
}

} // namespace detail
} // namespace lz

#endif
