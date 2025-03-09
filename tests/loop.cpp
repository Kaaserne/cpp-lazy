#include <Lz/loop.hpp>
#include <Lz/c_string.hpp>
#include <catch2/catch.hpp>


TEST_CASE("loop_iterable tests with sentinels") {
    auto cstr = lz::c_string("Hello");
    auto looper = lz::loop(cstr);
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
    std::array<int, 4> arr = { 1, 2, 3, 4 };

    SECTION("Empty") {
        auto looper = lz::loop(arr, 0);
        static_assert(std::is_same<decltype(looper.begin()), decltype(looper.end())>::value, "Should not be sentinel");
        REQUIRE(looper.size() == 0);
        REQUIRE(lz::empty(looper));

        std::vector<int> vec;
        auto looper2 = lz::loop(vec, 0);
        REQUIRE(looper2.size() == 0);
        REQUIRE(lz::empty(looper2));
    }

    SECTION("Size") {
        auto looper = lz::loop(arr, 2);
        REQUIRE(looper.size() == 8);
        REQUIRE(looper.size() == static_cast<std::size_t>(std::distance(looper.begin(), looper.end())));
    }

    SECTION("To vector") {
        auto looper = lz::loop(arr, 2);
        std::vector<int> expected = { 1, 2, 3, 4, 1, 2, 3, 4 };
        REQUIRE((looper | lz::to<std::vector>()) == expected);
    }

    SECTION("Distance positive") {
        auto looper = lz::loop(arr, 2);
        auto begin = looper.begin();

        for (std::size_t i = 0, start = 8; i < looper.size(); ++i, --start, ++begin) {
            REQUIRE(looper.end() - begin == static_cast<std::ptrdiff_t>(start));
        }

        begin = looper.begin();
        auto end = looper.end();

        for (std::size_t i = 0, start = 8; i < looper.size(); ++i, --start, --end) {
            REQUIRE(end - begin == static_cast<std::ptrdiff_t>(start));
        }
    }

    SECTION("Distance negative") {
        auto looper = lz::loop(arr, 3);
        auto begin = looper.begin();
        for (std::ptrdiff_t i = -static_cast<std::ptrdiff_t>(looper.size()); i < 0; ++i, ++begin) {
            REQUIRE(std::distance(looper.end(), begin) == i);
        }
    }

    // TODO Create more tests for these operators in all tests
    SECTION("Operator+") {
        auto looper = lz::loop(arr, 2);
        auto begin = looper.begin();
        REQUIRE(*(begin + 1) == 2);
        REQUIRE(*(begin + 2) == 3);
        REQUIRE(*(begin + 3) == 4);
        REQUIRE(*(begin + 4) == 1);
        REQUIRE(*(begin + 5) == 2);
        REQUIRE(*(begin + 6) == 3);
        REQUIRE(*(begin + 7) == 4);
        REQUIRE(begin + 8 == looper.end());

        auto end = looper.end();
        REQUIRE(*(end - 1) == 4);
        REQUIRE(*(end - 2) == 3);
        REQUIRE(*(end - 3) == 2);
        REQUIRE(*(end - 4) == 1);
        REQUIRE(*(end - 5) == 4);
        REQUIRE(*(end - 6) == 3);
        REQUIRE(*(end - 7) == 2);
        REQUIRE(*(end - 8) == 1);
        REQUIRE(looper.begin() == end - 8);
    }
}
