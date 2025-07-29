#include <Lz/drop.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <Lz/slice.hpp>
#include <Lz/take.hpp>
#include <catch2/catch.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <cpp-lazy-ut-helper/repeat.hpp>
#include <list>
#include <map>
#include <test_procs.hpp>
#include <unordered_map>

TEST_CASE("Take with sentinels") {
    const char* str = "Hello, world!";
    auto c_string = lz::c_string(str);
    lz::take_iterable<decltype(c_string)> take = c_string | lz::take(5);
    static_assert(std::is_same<decltype(take.begin()), decltype(take.end())>::value, "Should be sentinel");
    auto expected = lz::c_string("Hello");
    REQUIRE(lz::equal(take, expected));
    auto vec = take | lz::to<std::vector<char>>();
    REQUIRE(lz::equal(vec, expected));

    SECTION("Operator=") {
        auto it = take.begin();
        REQUIRE(it == take.begin());
        it = take.end();
        REQUIRE(it == take.end());
    }

    SECTION("Operator= for with iterator") {
        auto t = lz::take(c_string.begin(), 5);
        auto it = t.begin();
        REQUIRE(it == t.begin());
        it = t.end();
        REQUIRE(it == t.end());
    }
}

TEST_CASE("Take changing and creating elements") {
    std::array<int, 10> array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take = lz::take(array, 4);
    REQUIRE(take.size() == 4);
    REQUIRE(*take.begin() == 1);
    REQUIRE(*std::next(take.begin()) == 2);
    REQUIRE(*std::prev(take.end()) == 4);
    *take.begin() = 0;
    REQUIRE(array[0] == 0);
}

