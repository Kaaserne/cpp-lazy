#include <Lz/algorithm.hpp>
#include <Lz/map.hpp>
#include <Lz/range.hpp>
#include <catch2/catch.hpp>
#include <cstddef>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Range changing and creating elements") {
    SECTION("Looping upwards") {
        int expected_counter = 0;

        for (int i : lz::range(5)) {
            REQUIRE(expected_counter == i);
            expected_counter++;
        }
    }

    SECTION("Looping backwards") {
        int expected_counter = 5;

        for (int i : lz::range(5, 0, -1)) {
            REQUIRE(expected_counter == i);
            expected_counter--;
        }
    }

    SECTION("Looping upwards with step") {
        int expected_counter = 0;

        for (int i : lz::range(0, 5, 2)) {
            REQUIRE(expected_counter == i);
            expected_counter += 2;
        }
    }

    SECTION("Looping backwards with step") {
        int expected_counter = 5;

        for (int i : lz::range(5, 0, -2)) {
            REQUIRE(expected_counter == i);
            expected_counter -= 2;
        }
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
        ++it;
        REQUIRE(*it == 1);
        ++f_it;
        REQUIRE(*f_it == 0.5);
    }

    SECTION("Operator--") {
        ++it, --it;
        REQUIRE(*it == 0);
        ++f_it, --f_it;
        REQUIRE(*f_it == 0);
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
        for (std::size_t i = 0; i < lz::size(range) - 1; ++i) {
            INFO("With i = " << i);
            REQUIRE(*(begin + static_cast<std::ptrdiff_t>(i)) == *(expected.begin() + static_cast<std::ptrdiff_t>(i)));
        }
        REQUIRE(begin + static_cast<std::ptrdiff_t>(lz::size(range)) == range.end());
        for (std::size_t i = 1; i <= lz::size(range); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(end - static_cast<std::ptrdiff_t>(i)) == *(expected.end() - static_cast<std::ptrdiff_t>(i)));
        }
        REQUIRE(end - static_cast<std::ptrdiff_t>(lz::size(range)) == range.begin());

        std::advance(begin, static_cast<std::ptrdiff_t>(lz::size(range)));
        std::advance(end, -static_cast<std::ptrdiff_t>(lz::size(range)));
        REQUIRE(begin + 0 == begin);
        REQUIRE(end + 0 == end);

        for (std::size_t i = 0; i < lz::size(range) - 1; ++i) {
            INFO("With i = " << i);
            REQUIRE(*(end + static_cast<std::ptrdiff_t>(i)) == *(expected.begin() + static_cast<std::ptrdiff_t>(i)));
        }
        REQUIRE(end + static_cast<std::ptrdiff_t>(lz::size(range)) == range.end());
        for (std::size_t i = 1; i <= lz::size(range); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(begin - static_cast<std::ptrdiff_t>(i)) == *(expected.end() - static_cast<std::ptrdiff_t>(i)));
        }
        REQUIRE(begin - static_cast<std::ptrdiff_t>(lz::size(range)) == range.begin());
    }

    SECTION("Operator-") {
        auto begin = range.begin();
        auto end = range.end();
        for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(lz::size(range)); ++i) {
            INFO("With i = " << i);
            REQUIRE((end - i) - begin == static_cast<std::ptrdiff_t>(lz::size(range)) - i);
            REQUIRE(end - (begin + i) == static_cast<std::ptrdiff_t>(lz::size(range)) - i);
            REQUIRE((begin + i) - end == -(static_cast<std::ptrdiff_t>(lz::size(range)) - i));
            REQUIRE(begin - (end - i) == -(static_cast<std::ptrdiff_t>(lz::size(range)) - i));
        }

        for (std::size_t i = 0; i < lz::size(range) ; ++i) {
            INFO("With i = " << i);
            REQUIRE((end - static_cast<std::ptrdiff_t>(i)) - (begin + static_cast<std::ptrdiff_t>(i)) ==
                    static_cast<std::ptrdiff_t>(lz::size(range) - 2 * i));
            REQUIRE((begin + static_cast<std::ptrdiff_t>(i)) - (end - static_cast<std::ptrdiff_t>(i)) ==
                    -(static_cast<std::ptrdiff_t>(lz::size(range)) - 2 * static_cast<std::ptrdiff_t>(i)));
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
