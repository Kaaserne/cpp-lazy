#include <Lz/c_string.hpp>
#include <Lz/map.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <Lz/rotate.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

TEST_CASE("rotate_iterable with sentinels") {
    auto c_str = lz::c_string("Hello, World!");
    lz::rotate_iterable<decltype(c_str)> rotated = lz::rotate(c_str, 7);
    static_assert(!std::is_same<decltype(rotated.begin()), decltype(rotated.end())>::value, "Should be sentinel");
    REQUIRE((rotated | lz::to<std::string>()) == "World!Hello, ");
}

TEST_CASE("Operator=(default_sentinel_t)") {
    SUBCASE("forward") {
        std::forward_list<int> lst{ 1, 2, 3, 4, 5 };
        auto rotated2 = lz::rotate(lst, 2);
        auto common = make_sentinel_assign_op_tester(rotated2);
        std::vector<int> expected = { 3, 4, 5, 1, 2 };
        REQUIRE(lz::equal(common, expected));
    }

    SUBCASE("bidirectional") {
        std::list<int> lst{ 1, 2, 3, 4, 5 };
        auto rotated2 = lz::rotate(make_sized_bidi_sentinelled(lst), 2);
        auto common = make_sentinel_assign_op_tester(rotated2);
        std::vector<int> expected = { 3, 4, 5, 1, 2 };
        REQUIRE(lz::equal(common | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("random access") {
        auto rotated2 = lz::rotate(lz::repeat(5, 5), 2);
        auto common = make_sentinel_assign_op_tester(rotated2);
        std::vector<int> expected = { 5, 5, 5, 5, 5 };
        test_procs::test_operator_minus(common);
        test_procs::test_operator_plus(common, expected);
    }
}

TEST_CASE("rotate_iterable basic functionality") {
    std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
    auto rotate = lz::rotate(arr, 2);
    REQUIRE(rotate.size() == arr.size());

    SUBCASE("With bidirectional iterator") {
        std::list<int> lst = { 1, 2, 3, 4, 5, 6 };
        auto rotator = lz::rotate(lst, 2);
        REQUIRE(std::distance(rotator.begin(), rotator.end()) == static_cast<std::ptrdiff_t>(lst.size()));
        REQUIRE(rotator.size() == lst.size());
    }

    SUBCASE("Rotate where n is larger than size / 2 (random access & sized)") {
        rotate = lz::rotate(arr, 3);
        REQUIRE(rotate.size() == arr.size());
        REQUIRE((rotate | lz::to<std::vector>()) == std::vector<int>{ 4, 5, 1, 2, 3 });
    }

    SUBCASE("Rotate where n is larger than size / 2 (forward, not sized)") {
        auto str = lz::c_string("Hello, World!");
        auto rotator = lz::rotate(str, 7);
        auto expected = lz::c_string("World!Hello, ");
        REQUIRE(lz::equal(rotator, expected));
    }

    SUBCASE("Rotate where n == size") {
        rotate = lz::rotate(arr, lz::size(arr));
        REQUIRE(rotate.size() == 0);
        REQUIRE(lz::empty(rotate));
        REQUIRE(!lz::has_one(rotate));
        REQUIRE(!lz::has_many(rotate));
    }
}

TEST_CASE("rotate_iterable binary operations") {
    std::vector<int> vec = { 1, 2, 3, 4 };
    auto rotate = lz::rotate(vec, 3);

    SUBCASE("Operator++") {
        auto expected = { 4, 1, 2, 3 };
        REQUIRE(lz::equal(rotate, expected));
    }

    SUBCASE("Operator--") {
        auto expected = { 4, 1, 2, 3 };
        REQUIRE(lz::equal(rotate | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("Operator+") {
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

    SUBCASE("Operator-") {
        test_procs::test_operator_minus(rotate);

        std::vector<int> container = { 1, 2, 3, 4, 5 };
        rotate = lz::rotate(container, 3);
        test_procs::test_operator_minus(rotate);

        container = { 1, 2, 3, 4 };
        rotate = lz::rotate(container, 2);
        test_procs::test_operator_minus(rotate);
    }

    SUBCASE("Operator-(default_sentinel_t)") {
        auto r = lz::rotate(lz::repeat(1, 5), 2);
        test_procs::test_operator_minus(r);

        r = lz::rotate(lz::repeat(1, 0), 0);
        test_procs::test_operator_minus(r);

        r = lz::rotate(lz::repeat(1, 1), 1);
        test_procs::test_operator_minus(r);
    }

    SUBCASE("Operator+(default_sentinel_t)") {
        auto r = lz::rotate(lz::repeat(0, 5), 2);
        std::vector<int> expected = { 0, 0, 0, 0, 0 };
        test_procs::test_operator_plus(r, expected);

        r = lz::rotate(lz::repeat(0, 1), 1);
        expected = {};
        test_procs::test_operator_plus(r, expected);

        r = lz::rotate(lz::repeat(0, 0), 0);
        expected = {};
        test_procs::test_operator_plus(r, expected);
    }
}

TEST_CASE("Empty or one element rotate") {
    SUBCASE("Empty, increment 2") {
        std::vector<int> vec{};
        auto rotate = lz::rotate(vec, 2);
        REQUIRE(lz::empty(rotate));
        REQUIRE_FALSE(lz::has_many(rotate));
        REQUIRE_FALSE(lz::has_one(rotate));
    }

    SUBCASE("One element, increment 2") {
        std::vector<int> vec = { 1 };
        auto rotate = lz::rotate(vec, 2);
        REQUIRE(lz::empty(rotate));
        REQUIRE(!lz::has_many(rotate));
        REQUIRE(!lz::has_one(rotate));
    }

    SUBCASE("One element, increment 0") {
        std::vector<int> vec = { 1 };
        auto rotate = lz::rotate(vec, 0);
        REQUIRE_FALSE(lz::empty(rotate));
        REQUIRE_FALSE(lz::has_many(rotate));
        REQUIRE(lz::has_one(rotate));
    }

    SUBCASE("Empty, increment 2 with sentinel") {
        auto cstr = lz::c_string("");
        auto rotate = lz::rotate(cstr, 2);
        REQUIRE(lz::empty(rotate));
        REQUIRE_FALSE(lz::has_many(rotate));
        REQUIRE_FALSE(lz::has_one(rotate));
    }

    SUBCASE("One element, increment 0 with sentinel") {
        auto cstr = lz::c_string("a");
        auto rotate = lz::rotate(cstr, 0);
        REQUIRE_FALSE(lz::empty(rotate));
        REQUIRE_FALSE(lz::has_many(rotate));
        REQUIRE(lz::has_one(rotate));
    }

    SUBCASE("One element, increment 2") {
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

    SUBCASE("Reverse to container") {
        auto reversed = lz::reverse(rotator);
        REQUIRE((reversed | lz::to<std::array<int, size>>()) == std::array<int, size>{ 2, 1, 6, 5, 4, 3 });
    }

    SUBCASE("To array") {
        REQUIRE((rotator | lz::to<std::array<int, size>>()) == std::array<int, size>{ 3, 4, 5, 6, 1, 2 });
    }

    SUBCASE("To vector") {
        REQUIRE((rotator | lz::to<std::vector>()) == std::vector<int>{ 3, 4, 5, 6, 1, 2 });
    }

    SUBCASE("To other container using to<>()") {
        REQUIRE((rotator | lz::to<std::list<int>>()) == std::list<int>{ 3, 4, 5, 6, 1, 2 });
    }

    SUBCASE("To map") {
        auto map = rotator | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();
        REQUIRE(map == std::map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 1, 1 }, { 2, 2 } });
    }

    SUBCASE("To unordered map") {
        auto map = rotator | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        REQUIRE(map == std::unordered_map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 1, 1 }, { 2, 2 } });
    }
}
