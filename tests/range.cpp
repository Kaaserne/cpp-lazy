#include <Lz/range.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Range changing and creating elements", "[Range][Basic functionality]") {
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

TEST_CASE("Range binary operations", "[Range][Binary ops]") {
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

    SECTION("Operator+(int) offset, tests += as well") {
        REQUIRE(*(it + 2) == 2);
        REQUIRE(*(it + 4) == 4);
        REQUIRE(*(f_it + 2) == 1.);
        REQUIRE(*(f_it + 3) == 1.5);
        REQUIRE(*(f_it + 5) == 2.5);
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        REQUIRE(*((it + 2) - 1) == 1);
        REQUIRE(*((it + 4) - 2) == 2);
        REQUIRE(*((f_it + 2) - 1) == .5);
        REQUIRE(*((f_it + 3) - 2) == .5);
        REQUIRE(*((f_it + 5) - 2) == 1.5);
    }

    SECTION("Operator-(Iterator)") {
        REQUIRE(range.end() - it == 10);
        REQUIRE(f_range.end() - f_it == 21);

        REQUIRE(range.end() - (it + 1) == 9);
        REQUIRE(f_range.end() - (f_it + 1) == 20);
    }

    SECTION("Operator[]()") {
        REQUIRE(f_it[1] == .5);
        REQUIRE(it[1] == 1);
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto b = range.begin();
        auto end = range.end();
        auto distance = std::distance(b, end);

        REQUIRE(b < end);
        REQUIRE(b + distance - 1 > end - distance);
        REQUIRE(b + distance - 1 <= end);
        REQUIRE(b + size - 1 >= end - 1);

        auto f_b = range.begin();
        auto f_end = range.end();

        REQUIRE(f_b < end);
        REQUIRE(f_b + distance - 1 > f_end - distance);
        REQUIRE(f_b + distance - 1 <= f_end);
        REQUIRE(f_b + 20 >= f_end - 1);
    }
}

TEST_CASE("Range to containers", "[Range][To container]") {
    constexpr int size = 10;
    auto range = lz::range(size);

    SECTION("To array") {
        std::array<int, static_cast<std::size_t>(size)> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        auto actual = range.to<std::array<int, static_cast<std::size_t>(size)>>();

        REQUIRE(expected == actual);
    }

    SECTION("To vector") {
        std::vector<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        auto actual = range.to_vector();

        REQUIRE(expected == actual);
    }

    SECTION("To other container using to<>()") {
        std::list<int> expected = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        auto actual = range.to<std::list<int>>();

        REQUIRE(expected == actual);
    }

    SECTION("To map") {
        std::map<int, int> expected = range.to_map([](const int i) { return std::make_pair(i, i); });
        std::map<int, int> actual;

        for (int i : lz::range(size)) {
            actual.insert(std::make_pair(i, i));
        }

        REQUIRE(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> expected = range.to_unordered_map([](const int i) { return std::make_pair(i, i); });
        std::unordered_map<int, int> actual;

        for (int i : lz::range(size)) {
            actual.insert(std::make_pair(i, i));
        }

        REQUIRE(expected == actual);
    }
}
