#include <Lz/common.hpp>
#include <Lz/intersection.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

TEST_CASE("Intersection tests with sentinels") {
    auto str = lz::c_string("aaaabbcccddee");
    auto str2 = lz::c_string("aabccce");
    lz::intersection_iterable<decltype(str), decltype(str2)> intersect = lz::intersection(str, str2);
    static_assert(!std::is_same<decltype(intersect.begin()), decltype(intersect.end())>::value, "Must be sentinel");
    REQUIRE((intersect | lz::to<std::string>()) == "aabccce");

    std::swap(str, str2);
    intersect = lz::intersection(str, str2, MAKE_BIN_PRED(less){});
    REQUIRE((intersect | lz::to<std::string>()) == "aabccce");

    SUBCASE("Operator=") {
        std::forward_list<int> a = { 1, 2, 3, 4, 5 };
        std::forward_list<int> b = { 2, 4, 6 };
        auto intersected = lz::intersection(a, b);
        auto common = lz::common(intersected);
        auto expected = { 2, 4 };
        REQUIRE(lz::equal(common, expected));
    }
}

TEST_CASE("Empty or one element intersection") {
    SUBCASE("All empty") {
        std::string a;
        std::string b;
        auto intersect = a | lz::intersection(b);
        REQUIRE(lz::empty(intersect));
        REQUIRE_FALSE(lz::has_one(intersect));
        REQUIRE_FALSE(lz::has_many(intersect));
    }

    SUBCASE("One element 1") {
        std::string a = "h";
        std::string b;
        auto intersect = a | lz::intersection(b, MAKE_BIN_PRED(less){});
        REQUIRE(lz::empty(intersect));
        REQUIRE_FALSE(lz::has_one(intersect));
        REQUIRE_FALSE(lz::has_many(intersect));
    }

    SUBCASE("One element 2") {
        std::string a;
        std::string b = "w";
        auto intersect = lz::intersection(a, b);
        REQUIRE(lz::empty(intersect));
        REQUIRE_FALSE(lz::has_one(intersect));
        REQUIRE_FALSE(lz::has_many(intersect));
    }

    SUBCASE("One element both 1") {
        std::string a = "h";
        std::string b = "h";
        auto intersect = lz::intersection(a, b);
        REQUIRE_FALSE(lz::empty(intersect));
        REQUIRE(lz::has_one(intersect));
        REQUIRE_FALSE(lz::has_many(intersect));
    }

    SUBCASE("One element both 2") {
        std::string a = "h";
        std::string b = "w";
        auto intersect = lz::intersection(a, b);
        REQUIRE(lz::empty(intersect));
        REQUIRE_FALSE(lz::has_one(intersect));
        REQUIRE_FALSE(lz::has_many(intersect));
    }
}

TEST_CASE("Intersection binary operations") {
    std::string a = "aaaabbcccddee";
    std::string b = "aabccce";

    SUBCASE("Operator++ 1") {
        auto intersect = lz::intersection(a, b);
        auto excpected = lz::c_string("aabccce");
        REQUIRE(lz::equal(intersect, excpected));
    }

    SUBCASE("Operator-- 1") {
        auto intersect = lz::intersection(a, b);
        auto expected = lz::c_string("ecccbaa");
        REQUIRE(lz::equal(intersect | lz::reverse, expected));
    }

    SUBCASE("Operator++ 2") {
        auto intersect = lz::intersection(b, a);
        auto expected = lz::c_string("aabccce");
        REQUIRE(lz::equal(intersect, expected));
    }

    SUBCASE("Operator-- 2") {
        auto intersect = lz::intersection(b, a);
        auto expected = lz::c_string("ecccbaa");
        REQUIRE(lz::equal(intersect | lz::reverse, expected));
    }

    SUBCASE("Operator=") {
        auto intersect = lz::intersection(a, b);
        auto it = intersect.begin();
        auto it2 = intersect.end();
        REQUIRE(it != it2);
        it2 = it;
        REQUIRE(it == it2);
    }
}

TEST_CASE("To container intersection") {
    std::string a = "aaaabbcccddee";
    std::string b = "aabccce";
    auto intersect = lz::intersection(a, b);

    SUBCASE("To array") {
        auto intersected = intersect | lz::to<std::array<char, 7>>();
        REQUIRE(intersected == std::array<char, 7>{ 'a', 'a', 'b', 'c', 'c', 'c', 'e' });
    }

    SUBCASE("To vector") {
        auto intersected = intersect | lz::to<std::vector<char>>();
        REQUIRE(intersected == std::vector<char>{ 'a', 'a', 'b', 'c', 'c', 'c', 'e' });
    }

    SUBCASE("To other container using to<>()") {
        auto intersected = intersect | lz::to<std::list<char>>();
        REQUIRE(intersected == std::list<char>{ 'a', 'a', 'b', 'c', 'c', 'c', 'e' });
    }

    SUBCASE("To map") {
        auto intersected =
            intersect | lz::map([](const char i) { return std::make_pair(i, i); }) | lz::to<std::map<char, char>>();
        std::map<char, char> expected = {
            std::make_pair('a', 'a'),
            std::make_pair('b', 'b'),
            std::make_pair('c', 'c'),
            std::make_pair('e', 'e'),
        };
        REQUIRE(intersected == expected);
    }

    SUBCASE("To unordered map") {
        auto intersected =
            intersect | lz::map([](const char i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<char, char>>();
        std::unordered_map<char, char> expected = {
            std::make_pair('a', 'a'),
            std::make_pair('b', 'b'),
            std::make_pair('c', 'c'),
            std::make_pair('e', 'e'),
        };
        REQUIRE(intersected == expected);
    }
}
