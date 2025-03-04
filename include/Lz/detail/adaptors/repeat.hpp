#pragma once

#ifndef LZ_REPEAT_ADAPTOR_HPP
#define LZ_REPEAT_ADAPTOR_HPP

#include <Lz/detail/iterables/repeat.hpp>

namespace lz {
namespace detail {
struct repeat_adaptor {
    using adaptor = repeat_adaptor;

#ifdef LZ_HAS_CXX_11

    static constexpr adaptor repeat{};

#endif

    /**
     * @brief Returns an element n times. It returns a sentinel as its end() iterator. It contains a .size() method which is equal
     * to the amount of times the element is repeated. Its iterator category is forward. Example:
     * ```cpp
     * auto repeater = lz::repeat(20, 5); // {20, 20, 20, 20, 20}
     * ```
     * @param value The value to repeat
     * @param amount The amount of times to repeat the value
     */
    template<class T>
    LZ_NODISCARD constexpr repeat_iterable<T> operator()(T value, std::size_t amount) const {
        return { std::move(value), amount };
    }
};
} // namespace detail
} // namespace lz
#endif