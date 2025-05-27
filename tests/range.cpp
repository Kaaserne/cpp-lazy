#include <Lz/algorithm.hpp>
#include <Lz/map.hpp>
#include <Lz/range.hpp>
#include <Lz/reverse.hpp>
#include <catch2/catch.hpp>
#include <cstddef>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Range changing and creating elements") {
    SECTION("Looping upwards int") {
        auto expected = { 0, 1, 2, 3, 4 };
        REQUIRE(lz::equal(lz::range(5), expected));
    }

    SECTION("Looping backwards int") {
        auto expected = { 5, 4, 3, 2, 1 };
        REQUIRE(lz::equal(lz::range(5, 0, -1), expected));
    }

    SECTION("Looping upwards with step int") {
        auto expected = { 0, 2, 4 };
        REQUIRE(lz::equal(lz::range(0, 5, 2), expected));
    }

    SECTION("Looping backwards with step int") {
        auto expected = { 5, 3, 1 };
        REQUIRE(lz::equal(lz::range(5, 0, -2), expected));
    }

    SECTION("Looping upwards float") {
        auto expected = { Approx(0.0), Approx(0.5), Approx(1.0), Approx(1.5), Approx(2.0) };
        REQUIRE(lz::equal(lz::range(0., 2.5, 0.5), expected));
    }

    SECTION("Looping backwards float") {
        auto expected = { Approx(2.5), Approx(2.0), Approx(1.5), Approx(1.0), Approx(0.5) };
        REQUIRE(lz::equal(lz::range(2.5, 0., -0.5), expected));
    }

    SECTION("Looping upwards with step float") {
        auto expected = { Approx(0.0), Approx(1.0), Approx(2.0) };
        REQUIRE(lz::equal(lz::range(0., 2.5, 1.), expected));
    }

    SECTION("Looping backwards with step float") {
        auto expected = { Approx(2.5), Approx(1.5), Approx(0.5) };
        REQUIRE(lz::equal(lz::range(2.5, 0., -1.), expected));
    }


    SECTION("Operator=") {
        auto float_range = lz::range(0., 10.5, 0.5);
        auto it = float_range.begin();
        REQUIRE(it != float_range.end());
        it = float_range.end();
        REQUIRE(it == float_range.end());

        auto int_range = lz::range(0, 10, 1);
        auto it2 = int_range.begin();
        REQUIRE(it2 != int_range.end());
        it2 = int_range.end();
        REQUIRE(it2 == int_range.end());

        float_range = lz::range(10.5, 0., -0.5);
        it = float_range.begin();
        REQUIRE(it != float_range.end());
        it = float_range.end();
        REQUIRE(it == float_range.end());

        int_range = lz::range(10, 0, -1);
        it2 = int_range.begin();
        REQUIRE(it2 != int_range.end());
        it2 = int_range.end();
        REQUIRE(it2 == int_range.end());
    }
}

TEST_CASE("Empty or one element range") {
    SECTION("Empty") {
        auto range = lz::range(0);
        REQUIRE(lz::empty(range));
        REQUIRE(!lz::has_many(range));
        REQUIRE(!lz::has_one(range));
    }

    SECTION("One element") {
        auto range = lz::range(1);
        REQUIRE(!lz::empty(range));
        REQUIRE(!lz::has_many(range));
        REQUIRE(lz::has_one(range));
    }
}

