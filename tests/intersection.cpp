#include <Lz/algorithm/empty.hpp>
#include <Lz/algorithm/equal.hpp>
#include <Lz/algorithm/has_many.hpp>
#include <Lz/algorithm/has_one.hpp>
#include <Lz/c_string.hpp>
#include <Lz/intersection.hpp>
#include <Lz/map.hpp>
#include <Lz/procs/to.hpp>
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
    intersect = lz::intersection(str, str2, LZ_BIN_OP(less, char){});
    REQUIRE((intersect | lz::to<std::string>()) == "aabccce");
}

TEST_CASE("Operator=(default_sentinel_t)") {
    SUBCASE("forward") {
        std::forward_list<int> a = { 1, 2, 3, 4, 5 };
        std::forward_list<int> b = { 2, 4, 6 };
        auto intersected = lz::intersection(a, b);
        auto common = make_sentinel_assign_op_tester(intersected);
        auto expected = { 2, 4 };
        REQUIRE(lz::equal(common, expected));

        intersected = lz::intersection(b, a);
        common = make_sentinel_assign_op_tester(intersected);
        REQUIRE(lz::equal(common, expected));
    }

    SUBCASE("bidirectional") {
        std::vector<int> a = { 1, 2, 3, 4, 5 };
        std::vector<int> b = { 2, 4, 6 };
        auto intersected = lz::intersection(make_sized_bidi_sentinelled(a), make_sized_bidi_sentinelled(b));
        auto common = make_sentinel_assign_op_tester(intersected);
        auto expected = { 2, 4 };
        REQUIRE(lz::equal(common, expected));
        REQUIRE(lz::equal(common | lz::reverse, expected | lz::reverse));
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
        auto intersect = a | lz::intersection(b, LZ_BIN_OP(less, char){});
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
    SUBCASE("Operator++/-- 1") {
        std::string a = "aaaabbcccddee";
        std::string b = "aabccce";
        auto intersect = lz::intersection(a, b);
        std::string expected = "aabccce";
        REQUIRE(lz::equal(intersect, expected));
        REQUIRE(lz::equal(intersect | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("Operator++/-- 2") {
        std::string a = "aaaabbcccddee";
        std::string b = "aabccce";
        auto intersect = lz::intersection(b, a);
        std::string expected = "aabccce";
        REQUIRE(lz::equal(intersect, expected));
        REQUIRE(lz::equal(intersect | lz::reverse, expected | lz::reverse));
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
