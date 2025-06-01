#pragma once

#ifndef LZ_RA_TEST_PROCS_HPP
#define LZ_RA_TEST_PROCS_HPP

#include <Lz/detail/procs.hpp>
#include <Lz/stream.hpp>
#include <catch2/catch.hpp>

namespace test_procs {

template<class Iterable>
void test_operator_minus(const Iterable& it) {
    auto begin = it.begin();
    auto end = it.end();

    for (std::ptrdiff_t i = 0; i < lz::ssize(it); ++i) {
        INFO("With i = " << i);
        REQUIRE((end - i) - begin == lz::ssize(it) - i);
        REQUIRE(end - (begin + i) == lz::ssize(it) - i);
        REQUIRE((begin + i) - end == -(lz::ssize(it) - i));
        REQUIRE(begin - (end - i) == -(lz::ssize(it) - i));
    }

    for (std::ptrdiff_t i = 0; i < lz::ssize(it); ++i) {
        INFO("With i = " << i);
        REQUIRE((end - i) - (begin + i) == lz::ssize(it) - 2 * i);
        REQUIRE((begin + i) - (end - i) == -(lz::ssize(it) - 2 * i));
    }
}

template<class Iterable, class ExpectedIterable, class EqCompare = MAKE_BIN_PRED(equal_to)>
void test_operator_plus(const Iterable& it, const ExpectedIterable& expected, EqCompare eq_compare = {}) {
    auto begin = it.begin();
    auto end = it.end();

    for (std::ptrdiff_t i = 0; i < lz::ssize(it) - 1; ++i) {
        INFO("With i = " << i);
        REQUIRE(eq_compare(*(begin + i), *(expected.begin() + i)));
    }
    REQUIRE(begin + lz::ssize(it) == it.end());
    for (std::ptrdiff_t i = 1; i <= lz::ssize(it); ++i) {
        INFO("With i = " << i);
        REQUIRE(eq_compare(*(end - i), *(expected.end() - i)));
    }
    REQUIRE(end - lz::ssize(it) == it.begin());

    std::advance(begin, lz::ssize(it));
    std::advance(end, -lz::ssize(it));
    REQUIRE(begin + 0 == begin);
    REQUIRE(end + 0 == end);

    for (std::ptrdiff_t i = 0; i < lz::ssize(it) - 1; ++i) {
        INFO("With i = " << i);
        REQUIRE(eq_compare(*(end + i), *(expected.begin() + i)));
    }
    REQUIRE(end + lz::ssize(it) == it.end());
    for (std::ptrdiff_t i = 1; i <= lz::ssize(it); ++i) {
        INFO("With i = " << i);
        REQUIRE(eq_compare(*(begin - i), *(expected.end() - i)));
    }
    REQUIRE(begin - lz::ssize(it) == it.begin());
}
} // namespace test_procs

#endif
