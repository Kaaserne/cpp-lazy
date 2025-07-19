#pragma once

#ifndef LZ_RA_TEST_PROCS_HPP
#define LZ_RA_TEST_PROCS_HPP

#include <Lz/detail/procs.hpp>
#include <Lz/stream.hpp>
#include <catch2/catch.hpp>

namespace test_procs {

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
} // namespace test_procs

#endif
