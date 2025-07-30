#pragma once

#ifndef LZ_GENERATE_HPP
#define LZ_GENERATE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/generate.hpp>

LZ_MODULE_EXPORT namespace lz {

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

/**
 * @brief Generate iterable helper alias for infinite generation.
 * @tparam GeneratorFunc The type of the generator function.
 * ```cpp
 * lz::generate_iterable_inf<std::function<int()>> = lz::generate([]() { return 10; });
 * ```
 */
template<class GeneratorFunc>
using generate_iterable_inf = detail::generate_iterable<GeneratorFunc, true>;

/**
 * @brief Generate iterable helper alias for finite generation.
 * @tparam GeneratorFunc The type of the generator function.
 * ```cpp
 * lz::generate_iterable<std::function<int()>> = lz::generate([]() { return 10; }, 10);
 * ```
 */
template<class GeneratorFunc>
using generate_iterable = detail::generate_iterable<GeneratorFunc, false>;

} // namespace lz

#endif