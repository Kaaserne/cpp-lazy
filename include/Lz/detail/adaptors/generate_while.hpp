#pragma once

#ifndef LZ_GENERATE_WHILE_ADAPTOR_HPP
#define LZ_GENERATE_WHILE_ADAPTOR_HPP

#include <Lz/detail/iterables/generate_while.hpp>

namespace lz {
namespace detail {
struct generate_while_adaptor {
    using adaptor = generate_while_adaptor;

    /**
     * @brief Generates elements while the predicate returns true. The predicate must return an object that is compatible with
     * std::get. The first element (std::get<0>) must be an object convertible to bool, the second element (std::get<1>) can be
     * any type. This iterable does not contain a .size() member function. Its end() function returns a sentinel, rather than an
     * actual iterator object. Example:
     * ```cpp
     * int i = 0;
     * auto generator = lz::generate_while([&i]() {
     *    auto copy = i++;
     *    return std::make_pair(copy != 4, copy);
     * }); // { 0, 1, 2, 3 }
     * // or (cxx 14)
     * auto generator = lz::generate_while([i = 0]() {
     *   auto pair = std::make_pair(i != 4, i);
     *    ++i;
     *   return pair;
     * }); // { 0, 1, 2, 3 }
     * ```
     * @param generator_func The generator function that returns a std::pair compatible object, where pair::first_type is
     * convertible to bool.
     * @return An iterable that generates elements while the predicate returns true.
     */
    template<class GeneratorFunc>
    LZ_NODISCARD constexpr generate_while_iterable<GeneratorFunc> operator()(GeneratorFunc generator_func) const {
        using pair = decltype(generator_func());
        using pair_first = decltype(std::get<0>(std::declval<pair>()));
        static_assert(std::is_convertible<detail::decay_t<pair_first>, bool>::value,
                      "Function must return a std::pair compatible object (i.e. object::first, object::second), where "
                      "object::first"
                      "returns a bool like object.");
        return { std::move(generator_func) };
    }
};
} // namespace detail
} // namespace lz

#endif