TEST_CASE("Take binary operations where n is smaller than size") {
    std::array<int, 10> array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take = lz::take(array, 4);
    REQUIRE(take.size() == 4);

    SECTION("Operator++") {
        auto expected = { 1, 2, 3, 4 };
        REQUIRE(lz::equal(take, expected));
    }

    SECTION("Operator--") {
        auto expected = { 4, 3, 2, 1 };
        REQUIRE(lz::equal(take | lz::reverse, expected));
    }

    SECTION("Operator== & Operator!=") {
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

    SECTION("Operator+") {
        std::vector<int> expected = { 1, 2, 3, 4 };
        test_procs::test_operator_plus(take, expected);
    }

    SECTION("Operator-") {
        test_procs::test_operator_minus(take);
    }

    SECTION("Operator-(default_sentinel_t)") {
        auto r = lz::repeat(1, 5) | lz::take(3);
        test_procs::test_operator_minus(r);

        auto rep = lz::repeat(0, 5);
        auto r2 = lz::take(rep.begin(), 3);
        test_procs::test_operator_minus(r2);
    }
}

TEST_CASE("Take binary operations where n is larger than size") {
    std::array<int, 10> array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take = lz::take(array, 20);
    REQUIRE(take.size() == 10);

    SECTION("Operator++") {
        auto begin = take.begin();
        ++begin;
        REQUIRE(*begin == 2);
    }

    SECTION("Operator--") {
        auto end = take.end();
        --end;
        REQUIRE(*end == 10);
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(take.begin() != take.end());
        REQUIRE_FALSE(take.begin() == take.end());
        auto it = take.begin();
        it = take.end();
        REQUIRE(it == take.end());
    }

    SECTION("Operator+") {
        INFO("Operator+");
        std::vector<int> expected = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        test_procs::test_operator_plus(take, expected);
    }

    SECTION("Operator-") {
        INFO("Operator-");
        test_procs::test_operator_minus(take);
    }
}

TEST_CASE("Empty or one element take") {
    SECTION("Empty") {
        std::vector<int> vec;
        auto take = lz::take(vec, 0);
        REQUIRE(take.size() == 0);
        REQUIRE(lz::empty(take));
        REQUIRE_FALSE(lz::has_one(take));
        REQUIRE_FALSE(lz::has_many(take));
    }

    SECTION("One element") {
        std::vector<int> vec = { 1 };
        auto take = lz::take(vec, 1);
        REQUIRE(take.size() == 1);
        REQUIRE_FALSE(lz::empty(take));
        REQUIRE(lz::has_one(take));
        REQUIRE_FALSE(lz::has_many(take));
    }

    SECTION("Empty iterator") {
        std::vector<int> vec;
        auto take = lz::take(vec.begin(), 0);
        REQUIRE(take.size() == 0);
        REQUIRE(lz::empty(take));
        REQUIRE_FALSE(lz::has_one(take));
        REQUIRE_FALSE(lz::has_many(take));
    }

    SECTION("One element iterator") {
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

    SECTION("To array") {
        auto arr = take | lz::to<std::array<int, 4>>();
        REQUIRE(lz::equal(arr, take));
    }

    SECTION("To vector") {
        auto vec = take | lz::to<std::vector>(std::allocator<int>());
        REQUIRE(lz::equal(vec, take));
    }

    SECTION("To other container using to<>()") {
        auto lst = take | lz::to<std::list<int>>(std::allocator<int>());
        REQUIRE(lz::equal(lst, take));
    }

    SECTION("To map") {
        auto map_obj = take | lz::map([](int i) { return std::make_pair(i, i); });
        auto map = lz::to<std::map<int, int>>(map_obj);
        REQUIRE(map.size() == 4);
        std::map<int, int> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
        REQUIRE(map == expected);
    }

    SECTION("To unordered map") {
        auto map = take | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        REQUIRE(map.size() == 4);
        std::unordered_map<int, int> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
        REQUIRE(map == expected);
    }
}

TEST_CASE("Drop & slice") {
    SECTION("Drop iterator") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto drop = vec | lz::drop(4);
        REQUIRE(drop.size() == 4);
        std::vector<int> expected = { 5, 6, 7, 8 };
        REQUIRE(lz::equal(drop, expected));
    }

    SECTION("Empty drop") {
        std::vector<int> vec;
        auto drop = vec | lz::drop(3);
        REQUIRE(drop.size() == 0);
        REQUIRE(lz::empty(drop));
        REQUIRE(!lz::has_one(drop));
        REQUIRE(!lz::has_many(drop));
    }

    SECTION("Drop where n is larger than size") {
        std::vector<int> vec = { 1, 2, 3 };
        auto drop = vec | lz::drop(10);
        REQUIRE(drop.size() == 0);
        REQUIRE(lz::empty(drop));
        REQUIRE(!lz::has_one(drop));
        REQUIRE(!lz::has_many(drop));
    }

    SECTION("Drop where n is larger than size / 2, sized & random access") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto drop = vec | lz::drop(6);
        REQUIRE(drop.size() == 2);
        std::vector<int> expected = { 7, 8 };
        REQUIRE(lz::equal(drop, expected));
    }

    SECTION("Drop where n is larger than size / 2, sized & bidirectional") {
        std::list<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto drop = vec | lz::drop(6);
        REQUIRE(drop.size() == 2);
        std::vector<int> expected = { 7, 8 };
        REQUIRE(lz::equal(drop, expected));
    }

    SECTION("Drop where n is larger than size / 2, not sized & forward") {
        auto cstr = lz::c_string("Hello, world!");
        auto drop = cstr | lz::drop(7);
        std::vector<char> expected = { 'w', 'o', 'r', 'l', 'd', '!' };
        REQUIRE(lz::equal(drop, expected));
    }

    SECTION("Slice iterable") {
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

    SECTION("Operator++") {
        auto expected = { 1, 2, 3, 4 };
        REQUIRE(lz::equal(take, expected));
    }

    SECTION("Operator--") {
        auto expected = { 4, 3, 2, 1 };
        REQUIRE(lz::equal(take | lz::reverse, expected));
    }

    SECTION("Operator+") {
        auto expected = { 1, 2, 3, 4 };
        test_procs::test_operator_plus(take, expected);
    }

    SECTION("Operator-") {
        test_procs::test_operator_minus(take);
    }
}
