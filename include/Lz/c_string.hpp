#pragma once

#ifndef LZ_C_STRING_HPP
#define LZ_C_STRING_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/iterators/c_string.hpp>
#include <Lz/detail/traits.hpp>

namespace lz {

struct c_string_adaptor {
#ifdef LZ_HAS_CXX_11
    static c_string_adaptor c_string;
#endif

    using adaptor = c_string_adaptor;

    template<class C>
    LZ_NODISCARD constexpr detail::c_string_iterable<detail::decay_t<C>> operator()(C&& str) const noexcept {
        return { std::forward<C>(str) };
    }
};

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to create a c forward iterable cstring iterable object. Its end() function will return a sentinel,
 * rather than an actual iterator. Example:
 * ```cpp
 * const char* str = "Hello, World!";
 * auto cstr = lz::c_string(str);
 * // or:
 * auto cstr = "Hello" | lz::c_string;
 * ```
 */
c_string_adaptor c_string_adaptor::c_string{};

#else

/**
 * @brief This adaptor is used to create a c forward iterable cstring iterable object. Its end() function will return a sentinel,
 * rather than an actual iterator. Example:
 * ```cpp
 * const char* str = "Hello, World!";
 * auto cstr = lz::c_string(str);
 * // or:
 * auto cstr = "Hello" | lz::c_string;
 * ```
 */
LZ_INLINE_VAR constexpr c_string_adaptor c_string{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif