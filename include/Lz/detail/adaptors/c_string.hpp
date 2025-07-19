#pragma once

#ifndef LZ_C_STRING_ADAPTOR_HPP
#define LZ_C_STRING_ADAPTOR_HPP

#include <Lz/detail/iterables/c_string.hpp>

namespace lz {
namespace detail {
struct c_string_adaptor {
    using adaptor = c_string_adaptor;

    /**
     * @brief This adaptor is used to create a forward iterable cstring iterable object. Its end() function will return a
     * sentinel, rather than an actual iterator. This iterable does not contain a .size() method. Example:
     * ```cpp
     * const char* str = "Hello, World!";
     * auto cstr = lz::c_string(str);
     * // or:
     * const char str[] = "Hello, World!";
     * auto cstr = str | lz::c_string; // Only works for arrays
     * ```
     * @param str The string to create a cstring iterable from.
     * @return A c_string_iterable object that can be used to iterate over the characters in the string.
     **/
    template<class C>
    LZ_NODISCARD constexpr c_string_iterable<C> operator()(C* str) const noexcept {
        return { str };
    }

    /**
     * @brief This adaptor is used to create a forward iterable cstring iterable object. Its end() function will return a
     * sentinel, rather than an actual iterator. This iterable does not contain a .size() method. Example:
     * ```cpp
     * const char* str = "Hello, World!";
     * auto cstr = lz::c_string(str);
     * // or:
     * const char str[] = "Hello, World!";
     * auto cstr = str | lz::c_string; // Only works for arrays
     * ```
     * @param str The string to create a cstring iterable from.
     * @return A c_string_iterable object that can be used to iterate over the characters in the string.
     **/
    template<class C>
    LZ_NODISCARD constexpr c_string_iterable<const C> operator()(const C* str) const noexcept {
        return { str };
    }
};
} // namespace detail
} // namespace lz

#endif