#include <Lz/c_string.hpp>
#include <Lz/intersection.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Intersection tests with sentinels") {
    auto str = lz::c_string("aaaabbcccddee");
    auto str2 = lz::c_string("aabccce");
    lz::intersection_iterable<decltype(str), decltype(str2)> intersect = lz::intersection(str, str2);
    static_assert(!std::is_same<decltype(intersect.begin()), decltype(intersect.end())>::value, "Must be sentinel");
    REQUIRE((intersect | lz::to<std::string>()) == "aabccce");

    std::swap(str, str2);
    intersect = lz::intersection(str, str2, MAKE_BIN_PRED(less){});
    REQUIRE((intersect | lz::to<std::string>()) == "aabccce");

    SECTION("Operator=") {
        auto begin = intersect.begin();
        REQUIRE(begin == intersect.begin());
        begin = intersect.end();
        REQUIRE(begin == intersect.end());
    }
}

TEST_CASE("Empty or one element intersection") {
    SECTION("All empty") {
        std::string a;
        std::string b;
        auto intersect = a | lz::intersection(b);
        REQUIRE(lz::empty(intersect));
        REQUIRE(!lz::has_one(intersect));
        REQUIRE(!lz::has_many(intersect));
    }

    SECTION("One element 1") {
        std::string a = "h";
        std::string b;
        auto intersect = a | lz::intersection(b, MAKE_BIN_PRED(less){});
        REQUIRE(lz::empty(intersect));
        REQUIRE(!lz::has_one(intersect));
        REQUIRE(!lz::has_many(intersect));
    }

    SECTION("One element 2") {
        std::string a;
        std::string b = "w";
        auto intersect = lz::intersection(a, b);
        REQUIRE(lz::empty(intersect));
        REQUIRE(!lz::has_one(intersect));
        REQUIRE(!lz::has_many(intersect));
    }

    SECTION("One element both 1") {
        std::string a = "h";
        std::string b = "h";
        auto intersect = lz::intersection(a, b);
        REQUIRE(!lz::empty(intersect));
        REQUIRE(lz::has_one(intersect));
        REQUIRE(!lz::has_many(intersect));
    }

    SECTION("One element both 2") {
        std::string a = "h";
        std::string b = "w";
        auto intersect = lz::intersection(a, b);
        REQUIRE(lz::empty(intersect));
        REQUIRE(!lz::has_one(intersect));
        REQUIRE(!lz::has_many(intersect));
    }
}

TEST_CASE("Intersection binary operations") {
    std::string a = "aaaabbcccddee";
    std::string b = "aabccce";

    SECTION("Operator++ 1") {
        auto intersect = lz::intersection(a, b);
        auto excpected = lz::c_string("aabccce");
        REQUIRE(lz::equal(intersect, excpected));
    }

    SECTION("Operator-- 1") {
        auto intersect = lz::intersection(a, b);
        auto expected = lz::c_string("ecccbaa");
        REQUIRE(lz::equal(intersect | lz::reverse, expected));
    }

    SECTION("Operator++ 2") {
        auto intersect = lz::intersection(b, a);
        auto expected = lz::c_string("aabccce");
        REQUIRE(lz::equal(intersect, expected));
    }

    SECTION("Operator-- 2") {
        auto intersect = lz::intersection(b, a);
        auto expected = lz::c_string("ecccbaa");
        REQUIRE(lz::equal(intersect | lz::reverse, expected));
    }

    SECTION("Operator=") {
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

    SECTION("To array") {
        auto intersected = intersect | lz::to<std::array<char, 7>>();
        REQUIRE(intersected == std::array<char, 7>{ 'a', 'a', 'b', 'c', 'c', 'c', 'e' });
    }

    SECTION("To vector") {
        auto intersected = intersect | lz::to<std::vector<char>>();
        REQUIRE(intersected == std::vector<char>{ 'a', 'a', 'b', 'c', 'c', 'c', 'e' });
    }

    SECTION("To other container using to<>()") {
        auto intersected = intersect | lz::to<std::list<char>>();
        REQUIRE(intersected == std::list<char>{ 'a', 'a', 'b', 'c', 'c', 'c', 'e' });
    }

    SECTION("To map") {
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

    SECTION("To unordered map") {
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
