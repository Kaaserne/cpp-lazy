#include <Lz/c_string.hpp>
#include <Lz/map.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <Lz/slice.hpp>
#include <Lz/take.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

TEST_CASE("Take with sentinels") {
    const char* str = "Hello, world!";
    auto c_string = lz::c_string(str);
    lz::take_iterable<decltype(c_string)> take = c_string | lz::take(5);
    static_assert(std::is_same<decltype(take.begin()), decltype(take.end())>::value, "Should be sentinel");
    auto expected = lz::c_string("Hello");
    REQUIRE(lz::equal(take, expected));
    auto vec = take | lz::to<std::vector<char>>();
    REQUIRE(lz::equal(vec, expected));
}

TEST_CASE("Operator=(default_sentinel_t)") {
    SUBCASE("with iterator forward") {
        std::forward_list<int> lst{ 1, 2, 3, 4, 5 };
        auto taken = lz::take(lst.begin(), 3);
        auto end = taken.begin();
        end = taken.end(); // calls operator=(sentinel)
        auto begin = taken.begin();
        auto common = lz::make_basic_iterable(begin, end);
        auto expected2 = { 1, 2, 3 };
        REQUIRE(lz::equal(common, expected2));
    }

    SUBCASE("with iterator bidirectional") {
        std::list<int> lst{ 1, 2, 3, 4, 5 };
        auto taken = lz::take(lst.begin(), 3);
        auto end = taken.begin();
        end = lz::default_sentinel; // calls operator=(sentinel)
        auto begin = taken.begin();
        auto common = lz::make_basic_iterable(begin, end);
        auto expected2 = { 1, 2, 3 };
        REQUIRE(lz::equal(common | lz::reverse, expected2 | lz::reverse));
    }

    SUBCASE("with iterator random access") {
        auto rep = lz::repeat(1, 5);
        auto taken = lz::take(rep.begin(), 3);
        auto end = taken.begin();
        end = lz::default_sentinel; // calls operator=(sentinel)
        auto begin = taken.begin();
        auto common = lz::make_basic_iterable(begin, end);
        auto expected = { 1, 1, 1 };
        test_procs::test_operator_minus(common);
        test_procs::test_operator_plus(common, expected);
    }
}

// TODO check enable ifs in adaptors for ambiguity
TEST_CASE("Take binary operations where n is smaller than size") {
    std::array<int, 10> array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take = lz::take(array, 4);
    REQUIRE(take.size() == 4);

    SUBCASE("Operator++") {
        auto expected = { 1, 2, 3, 4 };
        REQUIRE(lz::equal(take, expected));
    }

    SUBCASE("Operator--") {
        auto expected = { 4, 3, 2, 1 };
        REQUIRE(lz::equal(take | lz::reverse, expected));
    }

    SUBCASE("Operator== & Operator!=") {
        REQUIRE(take.begin() != take.end());
        REQUIRE(take.end() != take.begin());
        REQUIRE_FALSE(take.begin() == take.end());
        REQUIRE_FALSE(take.end() == take.begin());
        auto it = take.begin();
        it = take.end();
        REQUIRE(it == take.end());
        REQUIRE(take.end() == it);
        REQUIRE_FALSE(it != take.end());
        REQUIRE_FALSE(take.end() != it);
    }

    SUBCASE("Operator+") {
        std::vector<int> expected = { 1, 2, 3, 4 };
        test_procs::test_operator_plus(take, expected);
    }

    SUBCASE("Operator-") {
        test_procs::test_operator_minus(take);
    }

    SUBCASE("Operator-(default_sentinel_t)") {
        auto r = lz::repeat(1, 5) | lz::take(3);
        test_procs::test_operator_minus(r);

        auto rep = lz::repeat(0, 5);
        auto r2 = lz::take(rep.begin(), 3);
        test_procs::test_operator_minus(r2);
    }

    SUBCASE("Operator+(default_sentinel_t)") {
        auto r = lz::repeat(1, 5) | lz::take(3);
        std::vector<int> expected = { 1, 1, 1 };
        test_procs::test_operator_plus(r, expected);

        r = lz::repeat(0, 5) | lz::take(3);
        expected = { 0, 0, 0 };
        test_procs::test_operator_plus(r, expected);

        r = lz::repeat(0, 0) | lz::take(3);
        expected = {};
        test_procs::test_operator_plus(r, expected);
    }
}

TEST_CASE("Take binary operations where n is larger than size") {
    std::array<int, 10> array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take = lz::take(array, 20);
    REQUIRE(take.size() == 10);

    SUBCASE("Operator++") {
        auto begin = take.begin();
        ++begin;
        REQUIRE(*begin == 2);
    }

    SUBCASE("Operator--") {
        auto end = take.end();
        --end;
        REQUIRE(*end == 10);
    }

    SUBCASE("Operator== & Operator!=") {
        REQUIRE(take.begin() != take.end());
        REQUIRE_FALSE(take.begin() == take.end());
        auto it = take.begin();
        it = take.end();
        REQUIRE(it == take.end());
    }

    SUBCASE("Operator+") {
        INFO("Operator+");
        std::vector<int> expected = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        test_procs::test_operator_plus(take, expected);
    }

    SUBCASE("Operator-") {
        INFO("Operator-");
        test_procs::test_operator_minus(take);
    }
}

