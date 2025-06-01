#pragma once

#ifndef LZ_REPEAT_ADAPTOR_HPP
#define LZ_REPEAT_ADAPTOR_HPP

#include <Lz/detail/iterables/repeat.hpp>

namespace lz {
namespace detail {
struct repeat_adaptor {
    using adaptor = repeat_adaptor;

    /**
     * @brief Returns an element n times. It returns a sentinel as its end() iterator. It contains a .size() method which is equal
     * to the amount of times the element is repeated. Its iterator category is forward. Example:
     * ```cpp
     * auto repeater = lz::repeat(20, 5); // {20, 20, 20, 20, 20}
     * ```
     * @param value The value to repeat
     * @param amount The amount of times to repeat the value
     * @return A repeat_iterable that will yield the value @p `amount` times.
     */
    template<class T>
    LZ_NODISCARD constexpr repeat_iterable<false, T> operator()(T value, const std::size_t amount) const {
        return { std::move(value), amount };
    }

    /**
     * @brief Returns an element infinite times. It returns a sentinel as its end() iterator. It does not
     * contains a .size() method. Its iterator category is forward. Example:
     * ```cpp
     * auto repeater = lz::repeat(20, 5); // {20, 20, 20, 20, 20}
     * ```
     * @param value The value to repeat
     * @param amount The amount of times to repeat the value
     * @return A repeat_iterable that will yield the value infinite times.
     */
    template<class T>
    LZ_NODISCARD constexpr repeat_iterable<true, T> operator()(T value) const {
        return { std::move(value) };
    }
};
} // namespace detail
} // namespace lz
#endif