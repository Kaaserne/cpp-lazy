#include <Lz/algorithm.hpp>
#include <Lz/range.hpp>
#include <Lz/reverse.hpp>
#include <catch2/catch.hpp>
#include <test_procs.hpp>
#include <vector>

TEST_CASE("Range permutations") {
    SECTION("1 step, int") {
        auto range = lz::range(0, 10, 1);
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
        auto range = lz::range(0.f, 10.f, 1.f);
        std::vector<Approx> expected = { Approx(0.f), Approx(1.f), Approx(2.f), Approx(3.f), Approx(4.f),
                                         Approx(5.f), Approx(6.f), Approx(7.f), Approx(8.f), Approx(9.f) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(9.f, -1.f, -1.f);
        expected = { Approx(9.f), Approx(8.f), Approx(7.f), Approx(6.f), Approx(5.f),
                     Approx(4.f), Approx(3.f), Approx(2.f), Approx(1.f), Approx(0.f) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SECTION("0.5 step float") {
        auto range = lz::range(0.f, 5.6f, 0.5f);
        std::vector<Approx> expected = { Approx(0.f), Approx(0.5f), Approx(1.f), Approx(1.5f), Approx(2.f), Approx(2.5f),
                                         Approx(3.f), Approx(3.5f), Approx(4.f), Approx(4.5f), Approx(5.f), Approx(5.5f) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(5.f, -0.5f, -0.5f);
        expected = { Approx(5.f), Approx(4.5f), Approx(4.f), Approx(3.5f), Approx(3.f), Approx(2.5f),
                     Approx(2.f), Approx(1.5f), Approx(1.f), Approx(0.5f), Approx(0.f) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SECTION("2.5 step float") {
        auto range = lz::range(0.f, 5.5f, 2.5f);
        std::vector<Approx> expected = { Approx(0.f), Approx(2.5f), Approx(5.f) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(2.5f, -7.5f, -2.5f);
        expected = { Approx(2.5f), Approx(0.f), Approx(-2.5f), Approx(-5.f) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SECTION("No step specified int") {
        auto range = lz::range(10);
        std::vector<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(-1, 10);
        expected = { -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SECTION("No step specified float") {
        auto range = lz::range(10.5f);
        std::vector<Approx> expected = { Approx(0.f), Approx(1.f), Approx(2.f), Approx(3.f), Approx(4.f), Approx(5.f),
                                         Approx(6.f), Approx(7.f), Approx(8.f), Approx(9.f), Approx(10.f) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(10.f);
        expected = { Approx(0.f), Approx(1.f), Approx(2.f), Approx(3.f), Approx(4.f),
                     Approx(5.f), Approx(6.f), Approx(7.f), Approx(8.f), Approx(9.f) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(-1.f, 10.5f);
        expected = { Approx(-1.f), Approx(0.f), Approx(1.f), Approx(2.f), Approx(3.f), Approx(4.f),
                     Approx(5.f),  Approx(6.f), Approx(7.f), Approx(8.f), Approx(9.f), Approx(10.f) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(-1.f, 10.f);
        expected = { Approx(-1.f), Approx(0.f), Approx(1.f), Approx(2.f), Approx(3.f), Approx(4.f),
                     Approx(5.f),  Approx(6.f), Approx(7.f), Approx(8.f), Approx(9.f) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }
}

TEST_CASE("Binary operations") {
    SECTION("With step, int uneven") {
        lz::stepwise_range_iterable<int> range = lz::range(0, 10, 3);
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
        std::vector<Approx> expected = { Approx(0.f), Approx(0.5f), Approx(1.f), Approx(1.5f), Approx(2.f), Approx(2.5f),
                                         Approx(3.f), Approx(3.5f), Approx(4.f), Approx(4.5f), Approx(5.f) };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }

    SECTION("Without step, int") {
        lz::range_iterable<int> range = lz::range(10);
        std::vector<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }

    SECTION("Without step, float") {
        auto range = lz::range(10.f);
        std::vector<Approx> expected = { Approx(0.f), Approx(1.f), Approx(2.f), Approx(3.f), Approx(4.f),
                                         Approx(5.f), Approx(6.f), Approx(7.f), Approx(8.f), Approx(9.f) };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);

        range = lz::range(10.5f);
        expected = { Approx(0.f), Approx(1.f), Approx(2.f), Approx(3.f), Approx(4.f), Approx(5.f),
                     Approx(6.f), Approx(7.f), Approx(8.f), Approx(9.f), Approx(10.f) };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }

    SECTION("Without step, int, with start") {
        lz::range_iterable<int> range = lz::range(2, 10);
        std::vector<int> expected = { 2, 3, 4, 5, 6, 7, 8, 9 };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }

    SECTION("Without step, float, with start") {
        auto range = lz::range(2.3f, 10.5f);
        std::vector<Approx> expected = { Approx(2.3f), Approx(3.3f), Approx(4.3f), Approx(5.3f), Approx(6.3f),
                                         Approx(7.3f), Approx(8.3f), Approx(9.3f), Approx(10.3f) };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);

        range = lz::range(-1.f, 10.f);
        expected = { Approx(-1.f), Approx(0.f), Approx(1.f), Approx(2.f), Approx(3.f), Approx(4.f),
                     Approx(5.f),  Approx(6.f), Approx(7.f), Approx(8.f), Approx(9.f) };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }
}
