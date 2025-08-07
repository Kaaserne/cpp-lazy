#include <Lz/common.hpp>
#include <Lz/map.hpp>
#include <Lz/repeat.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <doctest/doctest.h>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

TEST_CASE("Basic common_iterable test") {
    const char* s = "hello, world!";
    auto cstr = lz::c_string(s);
    lz::common_iterable<lz::c_string_iterable<const char>> common = lz::common(cstr);
    REQUIRE(common.begin() != common.end());
    REQUIRE(common.end() != common.begin());
    REQUIRE(std::find(common.begin(), common.end(), 'h') == common.begin());
    REQUIRE(lz::equal(common, cstr));
}

TEST_CASE("common_iterable binary operations fwd") {
    const char* a = "hello ";
    auto c_string_view = lz::c_string(a);
    auto common = lz::common(c_string_view);

    SUBCASE("Operator++") {
        REQUIRE(lz::equal(common, c_string_view));
    }

    SUBCASE("Operator== & operator!=") {
        auto it = common.begin();
        REQUIRE(it != common.end());
        REQUIRE(common.end() != it);
        it = common.end();
        REQUIRE(it == common.end());
        REQUIRE(common.end() != common.begin());
    }
}

TEST_CASE("common_iterable random access") {
    auto repeater = lz::repeat(20, 5);
    auto common = lz::common(repeater);
    static_assert(std::is_same<decltype(common), lz::basic_iterable<lz::iter_t<decltype(repeater)>>>::value, "");
    static_assert(lz::detail::is_ra<lz::iter_t<decltype(repeater)>>::value, "");

    SUBCASE("Operator++") {
        REQUIRE(lz::equal(common, repeater));
    }
}

TEST_CASE("common_iterable empty or one element") {
    SUBCASE("Empty") {
        auto c_str = lz::c_string("");
        auto common = lz::common(c_str);
        REQUIRE(lz::empty(common));
        REQUIRE_FALSE(lz::has_one(common));
        REQUIRE_FALSE(lz::has_many(common));
    }

    SUBCASE("One element") {
        auto c_str = lz::c_string("a");
        auto common = lz::common(c_str);
        REQUIRE_FALSE(lz::empty(common));
        REQUIRE(lz::has_one(common));
        REQUIRE_FALSE(lz::has_many(common));
    }
}

TEST_CASE("common_iterable to containers") {
    const char* a = "hello ";
    auto c_string_view = lz::c_string(a);
    auto common = lz::common(c_string_view);

    SUBCASE("To array") {
        REQUIRE((common | lz::to<std::array<char, 6>>()) == std::array<char, 6>{ 'h', 'e', 'l', 'l', 'o', ' ' });
    }

    SUBCASE("To vector") {
        REQUIRE((common | lz::to<std::vector>()) == std::vector<char>{ 'h', 'e', 'l', 'l', 'o', ' ' });
    }

    SUBCASE("To other container using to<>()") {
        REQUIRE((common | lz::to<std::list<char>>()) == std::list<char>{ 'h', 'e', 'l', 'l', 'o', ' ' });
    }

    SUBCASE("To map") {
        REQUIRE((c_string_view | lz::map([](char c) { return std::make_pair(c, c); }) | lz::common |
                 lz::to<std::map<char, char>>()) ==
                std::map<char, char>{ { 'h', 'h' }, { 'e', 'e' }, { 'l', 'l' }, { 'o', 'o' }, { ' ', ' ' } });
    }

    SUBCASE("To unordered map") {
        REQUIRE((c_string_view | lz::map([](char c) { return std::make_pair(c, c); }) | lz::common |
                 lz::to<std::unordered_map<char, char>>()) ==
                std::unordered_map<char, char>{ { 'h', 'h' }, { 'e', 'e' }, { 'l', 'l' }, { 'o', 'o' }, { ' ', ' ' } });
    }
}