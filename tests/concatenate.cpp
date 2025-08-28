#include <Lz/algorithm.hpp>
#include <Lz/common.hpp>
#include <Lz/concatenate.hpp>
#include <Lz/map.hpp>
#include <Lz/range.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <Lz/string_view.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

TEST_CASE("Concatenate with sentinels") {
    const char* str = "hello, world!";
    auto cstr = lz::c_string(str);

    std::vector<char> vec = { 'h', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!' };
    lz::concatenate_iterable<decltype(cstr), decltype(vec)> concat = lz::concat(cstr, vec);
    static_assert(std::is_same<lz::default_sentinel_t, decltype(concat.end())>::value,
                  "Sentinel type should be default_sentinel_t");
    auto expected = { 'h', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!',
                      'h', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!' };
    REQUIRE(lz::equal(concat, expected));

    SUBCASE("operator=(default_sentinel_t)") {
        std::forward_list<int> a = { 1, 2 };
        std::forward_list<int> b = { 3, 4 };
        auto concatenated = lz::concat(a, b);

        auto common = lz::common(concatenated);

        auto expected2 = { 1, 2, 3, 4 };
        REQUIRE(lz::equal(common, expected2));
    }
}

TEST_CASE("Reference tests") {
    std::string a = "hello ";
    const std::string b = "world";

    auto concat = a | lz::concat(b);
    REQUIRE(concat.size() == a.size() + b.size());
    static_assert(std::is_same<decltype(concat.begin()), decltype(concat.end())>::value, "Should not be sentinel");

    auto range = lz::range(0);
    const std::vector<int> vec = { 1, 2, 3 };
    std::vector<int> vec2 = { 4, 5, 6 };
    auto concat2 = lz::concat(range, vec, vec2);
    using t = decltype(*concat2.begin());
    static_assert(std::is_same<int, t>::value, "Should be by value");

    auto concat3 = lz::concat(vec, vec2, range);
    using t2 = decltype(*concat3.begin());
    static_assert(std::is_same<int, t2>::value, "Should be by value");

    auto concat4 = lz::concat(vec2, vec, range);
    using t3 = decltype(*concat4.begin());
    static_assert(std::is_same<int, t3>::value, "Should be by value");

    volatile const int arr[] = { 0 };
    auto concat5 = lz::concat(range, arr);
    using t4 = decltype(*concat5.begin());
    static_assert(std::is_same<int, t4>::value, "Should be by value");

    int arr2[] = { 0 };
    auto concat6 = lz::concat(arr2, arr);
    using t5 = decltype(*concat6.begin());
    static_assert(std::is_same<const volatile int&, t5>::value, "Should be by const volatile reference");

    using t6 = decltype(*concat.begin());
    static_assert(std::is_same<const char&, t6>::value, "Should be const");
}

TEST_CASE("Empty or one element concatenate") {
    SUBCASE("Empty") {
        std::string a;
        std::string b;
        auto concat = lz::concat(a, b);
        REQUIRE(concat.size() == 0);
        REQUIRE(lz::empty(concat));
        REQUIRE_FALSE(lz::has_one(concat));
        REQUIRE_FALSE(lz::has_many(concat));
    }

    SUBCASE("One element 1") {
        std::string a = "h";
        std::string b;
        auto concat = lz::concat(a, b);
        REQUIRE(concat.size() == 1);
        REQUIRE_FALSE(lz::empty(concat));
        REQUIRE(lz::has_one(concat));
        REQUIRE_FALSE(lz::has_many(concat));
    }

    SUBCASE("One element 2") {
        std::string a;
        std::string b = "w";
        auto concat = lz::concat(a, b);
        REQUIRE(concat.size() == 1);
        REQUIRE_FALSE(lz::empty(concat));
        REQUIRE(lz::has_one(concat));
        REQUIRE_FALSE(lz::has_many(concat));
    }

    SUBCASE("One element both") {
        std::string a = "h";
        std::string b = "w";
        auto concat = lz::concat(a, b);
        REQUIRE(concat.size() == 2);
        REQUIRE_FALSE(lz::empty(concat));
        REQUIRE_FALSE(lz::has_one(concat));
        REQUIRE(lz::has_many(concat));
    }
}

TEST_CASE("Concat binary operations") {
    std::string a = "hello ", b = "world";
    auto concat = lz::concat(a, b);

    SUBCASE("Operator++") {
        lz::string_view expected = "hello world";
        REQUIRE(lz::equal(concat, expected));
    }

    SUBCASE("Operator--") {
        lz::string_view expected = "hello world";
        REQUIRE(lz::equal(lz::reverse(concat), lz::reverse(expected)));
    }

    SUBCASE("Operator== & operator!=") {
        auto it = concat.begin();
        REQUIRE(it == concat.begin());
        REQUIRE(it != concat.end());
        REQUIRE(concat.end() != it);
        REQUIRE(concat.begin() == it);
        it = concat.end();
        REQUIRE(it == concat.end());
        REQUIRE(it != concat.begin());
        REQUIRE(concat.end() == it);
        REQUIRE(concat.begin() != it);
    }

    SUBCASE("Operator+") {
        lz::string_view expected = "hello world";
        test_procs::test_operator_plus(concat, expected);
    }

    SUBCASE("Operator-") {
        test_procs::test_operator_minus(concat);
    }

    SUBCASE("Operator-(default_sentinel_t)") {
        auto rep1 = lz::repeat(42, 5);
        auto rep2 = lz::repeat(43, 5);
        auto concat_rep = lz::concat(rep1, rep2);
        test_procs::test_operator_minus(concat_rep);
    }

    SUBCASE("Operator+(default_sentinel_t)") {
        auto rep1 = lz::repeat(42, 5);
        auto rep2 = lz::repeat(43, 5);
        auto concat_rep = lz::concat(rep1, rep2);
        std::vector<int> expected = { 42, 42, 42, 42, 42, 43, 43, 43, 43, 43 };
        test_procs::test_operator_plus(concat_rep, expected);
    }
}

TEST_CASE("Concatenate to containers") {
    std::vector<int> v1 = { 1, 2, 3 };
    std::vector<int> v2 = { 4, 5, 6 };
    auto concat = lz::concat(v1, v2);

    SUBCASE("To array") {
        constexpr std::size_t size = 3 + 3;
        REQUIRE((concat | lz::to<std::array<int, size>>()) == std::array<int, size>{ 1, 2, 3, 4, 5, 6 });
    }

    SUBCASE("To vector") {
        REQUIRE((concat | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 4, 5, 6 });
    }

    SUBCASE("To other container using to<>()") {
        REQUIRE((concat | lz::to<std::list<int>>()) == std::list<int>{ 1, 2, 3, 4, 5, 6 });
    }

    SUBCASE("To map") {
        auto map = concat | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();
        std::map<int, int> expected = { std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3),
                                        std::make_pair(4, 4), std::make_pair(5, 5), std::make_pair(6, 6) };
        REQUIRE(map == expected);
    }

    SUBCASE("To unordered map") {
        auto map = concat | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        std::unordered_map<int, int> expected = { std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3),
                                                  std::make_pair(4, 4), std::make_pair(5, 5), std::make_pair(6, 6) };
        REQUIRE(map == expected);
    }
}
