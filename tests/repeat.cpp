#include <Lz/common.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cpp-lazy-ut-helper/repeat.hpp>
#include <list>
#include <map>
#include <test_procs.hpp>
#include <unordered_map>

TEST_CASE("repeat_iterable binary operations") {
    const int amount = 5;
    auto repeater = lz::repeat(20, amount);
    auto expected = { 20, 20, 20, 20, 20 };

    SECTION("Operator++") {
        REQUIRE(lz::equal(repeater, expected));
    }

    SECTION("Operator--") {
        REQUIRE(lz::equal(repeater | lz::reverse, expected));
    }

    SECTION("Operator+") {
        test_procs::test_operator_plus(repeater, expected);

        auto iterable = lz::common(repeater);
        test_procs::test_operator_plus(iterable, expected);
    }

    SECTION("Operator-") {
        test_procs::test_operator_minus(repeater);
        auto iterable = lz::common(repeater);
        test_procs::test_operator_minus(iterable);
    }
}

TEST_CASE("Empty or one element repeat") {
    SECTION("Empty") {
        auto repeater = lz::repeat(20, 0);
        REQUIRE(lz::empty(repeater));
        REQUIRE_FALSE(lz::has_one(repeater));
        REQUIRE_FALSE(lz::has_many(repeater));
    }

    SECTION("One element with result") {
        auto repeater = lz::repeat(20, 1);
        REQUIRE_FALSE(lz::empty(repeater));
        REQUIRE(lz::has_one(repeater));
        REQUIRE_FALSE(lz::has_many(repeater));
    }
}

TEST_CASE("repeat_iterable to containers") {
    constexpr auto times = 5;
    const int to_repeat = 20;
    auto repeater = lz::repeat(to_repeat, times);

    SECTION("To array") {
        std::array<int, times> array = repeater | lz::to<std::array<int, times>>();
        auto expected = { 20, 20, 20, 20, 20 };
        REQUIRE(lz::equal(array, expected));
    }

    SECTION("To vector") {
        std::vector<int> vec = repeater | lz::to<std::vector>();
        auto expected = { 20, 20, 20, 20, 20 };
        REQUIRE(lz::equal(vec, expected));
    }

    SECTION("To other container using to<>()") {
        std::list<int> lst = repeater | lz::to<std::list>();
        auto expected = { 20, 20, 20, 20, 20 };
        REQUIRE(lz::equal(lst, expected));
    }

    SECTION("To map") {
        auto actual = repeater | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();
        std::map<int, int> expected = { { 20, 20 }, { 20, 20 }, { 20, 20 }, { 20, 20 }, { 20, 20 } };
        REQUIRE(actual == expected);
    }

    SECTION("To unordered map") {
        auto actual =
            repeater | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        std::unordered_map<int, int> expected = { { 20, 20 }, { 20, 20 }, { 20, 20 }, { 20, 20 }, { 20, 20 } };
        REQUIRE(actual == expected);
    }
}

TEST_CASE("repeat_iterable infinite") {
    int to_repeat = 20;
    lz::repeat_iterable_inf<int> repeater = lz::repeat(to_repeat);

    SECTION("Should be infinite") {
        std::size_t counter = 0;
        lz::for_each_while(repeater, [&counter](int i) {
            REQUIRE(i == 20);
            ++counter;
            if (counter == 100) {
                return false;
            }
            return true;
        });
        REQUIRE(counter == 100);
    }

    SECTION("Should not be by reference") {
        auto start = repeater.begin();
        REQUIRE(&(*start) != &to_repeat);
    }
}

TEST_CASE("repeat_iterable infinite binary operations") {
    auto repeater = lz::repeat(20);
    auto begin = repeater.begin();

    SECTION("Operator++") {
        ++begin;
        REQUIRE(*begin == 20);
    }

    SECTION("Not empty, has many, not has one") {
        REQUIRE_FALSE(lz::empty(repeater));
        REQUIRE(lz::has_many(repeater));
        REQUIRE_FALSE(lz::has_one(repeater));
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(begin != repeater.end());
        std::size_t counter = 0;
        while (begin != repeater.end()) {
            ++begin;
            ++counter;
            if (counter == 100) {
                break;
            }
        }
        REQUIRE(counter == 100);
        REQUIRE(begin != repeater.end());
        REQUIRE(begin != begin);
    }
}