TEST_CASE("Range binary operations") {
    constexpr int size = 10;
    auto range = lz::range(size);
    auto f_range = lz::range(0., 10.5, 0.5);
    auto it = range.begin();
    auto f_it = f_range.begin();

    REQUIRE(*it == 0);
    REQUIRE(*f_it == 0.);

    SECTION("Operator++") {
        auto expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        REQUIRE(lz::equal(range, expected));
        auto f_expected = { Approx(0.0), Approx(0.5), Approx(1.0), Approx(1.5), Approx(2.0), Approx(2.5), Approx(3.0),
                            Approx(3.5), Approx(4.0), Approx(4.5), Approx(5.0), Approx(5.5), Approx(6.0), Approx(6.5),
                            Approx(7.0), Approx(7.5), Approx(8.0), Approx(8.5), Approx(9.0), Approx(9.5), Approx(10.0) };
        REQUIRE(lz::equal(f_range, f_expected));
    }

    SECTION("Operator--") {
        auto expected = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
        REQUIRE(lz::equal(range | lz::reverse, expected));
        auto f_expected = { Approx(10.), Approx(9.5), Approx(9.0), Approx(8.5), Approx(8.0), Approx(7.5), Approx(7.0),
                            Approx(6.5), Approx(6.0), Approx(5.5), Approx(5.0), Approx(4.5), Approx(4.0), Approx(3.5),
                            Approx(3.0), Approx(2.5), Approx(2.0), Approx(1.5), Approx(1.0), Approx(0.5), Approx(0.0) };
        REQUIRE(lz::equal(f_range | lz::reverse, f_expected));
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(it != range.end());
        it = range.end();
        REQUIRE(it == range.end());
    }

    SECTION("Operator+") {
        auto begin = range.begin();
        auto end = range.end();
        REQUIRE(begin + 0 == begin);
        REQUIRE(end + 0 == end);

        std::vector<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        for (std::ptrdiff_t i = 0; i < lz::ssize(range) - 1; ++i) {
            INFO("With i = " << i);
            REQUIRE(*(begin + i) == *(expected.begin() + i));
        }
        REQUIRE(begin + lz::ssize(range) == range.end());
        for (std::ptrdiff_t i = 1; i <= lz::ssize(range); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(end - i) == *(expected.end() - i));
        }
        REQUIRE(end - lz::ssize(range) == range.begin());

        std::advance(begin, lz::ssize(range));
        std::advance(end, -lz::ssize(range));
        REQUIRE(begin + 0 == begin);
        REQUIRE(end + 0 == end);

        for (std::ptrdiff_t i = 0; i < lz::ssize(range) - 1; ++i) {
            INFO("With i = " << i);
            REQUIRE(*(end + i) == *(expected.begin() + i));
        }
        REQUIRE(end + lz::ssize(range) == range.end());
        for (std::ptrdiff_t i = 1; i <= lz::ssize(range); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(begin - i) == *(expected.end() - i));
        }
        REQUIRE(begin - lz::ssize(range) == range.begin());
    }

    SECTION("Operator-") {
        auto begin = range.begin();
        auto end = range.end();
        for (std::ptrdiff_t i = 0; i < lz::ssize(range); ++i) {
            INFO("With i = " << i);
            REQUIRE((end - i) - begin == lz::ssize(range) - i);
            REQUIRE(end - (begin + i) == lz::ssize(range) - i);
            REQUIRE((begin + i) - end == -(lz::ssize(range) - i));
            REQUIRE(begin - (end - i) == -(lz::ssize(range) - i));
        }

        for (std::ptrdiff_t i = 0; i < lz::ssize(range); ++i) {
            INFO("With i = " << i);
            REQUIRE((end - i) - (begin + i) == lz::ssize(range) - 2 * i);
            REQUIRE((begin + i) - (end - i) == -(lz::ssize(range) - 2 * i));
        }
    }
}

TEST_CASE("Range to containers") {
    constexpr int size = 10;
    auto range = lz::range(size);

    SECTION("To array") {
        std::array<int, static_cast<std::size_t>(size)> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        auto actual = range | lz::to<std::array<int, static_cast<std::size_t>(size)>>();

        REQUIRE(expected == actual);
    }

    SECTION("To vector") {
        std::vector<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        auto actual = range | lz::to<std::vector>();

        REQUIRE(expected == actual);
    }

    SECTION("To other container using to<>()") {
        std::list<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        auto actual = range | lz::to<std::list>();

        REQUIRE(expected == actual);
    }

    SECTION("To map") {
        auto expected = range | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();
        std::map<int, int> actual;

        for (int i : lz::range(size)) {
            actual.insert(std::make_pair(i, i));
        }

        REQUIRE(expected == actual);
    }

    SECTION("To unordered map") {
        auto expected = range | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        std::unordered_map<int, int> actual;

        for (int i : lz::range(size)) {
            actual.insert(std::make_pair(i, i));
        }

        REQUIRE(expected == actual);
    }
}
