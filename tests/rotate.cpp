#include <Lz/c_string.hpp>
#include <Lz/filter.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <Lz/rotate.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("rotate_iterable with sentinels") {
    auto c_str = lz::c_string("Hello, World!");
    auto rotated = lz::rotate(c_str, 7);
    static_assert(!std::is_same<decltype(rotated.begin()), decltype(rotated.end())>::value, "Should be sentinel");
    REQUIRE((rotated | lz::to<std::string>()) == "World!Hello, ");
}

TEST_CASE("rotate_iterable basic functionality") {
    std::array<int, 5> vec = { 1, 2, 3, 4, 5 };
    auto rotate = lz::rotate(vec, 2);
    REQUIRE(rotate.size() == vec.size());

    SECTION("Should be correct length") {
        auto beg = rotate.begin();
        REQUIRE(std::distance(beg, rotate.end()) == static_cast<std::ptrdiff_t>(vec.size()));
        ++beg, ++beg;
        REQUIRE(std::distance(beg, rotate.end()) == 3);
        ++beg;
        REQUIRE(std::distance(beg, rotate.end()) == 2);
    }

    SECTION("With bidirectional iterator") {
        std::list<int> lst = { 1, 2, 3, 4, 5, 6 };
        auto rotator = lz::rotate(lst, 2);
        REQUIRE(std::distance(rotator.begin(), rotator.end()) == static_cast<std::ptrdiff_t>(lst.size()));
        REQUIRE(rotator.size() == lst.size());
    }
}

TEST_CASE("rotate_iterable binary operations") {
    std::vector<int> vec = { 1, 2, 3, 4 };
    auto rotate = lz::rotate(vec, 3);

    SECTION("Operator++") {
        auto begin = rotate.begin();
        auto end = rotate.end();

        REQUIRE(*begin == 4);
        REQUIRE(*end == 4);
        ++begin;
        REQUIRE(*begin == 1);
        ++end;
        REQUIRE(*end == 1);
    }

    SECTION("Operator--") {
        auto begin = rotate.begin();
        auto end = rotate.end();
        REQUIRE(begin != end);
        --end;
        REQUIRE(*end == 3);
        --end;
        REQUIRE(*end == 2);
        --end;
        REQUIRE(*end == 1);
        --end;
        REQUIRE(*end == 4);
        REQUIRE(begin == end);
    }

    SECTION("Operator== & Operator!=") {
        auto begin = rotate.begin();
        auto end = rotate.end();
        REQUIRE(begin != end);
        begin = end;
        REQUIRE(begin == end);
    }

    SECTION("Various ++ and -- operators with begin and end") {
        std::array<int, 5> container = { 1, 2, 3, 4, 5 };
        auto rotator = lz::rotate(container, 3);

        auto uneven_begin = rotator.begin();
        auto uneven_end = rotator.end();

        ++uneven_begin;
        REQUIRE(*uneven_begin == 5);
        --uneven_begin;
        REQUIRE(*uneven_begin == 4);
        ++uneven_begin;

        ++uneven_begin;
        REQUIRE(*uneven_begin == 1);

        --uneven_end;
        REQUIRE(*uneven_end == 3);

        --uneven_end;
        REQUIRE(*uneven_end == 2);

        --uneven_end;
        REQUIRE(*uneven_end == 1);

        --uneven_end;
        REQUIRE(*uneven_end == 5);
    }

    SECTION("Operator+") {
        auto do_test = [](const decltype(rotate)& rotator, std::vector<int> expected) {
            auto begin = rotator.begin();
            auto end = rotator.end();
            REQUIRE(begin + 0 == begin);
            REQUIRE(end + 0 == end);

            for (std::size_t i = 0; i < lz::size(rotator) - 1; ++i) {
                INFO("With i = " << i);
                REQUIRE(*(begin + i) == *(expected.begin() + i));
            }
            REQUIRE(begin + lz::size(rotator) == rotator.end());
            for (std::size_t i = 1; i <= lz::size(rotator); ++i) {
                INFO("With i = " << i);
                REQUIRE(*(end - i) == *(rotator.end() - i));
            }
            REQUIRE(end - lz::size(rotator) == rotator.begin());

            std::advance(begin, lz::size(rotator));
            std::advance(end, -static_cast<std::ptrdiff_t>(lz::size(rotator)));
            REQUIRE(begin + 0 == begin);
            REQUIRE(end + 0 == end);

            for (std::size_t i = 0; i < lz::size(rotator) - 1; ++i) {
                INFO("With i = " << i);
                REQUIRE(*(end + i) == *(rotator.begin() + i));
            }
            REQUIRE(end + lz::size(rotator) == rotator.end());
            for (std::size_t i = 1; i <= lz::size(rotator); ++i) {
                INFO("With i = " << i);
                REQUIRE(*(begin - i) == *(rotator.end() - i));
            }
            REQUIRE(begin - lz::size(rotator) == rotator.begin());
        };

        INFO("lz::rotate(container, 3)");
        std::vector<int> expected = { 4, 1, 2, 3 };
        do_test(rotate, std::move(expected));

        std::vector<int> container = { 1, 2, 3, 4, 5 };
        INFO("lz::rotate(container, 3)");
        auto rotator = lz::rotate(container, 3);
        expected = { 4, 5, 1, 2, 3 };
        do_test(rotator, std::move(expected));

        container = { 1, 2, 3, 4 };
        rotator = lz::rotate(container, 2);
        expected = { 3, 4, 1, 2 };
        INFO("lz::rotate(container, 2)");
        do_test(rotator, std::move(expected));
    }

    SECTION("Operator-") {
        using lz_iterable = decltype(rotate);
        auto test_iterable = [](const lz_iterable& iterable) {
            auto begin = iterable.begin();
            auto end = iterable.end();

            for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(lz::size(iterable)); ++i) {
                INFO("With i = " << i);
                REQUIRE((end - i) - begin == static_cast<std::ptrdiff_t>(lz::size(iterable) - i));
                REQUIRE(end - (begin + i) == static_cast<std::ptrdiff_t>(lz::size(iterable) - i));
                REQUIRE((begin + i) - end == -static_cast<std::ptrdiff_t>(lz::size(iterable) - i));
                REQUIRE(begin - (end - i) == -static_cast<std::ptrdiff_t>(lz::size(iterable) - i));
            }

            for (std::size_t i = 0; i < lz::size(iterable); ++i) {
                INFO("With i = " << i);
                REQUIRE((end - i) - (begin + i) == static_cast<std::ptrdiff_t>(lz::size(iterable) - 2 * i));
                REQUIRE((begin + i) - (end - i) == -static_cast<std::ptrdiff_t>(lz::size(iterable) - 2 * i));
            }
        };

        INFO("test_iterable(rotate)");
        test_iterable(rotate);

        std::vector<int> container = { 1, 2, 3, 4, 5 };
        auto rotator = lz::rotate(container, 3);
        INFO("lz::rotate(container, 3)");
        test_iterable(rotator);

        container = { 1, 2, 3, 4 };
        rotator = lz::rotate(container, 2);
        INFO("lz::rotate(container, 2)");
        test_iterable(rotator);
    }
}

