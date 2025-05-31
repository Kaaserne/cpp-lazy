#include <Lz/c_string.hpp>
#include <Lz/loop.hpp>
#include <catch2/catch.hpp>
#include <test_procs.hpp>

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

    SECTION("Operator=") {
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
        REQUIRE(begin != looper_2.end());
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

    SECTION("Always true") {
        REQUIRE(looper.begin() != looper.end());
        REQUIRE(looper.begin() != looper.begin());
    }

    SECTION("Always false") {
        REQUIRE(!(looper.begin() == looper.end()));
        REQUIRE(!(looper.begin() == looper.begin()));
    }
}

TEST_CASE("Loop with non while true argument") {
    std::vector<int> vec = { 1, 2, 3, 4 };

    SECTION("Empty") {
        lz::loop_iterable<std::vector<int>> looper = lz::loop(vec, 0);
        static_assert(std::is_same<decltype(looper.begin()), decltype(looper.end())>::value, "Should not be sentinel");
        REQUIRE(looper.size() == 0);
        REQUIRE(lz::empty(looper));

        std::vector<int> v;
        auto looper2 = lz::loop(v, 0);
        REQUIRE(looper2.size() == 0);
        REQUIRE(lz::empty(looper2));
    }

    SECTION("Size") {
        auto looper = lz::loop(vec, 2);
        REQUIRE(looper.size() == 8);
        REQUIRE(looper.size() == static_cast<std::size_t>(std::distance(looper.begin(), looper.end())));
    }

    SECTION("To vector") {
        auto looper = lz::loop(vec, 2);
        std::vector<int> expected = { 1, 2, 3, 4, 1, 2, 3, 4 };
        REQUIRE((looper | lz::to<std::vector>()) == expected);
    }

    SECTION("Operator+") {
        auto looper = lz::loop(vec, 2);
        std::vector<int> expected = { 1, 2, 3, 4, 1, 2, 3, 4 };
        test_procs::test_operator_plus(looper, expected);

        looper = lz::loop(vec, 1);
        expected = { 1, 2, 3, 4 };
        test_procs::test_operator_plus(looper, expected);
    }

    
    SECTION("Operator-") {
        auto looper = lz::loop(vec, 2);
        test_procs::test_operator_minus(looper);

        looper = lz::loop(vec, 1);
        test_procs::test_operator_minus(looper);

        looper = lz::loop(vec, 3);
        test_procs::test_operator_minus(looper);
    }
}
