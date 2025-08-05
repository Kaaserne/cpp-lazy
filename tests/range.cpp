#include <Lz/algorithm.hpp>
#include <Lz/range.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <doctest/doctest.h>
#include <vector>

TEST_CASE("Range permutations") {
    SUBCASE("1 step, int") {
        auto range = lz::range(0, 10, 1);
        std::vector<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(9, -1, -1);
        expected = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("2 steps, int") {
        auto range = lz::range(0, 10, 2);
        std::vector<int> expected = { 0, 2, 4, 6, 8 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(8, -2, -2);
        expected = { 8, 6, 4, 2, 0 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("3 steps, int") {
        auto range = lz::range(0, 10, 3);
        std::vector<int> expected = { 0, 3, 6, 9 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(9, -3, -3);
        expected = { 9, 6, 3, 0 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("1 step float") {
        auto range = lz::range(0.f, 10.f, 1.f);
        std::vector<doctest::Approx> expected = { doctest::Approx(0), doctest::Approx(1), doctest::Approx(2), doctest::Approx(3),
                                                  doctest::Approx(4), doctest::Approx(5), doctest::Approx(6), doctest::Approx(7),
                                                  doctest::Approx(8), doctest::Approx(9) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(9.f, -1.f, -1.f);
        expected = { doctest::Approx(9), doctest::Approx(8), doctest::Approx(7), doctest::Approx(6), doctest::Approx(5),
                     doctest::Approx(4), doctest::Approx(3), doctest::Approx(2), doctest::Approx(1), doctest::Approx(0) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("0.5 step float") {
        auto range = lz::range(0.f, 5.6f, 0.5f);
        std::vector<doctest::Approx> expected = { doctest::Approx(0),   doctest::Approx(0.5), doctest::Approx(1),
                                                  doctest::Approx(1.5), doctest::Approx(2),   doctest::Approx(2.5),
                                                  doctest::Approx(3),   doctest::Approx(3.5), doctest::Approx(4),
                                                  doctest::Approx(4.5), doctest::Approx(5),   doctest::Approx(5.5) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(5.f, -0.5f, -0.5f);
        expected = { doctest::Approx(5), doctest::Approx(4.5), doctest::Approx(4), doctest::Approx(3.5),
                     doctest::Approx(3), doctest::Approx(2.5), doctest::Approx(2), doctest::Approx(1.5),
                     doctest::Approx(1), doctest::Approx(0.5), doctest::Approx(0) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("2.5 step float") {
        auto range = lz::range(0.f, 5.5f, 2.5f);
        std::vector<doctest::Approx> expected = { doctest::Approx(0), doctest::Approx(2.5), doctest::Approx(5) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(2.5f, -7.5f, -2.5f);
        expected = { doctest::Approx(2.5), doctest::Approx(0), doctest::Approx(-2.5), doctest::Approx(-5.) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("No step specified int") {
        auto range = lz::range(10);
        std::vector<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(-1, 10);
        expected = { -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("No step specified float") {
        auto range = lz::range(10.5f);
        std::vector<doctest::Approx> expected = { doctest::Approx(0), doctest::Approx(1), doctest::Approx(2), doctest::Approx(3),
                                                  doctest::Approx(4), doctest::Approx(5), doctest::Approx(6), doctest::Approx(7),
                                                  doctest::Approx(8), doctest::Approx(9), doctest::Approx(10) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(10.f);
        expected = { doctest::Approx(0), doctest::Approx(1), doctest::Approx(2), doctest::Approx(3), doctest::Approx(4),
                     doctest::Approx(5), doctest::Approx(6), doctest::Approx(7), doctest::Approx(8), doctest::Approx(9) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(-1.f, 10.5f);
        expected = { doctest::Approx(-1.), doctest::Approx(0), doctest::Approx(1), doctest::Approx(2),
                     doctest::Approx(3),   doctest::Approx(4), doctest::Approx(5), doctest::Approx(6),
                     doctest::Approx(7),   doctest::Approx(8), doctest::Approx(9), doctest::Approx(10) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));

        range = lz::range(-1.f, 10.f);
        expected = { doctest::Approx(-1.), doctest::Approx(0), doctest::Approx(1), doctest::Approx(2),
                     doctest::Approx(3),   doctest::Approx(4), doctest::Approx(5), doctest::Approx(6),
                     doctest::Approx(7),   doctest::Approx(8), doctest::Approx(9) };
        REQUIRE(lz::equal(range, expected));
        REQUIRE(lz::equal(range | lz::reverse, expected | lz::reverse));
    }
}

TEST_CASE("Binary operations") {
    SUBCASE("With step, int uneven") {
        lz::stepwise_range_iterable<int> range = lz::range(0, 10, 3);
        std::vector<int> expected = { 0, 3, 6, 9 };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }

    SUBCASE("With step, int even") {
        auto range = lz::range(0, 10, 2);
        std::vector<int> expected = { 0, 2, 4, 6, 8 };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }

    SUBCASE("With step, float") {
        auto range = lz::range(0.f, 5.5f, 0.5f);
        std::vector<doctest::Approx> expected = { doctest::Approx(0),   doctest::Approx(0.5), doctest::Approx(1),
                                                  doctest::Approx(1.5), doctest::Approx(2),   doctest::Approx(2.5),
                                                  doctest::Approx(3),   doctest::Approx(3.5), doctest::Approx(4),
                                                  doctest::Approx(4.5), doctest::Approx(5) };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }

    SUBCASE("Without step, int") {
        lz::range_iterable<int> range = lz::range(10);
        std::vector<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }

    SUBCASE("Without step, float") {
        auto range = lz::range(10.f);
        std::vector<doctest::Approx> expected = { doctest::Approx(0), doctest::Approx(1), doctest::Approx(2), doctest::Approx(3),
                                                  doctest::Approx(4), doctest::Approx(5), doctest::Approx(6), doctest::Approx(7),
                                                  doctest::Approx(8), doctest::Approx(9) };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);

        range = lz::range(10.5f);
        expected = { doctest::Approx(0), doctest::Approx(1), doctest::Approx(2), doctest::Approx(3),
                     doctest::Approx(4), doctest::Approx(5), doctest::Approx(6), doctest::Approx(7),
                     doctest::Approx(8), doctest::Approx(9), doctest::Approx(10) };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }

    SUBCASE("Without step, int, with start") {
        lz::range_iterable<int> range = lz::range(2, 10);
        std::vector<int> expected = { 2, 3, 4, 5, 6, 7, 8, 9 };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }

    SUBCASE("Without step, float, with start") {
        auto range = lz::range(2.3f, 10.5f);
        std::vector<doctest::Approx> expected = { doctest::Approx(2.3), doctest::Approx(3.3), doctest::Approx(4.3),
                                                  doctest::Approx(5.3), doctest::Approx(6.3), doctest::Approx(7.3),
                                                  doctest::Approx(8.3), doctest::Approx(9.3), doctest::Approx(10.3) };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);

        range = lz::range(-1.f, 10.f);
        expected = { doctest::Approx(-1.), doctest::Approx(0), doctest::Approx(1), doctest::Approx(2),
                     doctest::Approx(3),   doctest::Approx(4), doctest::Approx(5), doctest::Approx(6),
                     doctest::Approx(7),   doctest::Approx(8), doctest::Approx(9) };
        test_procs::test_operator_plus(range, expected);
        test_procs::test_operator_minus(range);
    }
}
