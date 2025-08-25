#pragma once

#ifndef LZ_REPEAT_ADAPTOR_HPP
#define LZ_REPEAT_ADAPTOR_HPP

#include <Lz/detail/iterables/repeat.hpp>

namespace lz {
namespace detail {
struct repeat_adaptor {
    using adaptor = repeat_adaptor;

    /**
     * @brief Returns an element n (or infinite) times. It returns a sentinel as its end() iterator. It contains a .size() method
     * which is equal to the amount of times the element is repeated (if it is not infinite). Its iterator category is random
     * access. Will return the same reference type as the reference type passed. Example:
     * ```cpp
     * auto repeater = lz::repeat(20, 5); // {20, 20, 20, 20, 20} by value
     * // infinite variant
     * auto repeater = lz::repeat(20); // {20, 20, 20, ...} by value
     * auto val = 3;
     * auto repeater =  lz::repeat(val, 5); // {3, 3, 3, 3, 3}, by reference
     * auto repeater = lz::repeat(std::ref(val), 5); // {3, 3, 3, 3, 3}, by reference for more flexibility (copy ctors etc.)
     * auto repeater =  lz::repeat(std::as_const(val)); // {3, 3, 3, ...}, by const reference
     *
     * ```
     * @param value The value to repeat
     * @param amount The amount of times to repeat the value
     * @return A repeat_iterable that will yield the value @p `amount` times.
     */
    template<class T>
    LZ_NODISCARD constexpr repeat_iterable<false, remove_rvalue_reference_t<T>> operator()(T&& value, const size_t amount) const {
        return { std::forward<T>(value), amount };
    }

    /**
     * @brief Returns an element infinite times. It returns a sentinel as its end() iterator. It does not
     * contains a .size() method. Its iterator category is random access. Its iterator category is random
     * access. Will return the same reference type as the reference type passed. Example:
     * ```cpp
     * auto repeater = lz::repeat(20, 5); // {20, 20, 20, 20, 20} by value
     * auto val = 3;
     * auto repeater = lz::repeat(std::as_const(val)); // {3, 3, 3, ...}, by const reference
     * auto repeater = lz::repeat(val); // {3, 3, 3, ...}, by reference
     * auto repeater = lz::repeat(std::ref(val)); // {3, 3, 3, ...}, by reference for more flexibility (copy ctors etc.)
     * ```
     * @param value The value to repeat
     * @return A repeat_iterable that will yield the value infinite times.
     */
    template<class T>
    LZ_NODISCARD constexpr repeat_iterable<true, T> operator()(T&& value) const {
        return repeat_iterable<true, T>{ std::forward<T>(value) };
    }
};
} // namespace detail
} // namespace lz

#endif
