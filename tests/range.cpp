#include <Lz/algorithm.hpp>
#include <Lz/range.hpp>
#include <Lz/reverse.hpp>
#include <catch2/catch.hpp>
#include <cstddef>
#include <test_procs.hpp>
#include <vector>

TEST_CASE("Range permutations") {
    SECTION("1 step, int") {
        auto range = lz::range(10);
        std::vector<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(9, -1, -1);
        expected = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SECTION("2 steps, int") {
        auto range = lz::range(0, 10, 2);
        std::vector<int> expected = { 0, 2, 4, 6, 8 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(8, -2, -2);
        expected = { 8, 6, 4, 2, 0 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SECTION("3 steps, int") {
        auto range = lz::range(0, 10, 3);
        std::vector<int> expected = { 0, 3, 6, 9 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(9, -3, -3);
        expected = { 9, 6, 3, 0 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SECTION("1 step float") {
        auto range = lz::range(10.f);
        std::vector<float> expected = { 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(9.f, -1.f, -1.f);
        expected = { 9.f, 8.f, 7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SECTION("0.5 step float") {
        auto range = lz::range(0.f, 5.5f, 0.5f);
        std::vector<float> expected = { 0.f, 0.5f, 1.f, 1.5f, 2.f, 2.5f, 3.f, 3.5f, 4.f, 4.5f, 5.f };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(5.f, -0.5f, -0.5f);
        expected = { 5.f, 4.5f, 4.f, 3.5f, 3.f, 2.5f, 2.f, 1.5f, 1.f, 0.5f, 0.f };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SECTION("2.5 step float") {
        auto range = lz::range(0.f, 5.5f, 2.5f);
        std::vector<float> expected = { 0.f, 2.5f, 5.f };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(2.5f, -7.5f, -2.5f);
        expected = { 2.5f, 0.f, -2.5f, -5.f };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }
}

TEST_CASE("Binary operations") {
    SECTION("With step, int uneven") {
        lz::range_iterable<int> range = lz::range(0, 10, 3);
        std::vector<int> expected = { 0, 3, 6, 9 };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }

    SECTION("With step, int even") {
        auto range = lz::range(0, 10, 2);
        std::vector<int> expected = { 0, 2, 4, 6, 8 };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }

    SECTION("With step, float") {
        auto range = lz::range(0.f, 5.5f, 0.5f);
        std::vector<float> expected = { 0.f, 0.5f, 1.f, 1.5f, 2.f, 2.5f, 3.f, 3.5f, 4.f, 4.5f, 5.f };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }

    SECTION("Without step, int") {
        auto range = lz::range(10);
        std::vector<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }

    SECTION("Without step, float") {
        auto range = lz::range(10.f);
        std::vector<float> expected = { 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }
}
