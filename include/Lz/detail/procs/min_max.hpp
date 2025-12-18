#pragma once

#include <Lz/detail/compiler_config.hpp>

namespace lz {
namespace detail {

template<class T>
constexpr T max_variadic2(const T a, const T b) {
    return a > b ? a : b;
}

template<class T, class... U>
constexpr T max_variadic2(const T value1, const T value2, const U... values) {
    return value1 < value2 ? max_variadic2(value2, values...) : max_variadic2(value1, values...);
}

template<class T, class... U>
constexpr T max_variadic(const T value, const U... values) {
    return max_variadic2(value, values...);
}

template<class T>
constexpr T min_variadic2(const T a, const T b) {
    return a < b ? a : b;
}

template<class T, class... U>
constexpr T min_variadic2(const T value1, const T value2, const U... values) {
    return value1 > value2 ? min_variadic2(value2, values...) : min_variadic2(value1, values...);
}

template<class T, class... U>
constexpr T min_variadic(const T value, const U... values) {
    return min_variadic2(value, values...);
}

} // namespace detail
} // namespace lz