TEST_CASE("Empty or one element take") {
    SUBCASE("Empty") {
        std::vector<int> vec;
        auto take = lz::take(vec, 0);
        REQUIRE(take.size() == 0);
        REQUIRE(lz::empty(take));
        REQUIRE_FALSE(lz::has_one(take));
        REQUIRE_FALSE(lz::has_many(take));
    }

    SUBCASE("One element") {
        std::vector<int> vec = { 1 };
        auto take = lz::take(vec, 1);
        REQUIRE(take.size() == 1);
        REQUIRE_FALSE(lz::empty(take));
        REQUIRE(lz::has_one(take));
        REQUIRE_FALSE(lz::has_many(take));
    }

    SUBCASE("Empty iterator") {
        std::vector<int> vec;
        auto take = lz::take(vec.begin(), 0);
        REQUIRE(take.size() == 0);
        REQUIRE(lz::empty(take));
        REQUIRE_FALSE(lz::has_one(take));
        REQUIRE_FALSE(lz::has_many(take));
    }

    SUBCASE("One element iterator") {
        std::vector<int> vec = { 1 };
        auto take = lz::take(vec.begin(), 1);
        REQUIRE(take.size() == 1);
        REQUIRE_FALSE(lz::empty(take));
        REQUIRE(lz::has_one(take));
        REQUIRE_FALSE(lz::has_many(take));
    }
}

TEST_CASE("Take with bidirectional iterators") {
    std::list<int> lst = { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto take = lz::take(lst, 6);
    REQUIRE(take.size() == 6);
    auto expected = { 1, 2, 3, 4, 5, 6 };
    REQUIRE(lz::equal(take, expected));
    REQUIRE(lz::equal(take | lz::reverse, expected | lz::reverse));
}

TEST_CASE("Take to containers") {
    constexpr std::size_t size = 8;
    std::array<int, size> array = { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto take = lz::take(array, 4);
    REQUIRE(take.size() == 4);

    SUBCASE("To array") {
        auto arr = take | lz::to<std::array<int, 4>>();
        REQUIRE(lz::equal(arr, take));
    }

    SUBCASE("To vector") {
        auto vec = take | lz::to<std::vector>(std::allocator<int>());
        REQUIRE(lz::equal(vec, take));
    }

    SUBCASE("To other container using to<>()") {
        auto lst = take | lz::to<std::list<int>>(std::allocator<int>());
        REQUIRE(lz::equal(lst, take));
    }

    SUBCASE("To map") {
        auto map_obj = take | lz::map([](int i) { return std::make_pair(i, i); });
        auto map = lz::to<std::map<int, int>>(map_obj);
        REQUIRE(map.size() == 4);
        std::map<int, int> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
        REQUIRE(map == expected);
    }

    SUBCASE("To unordered map") {
        auto map = take | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        REQUIRE(map.size() == 4);
        std::unordered_map<int, int> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
        REQUIRE(map == expected);
    }
}

TEST_CASE("Drop & slice") {
    SUBCASE("Drop iterator") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto drop = vec | lz::drop(4);
        REQUIRE(drop.size() == 4);
        std::vector<int> expected = { 5, 6, 7, 8 };
        REQUIRE(lz::equal(drop, expected));
    }

    SUBCASE("Empty drop") {
        std::vector<int> vec;
        auto drop = vec | lz::drop(3);
        REQUIRE(drop.size() == 0);
        REQUIRE(lz::empty(drop));
        REQUIRE(!lz::has_one(drop));
        REQUIRE(!lz::has_many(drop));
    }

    SUBCASE("Drop where n is larger than size") {
        std::vector<int> vec = { 1, 2, 3 };
        auto drop = vec | lz::drop(10);
        REQUIRE(drop.size() == 0);
        REQUIRE(lz::empty(drop));
        REQUIRE(!lz::has_one(drop));
        REQUIRE(!lz::has_many(drop));
    }

    SUBCASE("Drop where n is larger than size / 2, sized & random access") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto drop = vec | lz::drop(6);
        REQUIRE(drop.size() == 2);
        std::vector<int> expected = { 7, 8 };
        REQUIRE(lz::equal(drop, expected));
    }

    SUBCASE("Drop where n is larger than size / 2, sized & bidirectional") {
        std::list<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto drop = vec | lz::drop(6);
        REQUIRE(drop.size() == 2);
        std::vector<int> expected = { 7, 8 };
        REQUIRE(lz::equal(drop, expected));
    }

    SUBCASE("Drop where n is larger than size / 2, not sized & forward") {
        auto cstr = lz::c_string("Hello, world!");
        auto drop = cstr | lz::drop(7);
        std::vector<char> expected = { 'w', 'o', 'r', 'l', 'd', '!' };
        REQUIRE(lz::equal(drop, expected));
    }

    SUBCASE("Slice iterable") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto slice = vec | lz::slice(2, 6);
        REQUIRE(slice.size() == 4);
        std::vector<int> expected = { 3, 4, 5, 6 };
        auto result = lz::to<std::vector>(slice, std::allocator<int>());
        REQUIRE(lz::equal(slice, expected));
    }
}

TEST_CASE("Take with iterator only") {
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto take = lz::take(vec.begin(), 4);
    REQUIRE(take.size() == 4);

    SUBCASE("Operator++") {
        auto expected = { 1, 2, 3, 4 };
        REQUIRE(lz::equal(take, expected));
    }

    SUBCASE("Operator--") {
        auto expected = { 4, 3, 2, 1 };
        REQUIRE(lz::equal(take | lz::reverse, expected));
    }

    SUBCASE("Operator+") {
        auto expected = { 1, 2, 3, 4 };
        test_procs::test_operator_plus(take, expected);
    }

    SUBCASE("Operator-") {
        test_procs::test_operator_minus(take);
    }
}
