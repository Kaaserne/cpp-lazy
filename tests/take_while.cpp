#include <Lz/drop_while.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <Lz/take_while.hpp>
#include <catch2/catch.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <forward_list>
#include <list>
#include <map>
#include <unordered_map>

#ifdef __GNUC__

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"

#endif

TEST_CASE("Take while with sentinels") {
    auto cstr = lz::c_string("Hello, World!");
    std::function<bool(char)> condition = [](char c) {
        return c != 'W';
    };
    lz::take_while_iterable<decltype(cstr), decltype(condition)> take_while = lz::take_while(cstr, std::move(condition));
    static_assert(!std::is_same<decltype(take_while.begin()), decltype(take_while.end())>::value, "Should be sentinel");
    auto c_str_expected = lz::c_string("Hello, ");
    REQUIRE(lz::equal(take_while, c_str_expected));

    SECTION("Operator=") {
        auto it = take_while.begin();
        REQUIRE(it == take_while.begin());
        it = take_while.end();
        REQUIRE(it == take_while.end());
    }
}

TEST_CASE("Empty or one element drop while") {
    SECTION("Empty") {
        std::vector<int> vec;
        auto drop_while = lz::drop_while(vec, [](int i) { return i == 2; });
        REQUIRE(lz::empty(drop_while));
        REQUIRE_FALSE(lz::has_many(drop_while));
        REQUIRE_FALSE(lz::has_one(drop_while));
    }

    SECTION("One element true") {
        std::vector<int> vec = { 1 };
        auto drop_while = lz::drop_while(vec, [](int i) { return i == 1; });
        REQUIRE(lz::empty(drop_while));
        REQUIRE_FALSE(lz::has_many(drop_while));
        REQUIRE_FALSE(lz::has_one(drop_while));
    }

    SECTION("One element false") {
        std::vector<int> vec = { 1 };
        auto drop_while = lz::drop_while(vec, [](int i) { return i != 1; });
        REQUIRE_FALSE(lz::empty(drop_while));
        REQUIRE_FALSE(lz::has_many(drop_while));
        REQUIRE(lz::has_one(drop_while));
    }

    SECTION("Many elements both true") {
        std::vector<int> vec = { 1, 2 };
        auto drop_while = lz::drop_while(vec, [](int i) { return i < 3; });
        REQUIRE(lz::empty(drop_while));
        REQUIRE_FALSE(lz::has_many(drop_while));
        REQUIRE_FALSE(lz::has_one(drop_while));
    }

    SECTION("Many elements first true") {
        std::vector<int> vec = { 1, 2 };
        auto drop_while = lz::drop_while(vec, [](int i) { return i == 1; });
        REQUIRE_FALSE(lz::empty(drop_while));
        REQUIRE_FALSE(lz::has_many(drop_while));
        REQUIRE(lz::has_one(drop_while));
    }

    SECTION("Many elements first false") {
        std::vector<int> vec = { 1, 2 };
        auto drop_while = lz::drop_while(vec, [](int i) { return i > 1; });
        REQUIRE_FALSE(lz::empty(drop_while));
        REQUIRE(lz::has_many(drop_while));
        REQUIRE_FALSE(lz::has_one(drop_while));
    }

    SECTION("Many elements all false") {
        std::vector<int> vec = { 1, 2 };
        auto drop_while = lz::drop_while(vec, [](int i) { return i > 2; });
        REQUIRE_FALSE(lz::empty(drop_while));
        REQUIRE(lz::has_many(drop_while));
        REQUIRE_FALSE(lz::has_one(drop_while));
    }
}

TEST_CASE("take_while_iterable binary operations random access") {
    constexpr size_t size = 10;
    std::array<int, size> array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take_while = lz::take_while(array, [](int element) { return element < 5; });

    SECTION("Operator++") {
        auto expected = { 1, 2, 3, 4 };
        REQUIRE(lz::equal(take_while, expected));
    }

    SECTION("Operator--") {
        auto expected = { 4, 3, 2, 1 };
        REQUIRE(lz::equal(take_while | lz::reverse, expected));
    }

    SECTION("Operator== & Operator!=") {
        auto it = take_while.begin();
        REQUIRE(it == take_while.begin());
        REQUIRE(it != take_while.end());
        REQUIRE(take_while.begin() == it);
        REQUIRE(take_while.end() != it);
        it = take_while.end();
        REQUIRE(it == take_while.end());
        REQUIRE(it != take_while.begin());
        REQUIRE(take_while.end() == it);
        REQUIRE(take_while.begin() != it);
    }
}

TEST_CASE("take_while_iterable binary operations bidirectional access") {
    std::list<int> lst{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take_while = lz::take_while(lst, [](int element) { return element < 5; });

    SECTION("Operator++") {
        auto expected = { 1, 2, 3, 4 };
        REQUIRE(lz::equal(take_while, expected));
    }

    SECTION("Operator== & Operator!=") {
        auto it = take_while.begin();
        REQUIRE(it == take_while.begin());
        REQUIRE(it != take_while.end());
        REQUIRE(take_while.begin() == it);
        REQUIRE(take_while.end() != it);
        it = take_while.end();
        REQUIRE(it == take_while.end());
        REQUIRE(it != take_while.begin());
        REQUIRE(take_while.end() == it);
        REQUIRE(take_while.begin() != it);
    }

    SECTION("Operator--") {
        auto expected = { 4, 3, 2, 1 };
        REQUIRE(lz::equal(take_while | lz::reverse, expected));
    }
}

TEST_CASE("take_while_iterable binary operations forward access") {
    std::forward_list<int> fl{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take_while = lz::take_while(fl, [](int element) { return element < 5; });

    SECTION("Operator++") {
        auto expected = { 1, 2, 3, 4 };
        REQUIRE(lz::equal(take_while, expected));
    }

    SECTION("Operator== & Operator!=") {
        auto it = take_while.begin();
        REQUIRE(it == take_while.begin());
        REQUIRE(it != take_while.end());
        REQUIRE(take_while.begin() == it);
        REQUIRE(take_while.end() != it);
        it = take_while.end();
        REQUIRE(it == take_while.end());
        REQUIRE(it != take_while.begin());
        REQUIRE(take_while.end() == it);
        REQUIRE(take_while.begin() != it);
    }
}

TEST_CASE("take_while_iterable to containers") {
    constexpr size_t size = 10;
    std::array<int, size> array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take_while = lz::take_while(array, [](int element) { return element < 5; });

    SECTION("To array") {
        REQUIRE(lz::distance(take_while) == 4);
        auto arr = take_while | lz::to<std::array<int, 4>>();
        auto expected = { 1, 2, 3, 4 };
        REQUIRE(lz::equal(arr, expected));
    }

    SECTION("To vector") {
        auto vec = take_while | lz::to<std::vector>();
        REQUIRE(lz::equal(vec, take_while));
    }

    SECTION("To other container using to<>()") {
        auto lst = take_while | lz::to<std::list>();
        REQUIRE(lz::equal(lst, take_while));
    }

    SECTION("To map") {
        auto map = take_while | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();
        REQUIRE(map.size() == 4);
        std::map<int, int> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
        REQUIRE(map == expected);
    }

    SECTION("To unordered map") {
        auto map = take_while | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        REQUIRE(map.size() == 4);
        std::unordered_map<int, int> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
        REQUIRE(map == expected);
    }
}

#ifdef __GNUC__

#pragma GCC diagnostic pop

#endif