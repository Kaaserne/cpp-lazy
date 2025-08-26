#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <Lz/common.hpp>
#include <doctest/doctest.h>
#include <pch.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <Lz/loop.hpp>
#include <Lz/reverse.hpp>
#include <Lz/repeat.hpp>

TEST_CASE("loop_iterable tests with sentinels") {
    auto cstr = lz::c_string("Hello");
    lz::loop_iterable_inf<decltype(cstr)> looper = lz::loop(cstr);
    static_assert(!std::is_same<decltype(looper.begin()), decltype(looper.end())>::value, "Should be sentinel");
    REQUIRE(*looper.begin() == 'H');
    auto looper_it = looper.begin();
    auto c_str_it = cstr.begin();
    for (std::size_t i = 0; i < 100; ++i) {
        REQUIRE(*looper_it == *c_str_it);
        ++looper_it;
        ++c_str_it;
        if (c_str_it == cstr.end()) {
            c_str_it = cstr.begin();
        }
    }

    SUBCASE("Operator=") {
        looper_it = looper.begin();
        // inf never stops
        REQUIRE(looper_it != looper.begin());
        looper_it = looper.end();
        // inf never stops
        REQUIRE(looper_it != looper.end());

        auto looper_2 = lz::loop(cstr, 3);
        auto begin = looper_2.begin();
        REQUIRE(begin != looper_2.end());
        begin = looper_2.end();
        REQUIRE(begin == looper_2.end());
    }
}

TEST_CASE("Empty loop iterable") {
    std::vector<int> vec;
    auto looper = lz::loop(vec);
    REQUIRE(lz::empty(looper));
}

TEST_CASE("Basic functionality loop") {
    std::vector<int> vec = { 1, 2, 3, 4 };
    auto looper = lz::loop(vec);
    static_assert(!std::is_same<decltype(looper.begin()), decltype(looper.end())>::value, "Should be sentinel");

    SUBCASE("Always true") {
        REQUIRE(looper.begin() != looper.end());
        REQUIRE(looper.begin() != looper.begin());
    }

    SUBCASE("Always false") {
        REQUIRE_FALSE(looper.begin() == looper.end());
        REQUIRE_FALSE(looper.begin() == looper.begin());
    }
}

TEST_CASE("Loop with non while true argument") {
    std::vector<int> vec = { 1, 2, 3, 4 };

    SUBCASE("Operator=(default_sentinel_t") {
        std::forward_list<int> lst = { 1, 2, 3 };
        auto looped = lz::loop(lst, 2);
        auto common = lz::common(looped);
        auto expected = { 1, 2, 3, 1, 2, 3 };
        REQUIRE(lz::equal(common, expected));
    }

    SUBCASE("Empty") {
        lz::loop_iterable<std::vector<int>> looper = lz::loop(vec, 0);
        static_assert(std::is_same<decltype(looper.begin()), decltype(looper.end())>::value, "Should not be sentinel");
        REQUIRE(looper.size() == 0);
        REQUIRE(lz::empty(looper));

        std::vector<int> v;
        auto looper2 = lz::loop(v, 0);
        REQUIRE(looper2.size() == 0);
        REQUIRE(lz::empty(looper2));
    }

    SUBCASE("Size") {
        auto looper = vec | lz::loop(2);
        CHECK(looper.size() == static_cast<std::size_t>(std::distance(looper.begin(), looper.end())));
        CHECK(looper.size() == 8);
        looper = lz::loop(vec, 3);
        CHECK(looper.size() == static_cast<std::size_t>(std::distance(looper.begin(), looper.end())));
        CHECK(looper.size() == 12);
    }

    SUBCASE("Operator+") {
        auto looper = lz::loop(vec, 2);
        std::vector<int> expected = { 1, 2, 3, 4, 1, 2, 3, 4 };
        test_procs::test_operator_plus(looper, expected);

        looper = lz::loop(vec, 3);
        expected = { 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4 };
        test_procs::test_operator_plus(looper, expected);

        looper = lz::loop(vec, 1);
        expected = { 1, 2, 3, 4 };
        test_procs::test_operator_plus(looper, expected);
    }

    SUBCASE("Operator-") {
        auto looper = lz::loop(vec, 2);
        test_procs::test_operator_minus(looper);

        looper = lz::loop(vec, 1);
        test_procs::test_operator_minus(looper);

        looper = lz::loop(vec, 3);
        test_procs::test_operator_minus(looper);
    }

    SUBCASE("Operator-(default_sentinel_t)") {
        auto l = lz::loop(lz::repeat(0, 5), 2);
        test_procs::test_operator_minus(l);

        l = lz::loop(lz::repeat(0, 5), 1);
        test_procs::test_operator_minus(l);

        l = lz::loop(lz::repeat(0, 5), 3);
        test_procs::test_operator_minus(l);
    }

    SUBCASE("Operator+(default_sentinel_t)") {
        auto l = lz::loop(lz::repeat(0, 5), 2);
        std::vector<int> expected = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        test_procs::test_operator_plus(l, expected);

        l = lz::loop(lz::repeat(0, 5), 1);
        expected = { 0, 0, 0, 0, 0 };
        test_procs::test_operator_plus(l, expected);

        l = lz::loop(lz::repeat(0, 5), 3);
        expected = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        test_procs::test_operator_plus(l, expected);
    }
}
