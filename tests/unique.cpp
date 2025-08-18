#include <doctest/doctest.h>
#include <pch.hpp>
#include <Lz/unique.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <Lz/reverse.hpp>
#include <Lz/map.hpp>

TEST_CASE("Unique using sentinels") {
    auto str = lz::c_string("aabbcccddefgghhj");
    lz::unique_iterable<decltype(str)> unique = lz::unique(str);
    static_assert(!std::is_same<decltype(unique.begin()), decltype(unique.end())>::value, "Should be sentinel");
    auto expected = lz::c_string("abcdefghj");
    REQUIRE(lz::equal(unique, expected));

    SUBCASE("Operator=") {
        auto it = unique.begin();
        REQUIRE(it == unique.begin());
        it = unique.end();
        REQUIRE(it == unique.end());
    }
}

TEST_CASE("Unique changing and creating elements") {
    std::array<int, 4> arr = { 3, 2, 3, 1 };
    std::sort(arr.begin(), arr.end());
    auto unique = arr | lz::unique;
    auto beg = unique.begin();
    constexpr std::size_t size = 3;

    REQUIRE(*beg == 1);
    REQUIRE(static_cast<std::size_t>(std::distance(beg, unique.end())) == size);

    SUBCASE("Should be unique") {
        std::array<int, size> expected = { 1, 2, 3 };
        REQUIRE(expected == (unique | lz::to<std::array<int, size>>()));
    }

    SUBCASE("Should be unique too, using >") {
        std::array<int, size> expected = { 3, 2, 1 };
        auto unique_greater = expected | lz::unique(std::greater<int>());
        REQUIRE(expected == (unique_greater | lz::to<std::array<int, size>>()));
        REQUIRE(std::is_sorted(expected.begin(), expected.end(), std::greater<int>()));
    }
}

TEST_CASE("Unique binary operations") {
    std::array<int, 5> arr = { 1, 2, 2, 3, 3 };
    auto expected = { 1, 2, 3 };

    SUBCASE("Operator++") {
        REQUIRE(lz::equal(lz::unique(arr), expected));
    }

    SUBCASE("Operator--") {
        REQUIRE(lz::equal(arr | lz::unique | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("Operator==, operator!=") {
        auto unique = lz::unique(arr);
        auto beg = unique.begin();
        REQUIRE(beg != unique.end());
        beg = unique.end();
        REQUIRE(beg == unique.end());
    }
}

TEST_CASE("Empty or one element unique") {
    SUBCASE("Empty") {
        std::vector<int> vec;
        auto unique = lz::unique(vec);
        REQUIRE(lz::empty(unique));
        REQUIRE_FALSE(lz::has_many(unique));
        REQUIRE_FALSE(lz::has_one(unique));
    }

    SUBCASE("One element") {
        std::vector<int> vec = { 1 };
        auto unique = lz::unique(vec);
        REQUIRE(lz::has_one(unique));
        REQUIRE_FALSE(lz::has_many(unique));
        REQUIRE_FALSE(lz::empty(unique));
    }
}

TEST_CASE("Unique to container") {
    std::array<int, 4> arr = { 3, 2, 3, 1 };
    std::sort(arr.begin(), arr.end());
    constexpr std::size_t size = 3;
    auto unique = lz::unique(arr);

    SUBCASE("To array") {
        auto unique_array = unique | lz::to<std::array<int, size>>();
        std::array<int, size> expected = { 1, 2, 3 };
        REQUIRE(unique_array == expected);
    }

    SUBCASE("To vector") {
        auto unique_vec = unique | lz::to<std::vector>();
        std::vector<int> expected = { 1, 2, 3 };
        REQUIRE(unique_vec == expected);
    }

    SUBCASE("To other container using to<>()") {
        auto unique_list = unique | lz::to<std::list<int>>();
        std::list<int> expected = { 1, 2, 3 };
        REQUIRE(unique_list == expected);
    }

    SUBCASE("To map") {
        auto actual = unique | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();

        std::map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2),
            std::make_pair(3, 3),
        };

        REQUIRE(expected == actual);
    }

    SUBCASE("To unordered map") {
        auto actual = unique | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();

        std::unordered_map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2),
            std::make_pair(3, 3),
        };

        REQUIRE(expected == actual);
    }
}
