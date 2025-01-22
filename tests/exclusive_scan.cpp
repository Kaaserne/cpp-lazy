#include <Lz/exclusive_scan.hpp>
#include <Lz/generate.hpp>
#include <catch2/catch.hpp>
#include <iostream>
#include <list>
#include <numeric>

TEST_CASE("Exclusive scan with sentinels") {
    int i = 0;
    auto generator = lz::generate([&i]() { return i++; }, 5);
    auto scan = lz::exclusive_scan(generator, 0);
    auto expected = { 0, 0, 1, 3, 6 };
    REQUIRE(lz::equal(scan, expected));
}

TEST_CASE("exclusive_scan basic functionality", "[exclusive_scan][Basic functionality]") {
    int arr[] = { 3, 1, 4, 1, 5, 9, 2, 6 };
    auto scan = lz::exclusive_scan(arr);
    static_assert(!std::is_same<decltype(scan.begin()), decltype(scan.end())>::value, "Iterators should not be the same type");

    REQUIRE(*scan.begin() == 0);
    REQUIRE(lz::distance(std::begin(scan), std::end(scan)) == std::distance(std::begin(arr), std::end(arr)));
    constexpr auto is_same = std::is_same<int&, decltype(*scan.begin())>::value;
    REQUIRE(is_same);
}

TEST_CASE("Empty or one element exclusive scan") {
    SECTION("Empty") {
        std::vector<int> empty;
        auto scan = lz::exclusive_scan(empty);
        REQUIRE(lz::empty(scan));
        REQUIRE(!lz::has_one(scan));
        REQUIRE(!lz::has_many(scan));
    }

    SECTION("One element") {
        std::vector<int> one_element = { 1 };
        auto scan = lz::exclusive_scan(one_element);
        REQUIRE(!lz::empty(scan));
        REQUIRE(lz::has_one(scan));
        REQUIRE(!lz::has_many(scan));
        REQUIRE(*scan.begin() == 0);
    }
}

TEST_CASE("Exclusive scan binary operations", "[exclusive_scan][Binary ops]") {
    int arr[] = { 3, 1, 4, 1, 5, 9, 2 };
    auto scan = lz::exclusive_scan(arr, 0);

    REQUIRE(*scan.begin() == 0);

    SECTION("Operator++") {
        auto begin = scan.begin();
        ++begin;
        REQUIRE(*begin == 0 + 3);
        ++begin;
        REQUIRE(*begin == 0 + 3 + 1);
        ++begin;
        REQUIRE(*begin == 0 + 3 + 1 + 4);
    }

    SECTION("Operator== & operator!=") {
        REQUIRE(scan.begin() != scan.end());
        auto begin = scan.begin();
        while (begin != scan.end()) {
            ++begin;
        }
        REQUIRE(begin == scan.end());
        begin = scan.begin();
        ++begin;
        REQUIRE(begin != scan.begin());
        REQUIRE(begin != scan.end());
    }
}

TEST_CASE("Exclusive scan to container", "[exclusive_scan][To container]") {
    int to_scan[] = { 2, 5, 6, 4, 87, 8, 45, 7 };
    auto scanner = lz::exclusive_scan(to_scan, 0);

    SECTION("To array") {
        std::array<int, 8> expected = { 0, 2, 7, 13, 17, 104, 112, 157 };
        auto actual = scanner.to<std::array<int, expected.size()>>();
        REQUIRE(actual == expected);
    }

    SECTION("To vector") {
        std::vector<int> expected = { 0, 2, 7, 13, 17, 104, 112, 157 };
        auto actual = scanner.to_vector();
        REQUIRE(expected == actual);
    }

    SECTION("To other container using to<>()") {
        std::list<int> expected = { 0, 2, 7, 13, 17, 104, 112, 157 };
        auto actual = scanner.to<std::list<int>>();
        REQUIRE(expected == actual);
    }

    SECTION("To map") {
        std::map<int, int> expected = { { 0, 0 },   { 4, 2 },     { 14, 7 },    { 26, 13 },
                                        { 34, 17 }, { 208, 104 }, { 224, 112 }, { 314, 157 } };
        auto actual = scanner.to_map([](int i) { return std::make_pair(i + i, i); });
        REQUIRE(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> expected = { { 0, 0 },   { 4, 2 },     { 14, 7 },    { 26, 13 },
                                                  { 34, 17 }, { 208, 104 }, { 224, 112 }, { 314, 157 } };
        auto actual = scanner.to_unordered_map([](int i) { return std::make_pair(i + i, i); });
        REQUIRE(expected == actual);
    }
}