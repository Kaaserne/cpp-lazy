#pragma once

#ifndef LZ_GENERATE_HPP
#define LZ_GENERATE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/generate.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Generates n amount of elements using a generator function. Is a forward iterable, contains a .size() function and
 * returns a sentinel. Example:
 * ```cpp
 * lz::generate([]() { return 10; }, 5); // Generates 5 times the number 10
 * // or
 * lz::generate([]() { return 10; }); // Generates infinite times the number 10
 * ```
 */
constexpr detail::generate_adaptor generate{};

#else

/**
 * @brief Generates n amount of elements using a generator function. Is a forward iterable, contains a .size() function and
 * returns a sentinel. Example:
 * ```cpp
 * lz::generate([]() { return 10; }, 5); // Generates 5 times the number 10
 * // or
 * lz::generate([]() { return 10; }); // Generates infinite times the number 10
 * ```
 */
LZ_INLINE_VAR constexpr detail::generate_adaptor generate{};

#endif

using detail::generate_iterable;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif