#include <Lz/algorithm.hpp>
#include <Lz/common.hpp>
#include <Lz/map.hpp>
#include <c_string/c_string_forward_decl.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Empty or one element c_string") {
    SECTION("Empty") {
        auto cstr = lz::c_string("");
        REQUIRE(lz::empty(cstr));
        REQUIRE(!lz::has_one(cstr));
        REQUIRE(!lz::has_many(cstr));
    }

    SECTION("One element") {
        auto cstr = lz::c_string("a");
        REQUIRE(!lz::empty(cstr));
        REQUIRE(lz::has_one(cstr));
        REQUIRE(!lz::has_many(cstr));
    }
}

TEST_CASE("CString binary operations") {
    const char string[] = "123 456 789";
    lz::c_string_iterable<const char> c_string = string | lz::c_string;

    SECTION("Operator++") {
        lz::string_view expected1 = "123 456 789";
        REQUIRE(lz::equal(c_string, expected1));
    }

    SECTION("Operator bool") {
        REQUIRE(c_string.begin());
        auto tmp = lz::c_string("");
        REQUIRE(!tmp.begin());
        tmp = lz::c_string_iterable<const char>{};
        REQUIRE(!tmp.begin());
    }

    SECTION("Operator=") {
        auto it = c_string.begin();
        REQUIRE(it == c_string.begin());
        REQUIRE(it != c_string.end());
        REQUIRE(c_string.end() != it);
        REQUIRE(c_string.begin() == it);
        it = c_string.end();
        REQUIRE(it == c_string.end());
        REQUIRE(it != c_string.begin());
        REQUIRE(c_string.begin() != it);
        REQUIRE(c_string.end() == it);
    }
}

TEST_CASE("CString to containers") {
    auto str = lz::c_string("Hello, World!");

    SECTION("To array") {
        std::array<char, 14> expected = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
        REQUIRE((str | lz::to<std::array<char, 14>>()) == expected);
    }

    SECTION("To vector") {
        std::vector<char> expected = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
        REQUIRE((str | lz::to<std::vector>()) == expected);
    }

    SECTION("To other container using to<>()") {
        std::list<char> expected = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
        REQUIRE((str | lz::to<std::list>()) == expected);
    }

    SECTION("To map") {
        std::map<char, char> expected = { { 'H', 'H' }, { 'e', 'e' }, { 'l', 'l' }, { 'o', 'o' }, { ',', ',' },
                                          { ' ', ' ' }, { 'W', 'W' }, { 'r', 'r' }, { 'd', 'd' }, { '!', '!' } };
        REQUIRE((str | lz::map([](char c) { return std::make_pair(c, c); }) | lz::to<std::map<char, char>>()) == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<char, char> expected = { { 'H', 'H' }, { 'e', 'e' }, { 'l', 'l' }, { 'o', 'o' }, { ',', ',' },
                                                    { ' ', ' ' }, { 'W', 'W' }, { 'r', 'r' }, { 'd', 'd' }, { '!', '!' } };
        REQUIRE((str | lz::map([](char c) { return std::make_pair(c, c); }) | lz::to<std::unordered_map<char, char>>()) ==
                expected);
    }
}
