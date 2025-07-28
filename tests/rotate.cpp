#include <Lz/filter.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <Lz/rotate.hpp>
#include <catch2/catch.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <cpp-lazy-ut-helper/repeat.hpp>
#include <list>
#include <map>
#include <test_procs.hpp>
#include <unordered_map>

TEST_CASE("rotate_iterable with sentinels") {
    auto c_str = lz::c_string("Hello, World!");
    lz::rotate_iterable<decltype(c_str)> rotated = lz::rotate(c_str, 7);
    static_assert(!std::is_same<decltype(rotated.begin()), decltype(rotated.end())>::value, "Should be sentinel");
    REQUIRE((rotated | lz::to<std::string>()) == "World!Hello, ");

    SECTION("Operator=") {
        auto begin = rotated.begin();
        REQUIRE(begin == rotated.begin());
        begin = rotated.end();
        REQUIRE(begin == rotated.end());
    }
}

TEST_CASE("rotate_iterable basic functionality") {
    std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
    auto rotate = lz::rotate(arr, 2);
    REQUIRE(rotate.size() == arr.size());

    SECTION("Should be correct length") {
        auto beg = rotate.begin();
        REQUIRE(std::distance(beg, rotate.end()) == static_cast<std::ptrdiff_t>(arr.size()));
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

    SECTION("Rotate where n is larger than size / 2 (random access & sized)") {
        rotate = lz::rotate(arr, 3);
        REQUIRE(rotate.size() == arr.size());
        REQUIRE((rotate | lz::to<std::vector>()) == std::vector<int>{ 4, 5, 1, 2, 3 });
    }

    SECTION("Rotate where n is larger than size / 2 (forward, not sized)") {
        auto str = lz::c_string("Hello, World!");
        auto rotator = lz::rotate(str, 7);
        auto expected = lz::c_string("World!Hello, ");
        REQUIRE(lz::equal(rotator, expected));
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
        std::vector<int> expected = { 4, 1, 2, 3 };
        test_procs::test_operator_plus(rotate, expected);

        std::vector<int> container = { 1, 2, 3, 4, 5 };
        rotate = lz::rotate(container, 3);
        expected = { 4, 5, 1, 2, 3 };
        test_procs::test_operator_plus(rotate, expected);

        container = { 1, 2, 3, 4 };
        rotate = lz::rotate(container, 2);
        expected = { 3, 4, 1, 2 };
        test_procs::test_operator_plus(rotate, expected);
    }

    SECTION("Operator-") {
        INFO("test_iterable(rotate)");
        test_procs::test_operator_minus(rotate);

        std::vector<int> container = { 1, 2, 3, 4, 5 };
        rotate = lz::rotate(container, 3);
        INFO("lz::rotate(container, 3)");
        test_procs::test_operator_minus(rotate);

        container = { 1, 2, 3, 4 };
        rotate = lz::rotate(container, 2);
        INFO("lz::rotate(container, 2)");
        test_procs::test_operator_minus(rotate);
    }

    SECTION("Operator-(default_sentinel_t)") {
        auto r = lz::repeat(1, 5);
        test_procs::test_operator_minus(r);

        r = lz::repeat(1, 0);
        test_procs::test_operator_minus(r);

        r = lz::repeat(1, 1);
        test_procs::test_operator_minus(r);
    }
}

TEST_CASE("Empty or one element rotate") {
    SECTION("Empty, increment 2") {
        std::vector<int> vec{};
        auto rotate = lz::rotate(vec, 2);
        REQUIRE(lz::empty(rotate));
        REQUIRE_FALSE(lz::has_many(rotate));
        REQUIRE_FALSE(lz::has_one(rotate));
    }

    SECTION("One element, increment 2") {
        std::vector<int> vec = { 1 };
        auto rotate = lz::rotate(vec, 2);
        REQUIRE(lz::empty(rotate));
        REQUIRE(!lz::has_many(rotate));
        REQUIRE(!lz::has_one(rotate));
    }

    SECTION("One element, increment 0") {
        std::vector<int> vec = { 1 };
        auto rotate = lz::rotate(vec, 0);
        REQUIRE_FALSE(lz::empty(rotate));
        REQUIRE_FALSE(lz::has_many(rotate));
        REQUIRE(lz::has_one(rotate));
    }

    SECTION("Empty, increment 2 with sentinel") {
        auto cstr = lz::c_string("");
        auto rotate = lz::rotate(cstr, 2);
        REQUIRE(lz::empty(rotate));
        REQUIRE_FALSE(lz::has_many(rotate));
        REQUIRE_FALSE(lz::has_one(rotate));
    }

    SECTION("One element, increment 0 with sentinel") {
        auto cstr = lz::c_string("a");
        auto rotate = lz::rotate(cstr, 0);
        REQUIRE_FALSE(lz::empty(rotate));
        REQUIRE_FALSE(lz::has_many(rotate));
        REQUIRE(lz::has_one(rotate));
    }

    SECTION("One element, increment 2") {
        auto cstr = lz::c_string("a");
        auto rotate = lz::rotate(cstr, 2);
        REQUIRE(lz::empty(rotate));
        REQUIRE(!lz::has_many(rotate));
        REQUIRE(!lz::has_one(rotate));
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
