#include <Lz/algorithm.hpp>
#include <Lz/common.hpp>
#include <Lz/except.hpp>
#include <Lz/map.hpp>
#include <Lz/range.hpp>
#include <Lz/reverse.hpp>
#include <array>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <doctest/doctest.h>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

TEST_CASE("Except tests with sentinels") {
    const char* str = "Hello, World!";
    const char* to_except = "eo";
    auto c_str = lz::c_string(str);
    auto c_str_to_except = lz::c_string(to_except);
    lz::except_iterable<decltype(c_str), decltype(c_str_to_except)> except = lz::except(c_str, c_str_to_except);
    static_assert(!std::is_same<decltype(except.begin()), decltype(except.end())>::value, "Must be sentinel");
    REQUIRE((except | lz::to<std::string>()) == "Hll, Wrld!");

    SUBCASE("Operator=") {
        auto it = except.begin();
        REQUIRE(it == except.begin());
        REQUIRE(it != except.end());
        REQUIRE(except.end() != it);
        REQUIRE(except.begin() == it);
        it = except.end();
        REQUIRE(it != except.begin());
        REQUIRE(it == except.end());
        REQUIRE(except.begin() != it);
        REQUIRE(except.end() == it);
    }
}

TEST_CASE("Empty or one element except") {
    SUBCASE("Empty") {
        std::string a;
        std::string b;
        auto except = lz::except(a, b);
        REQUIRE(lz::empty(except));
        REQUIRE_FALSE(lz::has_one(except));
        REQUIRE_FALSE(lz::has_many(except));
    }

    SUBCASE("One element 1") {
        std::string a = "h";
        std::string b;
        auto except = lz::except(a, b);
        REQUIRE_FALSE(lz::empty(except));
        REQUIRE(lz::has_one(except));
        REQUIRE_FALSE(lz::has_many(except));
    }

    SUBCASE("One element 2") {
        std::string a;
        std::string b = "w";
        auto except = lz::except(a, b);
        REQUIRE(lz::empty(except));
        REQUIRE_FALSE(lz::has_one(except));
        REQUIRE_FALSE(lz::has_many(except));
    }

    SUBCASE("One element both") {
        std::string a = "h";
        std::string b = "w";
        auto except = lz::except(a, b);
        REQUIRE_FALSE(lz::empty(except));
        REQUIRE(lz::has_one(except));
        REQUIRE_FALSE(lz::has_many(except));
    }

    SUBCASE("One element both 2") {
        std::string a = "h";
        std::string b = "h";
        auto except = lz::except(a, b);
        REQUIRE(lz::empty(except));
        REQUIRE_FALSE(lz::has_one(except));
        REQUIRE_FALSE(lz::has_many(except));
    }
}

TEST_CASE("Except binary operations") {
    std::vector<int> a = { 1, 2, 3, 4, 5, 6 };
    std::vector<int> b = { 2, 3, 6, 20 };

    auto except = lz::except(a, b);

    SUBCASE("Operator++") {
        auto expected = { 1, 4, 5 };
        REQUIRE(lz::equal(except, expected));
    }

    SUBCASE("Operator--") {
        auto expected = { 5, 4, 1 };
        REQUIRE(lz::equal(except | lz::cached_reverse, expected));
    }
}

TEST_CASE("Except to containers") {
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<int> b = { 1, 3 };
    auto except = lz::except(a, b);

    SUBCASE("To array") {
        auto excepted = except | lz::to<std::array<int, 2>>();
        REQUIRE(excepted == std::array<int, 2>{ 2, 4 });
    }

    SUBCASE("To vector") {
        auto excepted = except | lz::to<std::vector>();
        REQUIRE(excepted == std::vector<int>{ 2, 4 });
    }

    SUBCASE("To other container using to<>()") {
        auto excepted = except | lz::to<std::list>();
        REQUIRE(excepted == std::list<int>{ 2, 4 });
    }

    SUBCASE("To map") {
        auto actual = except | lz::map([](const int i) { return std::make_pair(i, i); }) |
                      lz::to<std::map<int, int>>();

        std::map<int, int> expected = {
            std::make_pair(2, 2),
            std::make_pair(4, 4),
        };

        REQUIRE(actual == expected);
    }

    SUBCASE("To unordered map") {
        auto actual = except | lz::map([](const int i) { return std::make_pair(i, i); }) |
                      lz::to<std::unordered_map<int, int>>();

        std::unordered_map<int, int> expected = {
            std::make_pair(2, 2),
            std::make_pair(4, 4),
        };

        REQUIRE(actual == expected);
    }
}
