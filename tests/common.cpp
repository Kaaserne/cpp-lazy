#include <Lz/common.hpp>
#include <Lz/map.hpp>
#include <c_string/c_string_forward_decl.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Basic common_iterable test") {
    const char* s = "hello, world!";
    auto cstr = lz::c_string(s);
    lz::common_iterable<lz::c_string_iterable<const char>> common = lz::common(cstr);
    REQUIRE(common.begin() != common.end());
    REQUIRE(common.end() != common.begin());
    REQUIRE(std::find(common.begin(), common.end(), 'h') == common.begin());
    REQUIRE(std::equal(common.begin(), common.end(), cstr.begin()));
}

TEST_CASE("common_iterable binary operations") {
    const char* a = "hello ";
    auto c_string_view = lz::c_string(a);
    auto common = lz::common(c_string_view);

    SECTION("Operator++") {
        REQUIRE(lz::equal(common, c_string_view));
    }

    SECTION("Operator== & operator!=") {
        auto it = common.begin();
        REQUIRE(it != common.end());
        REQUIRE(common.end() != it);
        it = common.end();
        REQUIRE(it == common.end());
        REQUIRE(common.end() != common.begin());
    }
}

TEST_CASE("common_iterable empty or one element") {
    SECTION("Empty") {
        auto c_str = lz::c_string("");
        auto common = lz::common(c_str);
        REQUIRE(lz::empty(common));
        REQUIRE(!lz::has_one(common));
        REQUIRE(!lz::has_many(common));
    }

    SECTION("One element") {
        auto c_str = lz::c_string("a");
        auto common = lz::common(c_str);
        REQUIRE(!lz::empty(common));
        REQUIRE(lz::has_one(common));
        REQUIRE(!lz::has_many(common));
    }
}

TEST_CASE("common_iterable to containers") {
    const char* a = "hello ";
    auto c_string_view = lz::c_string(a);
    auto common = lz::common(c_string_view);

    SECTION("To array") {
        REQUIRE((common | lz::to<std::array<char, 6>>()) == std::array<char, 6>{ 'h', 'e', 'l', 'l', 'o', ' ' });
    }

    SECTION("To vector") {
        REQUIRE((common | lz::to<std::vector>()) == std::vector<char>{ 'h', 'e', 'l', 'l', 'o', ' ' });
    }

    SECTION("To other container using to<>()") {
        REQUIRE((common | lz::to<std::list<char>>()) == std::list<char>{ 'h', 'e', 'l', 'l', 'o', ' ' });
    }

    SECTION("To map") {
        REQUIRE((c_string_view | lz::map([](char c) { return std::make_pair(c, c); }) | lz::common |
                 lz::to<std::map<char, char>>()) ==
                std::map<char, char>{ { 'h', 'h' }, { 'e', 'e' }, { 'l', 'l' }, { 'o', 'o' }, { ' ', ' ' } });
    }

    SECTION("To unordered map") {
        REQUIRE((c_string_view | lz::map([](char c) { return std::make_pair(c, c); }) | lz::common |
                 lz::to<std::unordered_map<char, char>>()) ==
                std::unordered_map<char, char>{ { 'h', 'h' }, { 'e', 'e' }, { 'l', 'l' }, { 'o', 'o' }, { ' ', ' ' } });
    }
}