TEST_CASE("Empty or one element rotate") {
    SECTION("Empty") {
        std::vector<int> vec{};
        auto rotate = lz::rotate(vec, 0);
        REQUIRE(lz::empty(rotate));
        REQUIRE(!lz::has_many(rotate));
        REQUIRE(!lz::has_one(rotate));
    }

    SECTION("One element") {
        std::vector<int> vec = { 1 };
        auto rotate = lz::rotate(vec, 0);
        REQUIRE(!lz::empty(rotate));
        REQUIRE(!lz::has_many(rotate));
        REQUIRE(lz::has_one(rotate));
    }

    SECTION("Empty with sentinel") {
        auto cstr = lz::c_string("");
        auto rotate = lz::rotate(cstr, 0);
        REQUIRE(lz::empty(rotate));
        REQUIRE(!lz::has_many(rotate));
        REQUIRE(!lz::has_one(rotate));
    }

    SECTION("One element with sentinel") {
        auto cstr = lz::c_string("a");
        auto rotate = lz::rotate(cstr, 0);
        REQUIRE(!lz::empty(rotate));
        REQUIRE(!lz::has_many(rotate));
        REQUIRE(lz::has_one(rotate));
    }
}

TEST_CASE("rotate_iterable to containers") {
    constexpr std::size_t size = 6;
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
    auto rotator = lz::rotate(vec, 2);

    SECTION("Reverse to container") {
        auto reversed = lz::reverse(rotator);
        REQUIRE((reversed | lz::to<std::array<int, size>>()) == std::array<int, size>{ 2, 1, 6, 5, 4, 3 });
    }

    SECTION("To array") {
        REQUIRE((rotator | lz::to<std::array<int, size>>()) == std::array<int, size>{ 3, 4, 5, 6, 1, 2 });
    }

    SECTION("To vector") {
        REQUIRE((rotator | lz::to<std::vector>()) == std::vector<int>{ 3, 4, 5, 6, 1, 2 });
    }

    SECTION("To other container using to<>()") {
        REQUIRE((rotator | lz::to<std::list<int>>()) == std::list<int>{ 3, 4, 5, 6, 1, 2 });
    }

    SECTION("To map") {
        auto map = rotator | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();
        REQUIRE(map == std::map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 1, 1 }, { 2, 2 } });
    }

    SECTION("To unordered map") {
        auto map = rotator | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        REQUIRE(map == std::unordered_map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 1, 1 }, { 2, 2 } });
    }
}
