#include <Lz/generate.hpp>
#include <Lz/inclusive_scan.hpp>
#include <Lz/map.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <numeric>
#include <unordered_map>

TEST_CASE("Inclusive scan with sentinels") {
    int x = 1;
    auto generator = lz::generate([x]() { return x; }, 10);
    lz::inclusive_scan_iterable<decltype(generator)> scan = lz::inclusive_scan(generator);
    static_assert(!std::is_same<decltype(scan.begin()), decltype(scan.end())>::value, "Should be sentinel");
    auto begin = scan.begin();
    for (int i = 1; i < 10; ++i) {
        REQUIRE(*begin == i);
        ++begin;
    }

    SECTION("Operator=") {
        begin = scan.begin();
        REQUIRE(begin == scan.begin());
        begin = scan.end();
        REQUIRE(begin == scan.end());
    }
}

TEST_CASE("Empty or one element inclusive scan") {
    SECTION("Empty") {
        std::vector<int> empty;
        auto scan = lz::inclusive_scan(empty);
        REQUIRE(lz::empty(scan));
    }

    SECTION("One element") {
        std::vector<int> one_element = { 1 };
        auto scan = lz::inclusive_scan(one_element);
        REQUIRE(!lz::empty(scan));
        REQUIRE(lz::has_one(scan));
        REQUIRE(!lz::has_many(scan));
    }
}

TEST_CASE("Inclusive scan changing and creating elements") {
    int arr[32];
    std::iota(std::begin(arr), std::end(arr), 0);
    auto scan = arr | lz::inclusive_scan(0);
    REQUIRE(scan.size() == lz::size(arr));
    auto begin = scan.begin();

    std::ptrdiff_t sum = 0;
    for (std::ptrdiff_t i = 0; i < std::distance(std::begin(arr), std::end(arr)); ++i) {
        sum += i;
        REQUIRE(*begin == sum);
        ++begin;
    }

    REQUIRE(lz::distance(scan) == std::distance(std::begin(arr), std::end(arr)));

    constexpr static int expected[] = { 0,   1,   3,   6,   10,  15,  21,  28,  36,  45,  55,  66,  78,  91,  105, 120,
                                        136, 153, 171, 190, 210, 231, 253, 276, 300, 325, 351, 378, 406, 435, 465, 496 };
    REQUIRE(std::equal(std::begin(expected), std::end(expected), std::begin(scan)));
}

TEST_CASE("Inclusive scan splitter binary operations") {
    int arr[] = { 1, 2, 3, 4, 5 };
    auto scan = lz::inclusive_scan(arr);

    SECTION("Operator++") {
        auto expected = { 1, 3, 6, 10, 15 };
        REQUIRE(lz::equal(scan, expected));
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

TEST_CASE("Inclusive scan splitter to containers") {
    int to_scan[] = { 2, 5, 6, 4, 87, 8, 45, 7 };
    auto scanner = lz::inclusive_scan(to_scan);

    SECTION("To array") {
        std::array<int, 8> expected = { 2, 7, 13, 17, 104, 112, 157, 164 };
        auto actual = scanner | lz::to<std::array<int, expected.size()>>();
        REQUIRE(actual == expected);
    }

    SECTION("To vector") {
        std::vector<int> expected = { 2, 7, 13, 17, 104, 112, 157, 164 };
        auto actual = scanner | lz::to<std::vector>();
        REQUIRE(expected == actual);
    }

    SECTION("To other container using to<>()") {
        std::list<int> expected = { 2, 7, 13, 17, 104, 112, 157, 164 };
        auto actual = scanner | lz::to<std::list<int>>();
        REQUIRE(expected == actual);
    }

    SECTION("To map") {
        std::map<int, int> expected = { { 4, 2 },     { 14, 7 },    { 26, 13 },   { 34, 17 },
                                        { 208, 104 }, { 224, 112 }, { 314, 157 }, { 328, 164 } };
        auto actual = scanner | lz::map([](int i) { return std::make_pair(i + i, i); }) | lz::to<std::map<int, int>>();
        REQUIRE(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> expected = { { 4, 2 },     { 14, 7 },    { 26, 13 },   { 34, 17 },
                                                  { 208, 104 }, { 224, 112 }, { 314, 157 }, { 328, 164 } };
        auto actual = scanner | lz::map([](int i) { return std::make_pair(i + i, i); }) | lz::to<std::unordered_map<int, int>>();
        REQUIRE(expected == actual);
    }
}
