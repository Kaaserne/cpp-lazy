#include <Lz/c_string.hpp>
#include <Lz/common.hpp>
#include <catch2/catch.hpp>
#include <list>

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

TEST_CASE("CString binary operations", "[CString][Binary ops]") {
    const char* string = "123 456 789";
    auto c_string = string | lz::c_string;

    SECTION("Operator++") {
        auto begin = c_string.begin();
        REQUIRE(*begin == '1');
        ++begin;
        REQUIRE(*begin == '2');
    }

    SECTION("Operator== & Operator!=") {
        auto beg = c_string.begin();
        REQUIRE(beg != c_string.end());

        auto common = lz::common(c_string);
        auto begin_common = common.begin();
        auto end_common = common.end();
        REQUIRE(begin_common != end_common);
        begin_common = common.end();
        REQUIRE(begin_common == end_common);
    }

    SECTION("Operator bool") {
        REQUIRE(c_string.begin());

        auto tmp = lz::c_string("");
        REQUIRE(!tmp.begin());
    }
}

// TODO: Add tests for c_string with different types
// TEST_CASE("CString to containers", "[CString][To container]") {
//     auto str = lz::c_string("Hello, World!");
//     auto map_str = lz::c_string("123456789");

//     SECTION("To array") {
//         std::array<char, 14> expected = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
//         REQUIRE(str.to<std::array<char, 14>>() == expected);
//     }

//     SECTION("To vector") {
//         std::vector<char> expected = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
//         REQUIRE(str.to_vector() == expected);
//     }

//     SECTION("To other container using to<>()") {
//         std::list<char> expected = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' };
//         REQUIRE(str.to<std::list<char>>() == expected);
//     }

//     SECTION("To map") {
//         std::map<char, char> expected = { { '1', '1' }, { '2', '2' }, { '3', '3' }, { '4', '4' }, { '5', '5' },
//                                           { '6', '6' }, { '7', '7' }, { '8', '8' }, { '9', '9' } };
//         REQUIRE(map_str.to_map([](char c) { return std::make_pair(c, c); }) == expected);
//     }

//     SECTION("To unordered map") {
//         std::unordered_map<char, char> expected = { { '1', '1' }, { '2', '2' }, { '3', '3' }, { '4', '4' }, { '5', '5' },
//                                                     { '6', '6' }, { '7', '7' }, { '8', '8' }, { '9', '9' } };
//         REQUIRE(map_str.to_unordered_map([](char c) { return std::make_pair(c, c); }) == expected);
//     }
// }