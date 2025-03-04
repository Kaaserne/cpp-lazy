#pragma once

#ifndef LZ_C_STRING_ADAPTOR_HPP
#define LZ_C_STRING_ADAPTOR_HPP

#include <Lz/detail/iterables/c_string.hpp>

namespace lz {
namespace detail {
struct c_string_adaptor {
    using adaptor = c_string_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor c_string;

#endif

    /**
     * @brief This adaptor is used to create a c forward iterable cstring iterable object. Its end() function will return a
     * sentinel, rather than an actual iterator. This iterable does not contain a .size() method. Example:
     * ```cpp
     * const char* str = "Hello, World!";
     * auto cstr = lz::c_string(str);
     * // or:
     * const char str[] = "Hello, World!";
     * auto cstr = str | lz::c_string;
     * ```
     * @param str The string to create a cstring iterable from.
     **/
    template<class C>
    LZ_NODISCARD constexpr c_string_iterable<decay_t<C>> operator()(C&& str) const noexcept {
        return { std::forward<C>(str) };
    }
};
} // namespace detail
} // namespace lz

#endif