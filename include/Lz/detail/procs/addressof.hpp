#pragma once

#ifndef LZ_DETAIL_PROCS_ADDRESSOF_HPP
#define LZ_DETAIL_PROCS_ADDRESSOF_HPP

#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/traits/enable_if.hpp>
#include <type_traits>

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class T>
[[nodiscard]] constexpr T* addressof(T& arg) noexcept {
    if constexpr (std::is_object_v<T>) {
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#endif
        return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(arg)));
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    }
    else {
        return &arg;
    }
}

template<class T>
LZ_NODISCARD constexpr enable_if_t<!std::is_object<T>::value, T*> addressof(T& arg) noexcept {
    return &arg;
}

#else

template<class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17 enable_if_t<std::is_object<T>::value, T*> addressof(T& arg) noexcept {
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#endif
    return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(arg)));
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
}

template<class T>
LZ_NODISCARD constexpr enable_if_t<!std::is_object<T>::value, T*> addressof(T& arg) noexcept {
    return &arg;
}

#endif

} // namespace detail
} // namespace lz

#endif
