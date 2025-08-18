#pragma once

#ifndef LZ_GENERATE_ADAPTOR_HPP
#define LZ_GENERATE_ADAPTOR_HPP

#include <Lz/detail/iterables/generate.hpp>

namespace lz {
namespace detail {
struct generate_adaptor {
    using adaptor = generate_adaptor;

    /**
     * @brief Generates n amount of elements using a generator function. Is a forward iterable, contains a .size() function and
     * returns a sentinel. Example:
     * ```cpp
     * lz::generate([]() { return 10; }, 5); // Generates 5 times the number 10
     * ```
     * @param generator_func The generator function that generates the elements
     * @param amount The amount of elements to generate
     * @return A generate_iterable that generates the elements @p amount times
     */
    template<class GeneratorFunc>
    LZ_NODISCARD constexpr generate_iterable<GeneratorFunc, false>
    operator()(GeneratorFunc generator_func, const size_t amount) const {
        return { std::move(generator_func), amount };
    }

    /**
     * @brief Generates infinite amount of elements using a generator function. Is a forward iterable, contains a .size() function
     * and returns a sentinel. Example:
     * ```cpp
     * lz::generate([]() { return 10; }); // Generates infinite times the number 10
     * ```
     * @param generator_func The generator function that generates the elements
     * @return A generate_iterable that infinitely generates the elements
     */
    template<class GeneratorFunc>
    LZ_NODISCARD constexpr generate_iterable<GeneratorFunc, true> operator()(GeneratorFunc generator_func) const {
        return generate_iterable<GeneratorFunc, true>{ std::move(generator_func) };
    }
};
} // namespace detail
} // namespace lz

#endif