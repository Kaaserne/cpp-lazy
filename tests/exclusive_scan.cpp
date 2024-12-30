#include <Lz/exclusive_scan.hpp>
#include <Lz/generate.hpp>
#include <catch2/catch.hpp>
#include <iostream>
#include <list>
#include <numeric>

TEST_CASE("Exclusive scan with sentinels") {
    auto generator = lz::generate([](int& i) { return i++; }, 5, 0);
    auto scan = lz::exclusive_scan(generator, 0);
    auto expected = { 0, 0, 1, 3, 6 };
    CHECK(lz::equal(scan, expected));
}

TEST_CASE("ExclusiveScan basic functionality", "[ExclusiveScan][Basic functionality]") {
    int arr[] = { 3, 1, 4, 1, 5, 9, 2, 6 };
    auto scan = lz::exclusive_scan(arr);
    static_assert(!std::is_same<decltype(scan.begin()), decltype(scan.end())>::value, "Iterators should not be the same type");

    CHECK(*scan.begin() == 0);
    CHECK(lz::distance(std::begin(scan), std::end(scan)) == std::distance(std::begin(arr), std::end(arr)));
    constexpr auto is_same = std::is_same<int&, decltype(*scan.begin())>::value;
    CHECK(is_same);
}

TEST_CASE("Exclusive scan binary operations", "[ExclusiveScan][Binary ops]") {
    int arr[] = { 3, 1, 4, 1, 5, 9, 2 };
    auto scan = lz::exclusive_scan(arr, 0);

    REQUIRE(*scan.begin() == 0);

    SECTION("Operator++") {
        auto begin = scan.begin();
        ++begin;
        CHECK(*begin == 0 + 3);
        ++begin;
        CHECK(*begin == 0 + 3 + 1);
        ++begin;
        CHECK(*begin == 0 + 3 + 1 + 4);
    }

    SECTION("Operator== & operator!=") {
        CHECK(scan.begin() != scan.end());
        auto begin = scan.begin();
        while (begin != scan.end()) {
            ++begin;
        }
        CHECK(begin == scan.end());
        begin = scan.begin();
        ++begin;
        CHECK(begin != scan.begin());
        CHECK(begin != scan.end());
    }
}

TEST_CASE("Exclusive scan to container", "[ExclusiveScan][To container]") {
    int to_scan[] = { 2, 5, 6, 4, 87, 8, 45, 7 };
    auto scanner = lz::exclusive_scan(to_scan, 0);

    SECTION("To array") {
        std::array<int, 8> expected = { 0, 2, 7, 13, 17, 104, 112, 157 };
        auto actual = scanner.to<std::array<int, expected.size()>>();
        CHECK(actual == expected);
    }

    SECTION("To vector") {
        std::vector<int> expected = { 0, 2, 7, 13, 17, 104, 112, 157 };
        auto actual = scanner.to_vector();
        CHECK(expected == actual);
    }

    SECTION("To other container using to<>()") {
        std::list<int> expected = { 0, 2, 7, 13, 17, 104, 112, 157 };
        auto actual = scanner.to<std::list<int>>();
        CHECK(expected == actual);
    }

    SECTION("To map") {
        std::map<int, int> expected = { { 0, 0 },   { 4, 2 },     { 14, 7 },    { 26, 13 },
                                        { 34, 17 }, { 208, 104 }, { 224, 112 }, { 314, 157 } };
        auto actual = scanner.to_map([](int i) { return std::make_pair(i + i, i); });
        CHECK(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> expected = { { 0, 0 },   { 4, 2 },     { 14, 7 },    { 26, 13 },
                                                  { 34, 17 }, { 208, 104 }, { 224, 112 }, { 314, 157 } };
        auto actual = scanner.to_unordered_map([](int i) { return std::make_pair(i + i, i); });
        CHECK(expected == actual);
    }
}