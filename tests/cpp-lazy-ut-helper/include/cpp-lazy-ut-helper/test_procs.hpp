#pragma once

#ifndef LZ_RA_TEST_PROCS_HPP
#define LZ_RA_TEST_PROCS_HPP

#include <Lz/detail/procs.hpp>
#include <Lz/stream.hpp>
#include <doctest/doctest.h>
#include <sstream>

namespace test_procs {

template<class T, class = void>
struct has_stream_operator : std::false_type {};

template<class T>
struct has_stream_operator<T, lz::detail::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> : std::true_type {
};

#ifdef LZ_HAS_CXX_17

template<class T, class U>
auto get_error_expr(const T& lhs, const U& rhs) {
    if constexpr (has_stream_operator<T>::value && has_stream_operator<U>::value) {
        std::ostringstream oss;
        oss << lhs << " != " << rhs;
        return oss.str();
    }
    else {
        static_cast<void>(lhs), static_cast<void>(rhs);
        return "? != ?";
    }
}

template<class Iterable>
void test_operator_minus(const Iterable& it) {
    if constexpr (lz::detail::has_sentinel<Iterable>::value) {
        auto begin = it.begin();
        auto end = it.end();

        const auto size = lz::ssize(it);
        for (std::ptrdiff_t i = 0; i < size; ++i) {
            INFO("With i = " << i);
            REQUIRE(end - (begin + i) == size - i);
            REQUIRE((begin + i) - end == -(size - i));
        }
    }
    else {
        auto begin = it.begin();
        auto end = it.end();

        const auto size = lz::ssize(it);
        for (std::ptrdiff_t i = 0; i < size; ++i) {
            INFO("With i = " << i);
            REQUIRE((end - i) - begin == size - i);
            REQUIRE(end - (begin + i) == size - i);
            REQUIRE((begin + i) - end == -(size - i));
            REQUIRE(begin - (end - i) == -(size - i));
        }

        for (std::ptrdiff_t i = 0; i < size; ++i) {
            INFO("With i = " << i);
            REQUIRE((end - i) - (begin + i) == size - 2 * i);
            REQUIRE((begin + i) - (end - i) == -(size - 2 * i));
        }
    }
}

template<class Iterable, class ExpectedIterable, class EqCompare = MAKE_BIN_PRED(equal_to)>
void test_operator_plus(const Iterable& it, const ExpectedIterable& expected, EqCompare eq_compare = {}) {
    REQUIRE(lz::size(it) == lz::size(expected));
    if constexpr (lz::detail::has_sentinel<Iterable>::value) {
        auto begin = it.begin();

        for (std::ptrdiff_t i = 0; i < lz::ssize(it) - 1; ++i) {
            INFO("With i = " << i << " with expr: " << get_error_expr(*(begin + i), *(expected.begin() + i)));
            REQUIRE(eq_compare(*(begin + i), *(expected.begin() + i)));
        }
        REQUIRE(begin + lz::ssize(it) == it.end());

        std::advance(begin, lz::ssize(it));
        REQUIRE(begin + 0 == begin);

        for (std::ptrdiff_t i = 1; i <= lz::ssize(it); ++i) {
            INFO("With i = " << i << " with expr: " << get_error_expr(*(begin - i), *(expected.end() - i)));
            REQUIRE(eq_compare(*(begin - i), *(expected.end() - i)));
        }
        REQUIRE(begin - lz::ssize(it) == it.begin());
    }
    else {
        auto begin = it.begin();
        auto end = it.end();

        const auto size = lz::ssize(it);
        for (std::ptrdiff_t i = 0; i < size - 1; ++i) {
            INFO("With i = " << i << " with expr: " << get_error_expr(*(begin + i), *(expected.begin() + i)));
            REQUIRE(eq_compare(*(begin + i), *(expected.begin() + i)));
        }
        REQUIRE(begin + size == it.end());
        for (std::ptrdiff_t i = 1; i <= size; ++i) {
            INFO("With i = " << i << " with expr: " << get_error_expr(*(end - i), *(expected.end() - i)));
            REQUIRE(eq_compare(*(end - i), *(expected.end() - i)));
        }
        REQUIRE(end - size == it.begin());

        std::advance(begin, size);
        std::advance(end, -size);
        REQUIRE(begin + 0 == begin);
        REQUIRE(end + 0 == end);

        for (std::ptrdiff_t i = 0; i < size - 1; ++i) {
            INFO("With i = " << i << " with expr: " << get_error_expr(*(end + i), *(expected.begin() + i)));
            REQUIRE(eq_compare(*(end + i), *(expected.begin() + i)));
        }
        REQUIRE(end + size == it.end());
        for (std::ptrdiff_t i = 1; i <= size; ++i) {
            INFO("With i = " << i << " with expr: " << get_error_expr(*(begin - i), *(expected.end() - i)));
            REQUIRE(eq_compare(*(begin - i), *(expected.end() - i)));
        }
        REQUIRE(begin - size == it.begin());
    }
}

#else

template<class T, class U>
lz::detail::enable_if<has_stream_operator<T>::value && has_stream_operator<U>::value> get_error_expr(const T& lhs, const U& rhs) {
    std::ostringstream oss;
    oss << lhs << " != " << rhs;
    return oss.str();
}

template<class T, class U>
lz::detail::enable_if<!has_stream_operator<T>::value || !has_stream_operator<U>::value, const char*>
get_error_expr(const T&, const U&) {
    return "? != ?";
}

template<class Iterable>
lz::detail::enable_if<!lz::detail::has_sentinel<Iterable>::value> test_operator_minus(const Iterable& it) {
    auto begin = it.begin();
    auto end = it.end();

    const auto size = lz::ssize(it);
    for (std::ptrdiff_t i = 0; i < size; ++i) {
        INFO("With i = " << i);
        REQUIRE((end - i) - begin == size - i);
        REQUIRE(end - (begin + i) == size - i);
        REQUIRE((begin + i) - end == -(size - i));
        REQUIRE(begin - (end - i) == -(size - i));
    }

    for (std::ptrdiff_t i = 0; i < size; ++i) {
        INFO("With i = " << i);
        REQUIRE((end - i) - (begin + i) == size - 2 * i);
        REQUIRE((begin + i) - (end - i) == -(size - 2 * i));
    }
}

template<class Iterable>
lz::detail::enable_if<lz::detail::has_sentinel<Iterable>::value> test_operator_minus(const Iterable& it) {
    auto begin = it.begin();
    auto end = it.end();

    const auto size = lz::ssize(it);
    for (std::ptrdiff_t i = 0; i < size; ++i) {
        INFO("With i = " << i);
        REQUIRE(end - (begin + i) == size - i);
        REQUIRE((begin + i) - end == -(size - i));
    }
}

template<class Iterable, class ExpectedIterable, class EqCompare = MAKE_BIN_PRED(equal_to)>
lz::detail::enable_if<!lz::detail::has_sentinel<Iterable>::value>
test_operator_plus(const Iterable& it, const ExpectedIterable& expected, EqCompare eq_compare = {}) {
    REQUIRE(lz::size(it) == lz::size(expected));

    auto begin = it.begin();
    auto end = it.end();

    const auto size = lz::ssize(it);
    for (std::ptrdiff_t i = 0; i < size - 1; ++i) {
        INFO("With i = " << i);
        REQUIRE(eq_compare(*(begin + i), *(expected.begin() + i)));
    }
    REQUIRE(begin + size == it.end());
    for (std::ptrdiff_t i = 1; i <= size; ++i) {
        INFO("With i = " << i);
        REQUIRE(eq_compare(*(end - i), *(expected.end() - i)));
    }
    REQUIRE(end - size == it.begin());

    std::advance(begin, size);
    std::advance(end, -size);
    REQUIRE(begin + 0 == begin);
    REQUIRE(end + 0 == end);

    for (std::ptrdiff_t i = 0; i < size - 1; ++i) {
        INFO("With i = " << i);
        REQUIRE(eq_compare(*(end + i), *(expected.begin() + i)));
    }
    REQUIRE(end + size == it.end());
    for (std::ptrdiff_t i = 1; i <= size; ++i) {
        INFO("With i = " << i);
        REQUIRE(eq_compare(*(begin - i), *(expected.end() - i)));
    }
    REQUIRE(begin - size == it.begin());
}

template<class Iterable, class ExpectedIterable, class EqCompare = MAKE_BIN_PRED(equal_to)>
lz::detail::enable_if<lz::detail::has_sentinel<Iterable>::value>
test_operator_plus(const Iterable& it, const ExpectedIterable& expected, EqCompare eq_compare = {}) {
    REQUIRE(lz::size(it) == lz::size(expected));

    auto begin = it.begin();

    for (std::ptrdiff_t i = 0; i < lz::ssize(it) - 1; ++i) {
        INFO("With i = " << i);
        REQUIRE(eq_compare(*(begin + i), *(expected.begin() + i)));
    }
    REQUIRE(begin + lz::ssize(it) == it.end());

    std::advance(begin, lz::ssize(it));
    REQUIRE(begin + 0 == begin);

    for (std::ptrdiff_t i = 1; i <= lz::ssize(it); ++i) {
        INFO("With i = " << i);
        REQUIRE(eq_compare(*(begin - i), *(expected.end() - i)));
    }
    REQUIRE(begin - lz::ssize(it) == it.begin());
}

#endif
} // namespace test_procs

#endif
