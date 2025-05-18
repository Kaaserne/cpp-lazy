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
        auto looper = lz::loop(vec, 0);
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
        auto test_looper = [](const decltype(lz::loop(vec, 0))& l, std::vector<int> expected) {
            auto begin = l.begin();
            auto end = l.end();
            REQUIRE(begin + 0 == begin);
            REQUIRE(end + 0 == end);

            for (std::size_t i = 0; i < lz::size(l) - 1; ++i) {
                INFO("With i = " << i);
                REQUIRE(*(begin + static_cast<std::ptrdiff_t>(i)) == *(expected.begin() + static_cast<std::ptrdiff_t>(i)));
            }
            REQUIRE(begin + static_cast<std::ptrdiff_t>(lz::size(l)) == l.end());

            REQUIRE(end - 0 == end);
            for (std::size_t i = 1; i <= lz::size(l); ++i) {
                INFO("With i = " << i);
                REQUIRE(*(end - static_cast<std::ptrdiff_t>(i)) == *(expected.end() - static_cast<std::ptrdiff_t>(i)));
            }
            REQUIRE(end - static_cast<std::ptrdiff_t>(lz::size(l)) == l.begin());

            std::advance(begin, static_cast<std::ptrdiff_t>(lz::size(l)));
            std::advance(end, -static_cast<std::ptrdiff_t>(lz::size(l)));

            REQUIRE(begin + 0 == begin);
            REQUIRE(end + 0 == end);

            for (std::size_t i = 0; i < lz::size(l) - 1; ++i) {
                INFO("With i = " << i);
                REQUIRE(*(end + static_cast<std::ptrdiff_t>(i)) == *(expected.begin() + static_cast<std::ptrdiff_t>(i)));
            }
            REQUIRE(end + static_cast<std::ptrdiff_t>(lz::size(l)) == l.end());
            for (std::size_t i = 1; i <= lz::size(l); ++i) {
                INFO("With i = " << i);
                REQUIRE(*(begin - static_cast<std::ptrdiff_t>(i)) == *(expected.end() - static_cast<std::ptrdiff_t>(i)));
            }
            REQUIRE(begin - static_cast<std::ptrdiff_t>(lz::size(l)) == l.begin());
        };

        auto looper = lz::loop(vec, 2);
        std::vector<int> expected = { 1, 2, 3, 4, 1, 2, 3, 4 };
        INFO("lz::loop(vec, 2)");
        test_looper(looper, std::move(expected));

        looper = lz::loop(vec, 1);
        expected = { 1, 2, 3, 4 };
        INFO("lz::loop(vec, 1)");
        test_looper(looper, std::move(expected));
    }

    
    SECTION("Operator-") {
        auto test_looper = [](const decltype(lz::loop(vec, 2))& l) {
            auto begin = l.begin();
            auto end = l.end();

            for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(lz::size(l)); ++i) {
                INFO("With i = " << i);
                CHECK((end - i) - begin == static_cast<std::ptrdiff_t>(lz::size(l)) - i);
                CHECK(end - (begin + i) == static_cast<std::ptrdiff_t>(lz::size(l)) - i);
                CHECK((begin + i) - end == -static_cast<std::ptrdiff_t>(lz::size(l)) - i);
                CHECK(begin - (end - i) == -static_cast<std::ptrdiff_t>(lz::size(l)) - i);
            }
            for (std::size_t i = 0; i < lz::size(l); ++i) {
                INFO("With i = " << i);
                CHECK((end - static_cast<std::ptrdiff_t>(i)) - (begin + static_cast<std::ptrdiff_t>(i)) ==
                      static_cast<std::ptrdiff_t>(lz::size(l)) - 2 * static_cast<std::ptrdiff_t>(i));
                CHECK((begin + static_cast<std::ptrdiff_t>(i)) - (end - static_cast<std::ptrdiff_t>(i)) ==
                      -(static_cast<std::ptrdiff_t>(lz::size(l)) - 2 * static_cast<std::ptrdiff_t>(i)));
            }
        };
        auto looper = lz::loop(vec, 2);
        INFO("lz::loop(vec, 2)");
        test_looper(looper);

        looper = lz::loop(vec, 1);
        INFO("lz::loop(vec, 1)");
        test_looper(looper);

        looper = lz::loop(vec, 3);
        INFO("lz::loop(vec, 3)");
        test_looper(looper);
    }
}
