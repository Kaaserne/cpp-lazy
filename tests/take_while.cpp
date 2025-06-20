#include <Lz/drop_while.hpp>
#include <Lz/map.hpp>
#include <Lz/take_while.hpp>
#include <c_string/c_string_forward_decl.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

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

TEST_CASE("take_while_iterable takes elements and is by reference") {
    constexpr size_t size = 10;
    std::array<int, size> array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    SECTION("Should take elements") {
        auto take_while = lz::take_while(array, [](int element) { return element < 5; });
        auto it = take_while.begin();
        REQUIRE(std::distance(it, take_while.end()) == 4);
    }

    SECTION("Should be by reference") {
        auto take_while = lz::take_while(array, [](int element) { return element < 5; });
        auto it = take_while.begin();
        *it = 50;
        REQUIRE(array[0] == 50);
    }
}

TEST_CASE("Empty or one element drop while") {
    SECTION("Empty") {
        std::vector<int> vec;
        auto drop_while = lz::drop_while(vec, [](int i) { return i == 2; });
        REQUIRE(lz::empty(drop_while));
        REQUIRE(!lz::has_many(drop_while));
        REQUIRE(!lz::has_one(drop_while));
    }

    SECTION("One element true") {
        std::vector<int> vec = { 1 };
        auto drop_while = lz::drop_while(vec, [](int i) { return i == 1; });
        REQUIRE(lz::empty(drop_while));
        REQUIRE(!lz::has_many(drop_while));
        REQUIRE(!lz::has_one(drop_while));
    }

    SECTION("One element false") {
        std::vector<int> vec = { 1 };
        auto drop_while = lz::drop_while(vec, [](int i) { return i != 1; });
        REQUIRE(!lz::empty(drop_while));
        REQUIRE(!lz::has_many(drop_while));
        REQUIRE(lz::has_one(drop_while));
    }
}

TEST_CASE("Drop while takes elements and is by reference") {
    constexpr size_t size = 10;
    std::array<int, size> array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    SECTION("Should drop elements") {
        auto drop_while = lz::drop_while(array, [](int element) { return element < 5; });
        auto it = drop_while.begin();
        REQUIRE(std::distance(it, drop_while.end()) == 6);
    }

    SECTION("Should be by reference") {
        auto drop_while = lz::drop_while(array, [](int element) { return element < 5; });
        auto it = drop_while.begin();
        *it = 50;
        REQUIRE(array[4] == 50);
        REQUIRE(array[4] != 6);
        REQUIRE(array[0] == 1);
    }
}

TEST_CASE("take_while_iterable binary operations") {
    constexpr size_t size = 10;
    std::array<int, size> array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take_while = lz::take_while(array, [](int element) { return element < 5; });

    SECTION("Operator++") {
        auto begin = take_while.begin();
        ++begin;
        REQUIRE(*begin == 2);
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(take_while.begin() != take_while.end());
        REQUIRE_FALSE(take_while.begin() == take_while.end());
        auto it = take_while.begin();
        it = take_while.end();
        REQUIRE(it == take_while.end());
    }

    SECTION("Operator--") {
        auto end = take_while.end();
        --end;
        REQUIRE(*end == 4);
        --end;
        REQUIRE(*end == 3);
        --end;
        REQUIRE(*end == 2);
        --end;
        REQUIRE(*end == 1);
    }
}

TEST_CASE("take_while_iterable to containers") {
    constexpr size_t size = 10;
    std::array<int, size> array{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take_while = lz::take_while(array, [](int element) { return element < 5; });

    SECTION("To array") {
        auto arr = take_while | lz::to<std::array<int, 4>>();
        REQUIRE(arr.size() == 4);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
        REQUIRE(arr[3] == 4);
    }

    SECTION("To vector") {
        auto vec = take_while | lz::to<std::vector>();
        REQUIRE(std::equal(vec.begin(), vec.end(), take_while.begin()));
    }

    SECTION("To other container using to<>()") {
        auto lst = take_while | lz::to<std::list>();
        REQUIRE(std::equal(lst.begin(), lst.end(), take_while.begin()));
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