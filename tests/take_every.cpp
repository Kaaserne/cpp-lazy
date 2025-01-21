#include <Lz/take_every.hpp>
#include <Lz/c_string.hpp>
#include <array>
#include <catch2/catch.hpp>
#include <list>


TEST_CASE("take_every_iterable with sentinels") {
    auto cstr = lz::c_string("Hello");
    auto take_every = lz::take_every(cstr, 2);
    static_assert(!std::is_same<decltype(take_every.begin()), decltype(take_every.end())>::value, "Should be sentinel");
    auto expected = lz::c_string("Hlo");
    REQUIRE(lz::equal(take_every, expected));
}

TEST_CASE("take_every_iterable changing and creating elements", "[take_every_iterable][Basic functionality]") {
    constexpr std::size_t size = 4;
    std::array<int, size> array = { 1, 2, 3, 4 };
    auto take_every = lz::take_every(array, 2);
    auto iterator = take_every.begin();

    SECTION("take_every_iterable should be by reference") {
        *iterator = 0;
        REQUIRE(array[0] == 0);
    }

    SECTION("take_every_iterable should select every amount-th") {
        REQUIRE(*iterator == 1);
        ++iterator;
        REQUIRE(*iterator == 3);
        ++iterator;
        REQUIRE(iterator == take_every.end());
    }

    SECTION("take_every_iterable should select every amount-th with skip first") {
        take_every = lz::take_every(array, 2, array.begin() + 1);
        iterator = take_every.begin();

        REQUIRE(*iterator == 2);
        ++iterator;
        REQUIRE(*iterator == 4);
        ++iterator;
        REQUIRE(iterator == take_every.end());
    }
}

TEST_CASE("Empty or one element take every") {
    SECTION("Empty") {
        std::vector<int> vec;
        auto takeEvery = lz::take_every(vec, 2);
        REQUIRE(lz::empty(takeEvery));
        REQUIRE(!lz::has_many(takeEvery));
        REQUIRE(!lz::has_one(takeEvery));
    }

    SECTION("One element") {
        std::vector<int> vec = { 1 };
        auto takeEvery = lz::take_every(vec, 2);
        REQUIRE(!lz::empty(takeEvery));
        REQUIRE(!lz::has_many(takeEvery));
        REQUIRE(lz::has_one(takeEvery));
    }
}

TEST_CASE("take_every_iterable binary operations", "[take_every_iterable][Binary ops]") {
    constexpr std::size_t size = 4;
    std::array<int, size> array = { 1, 2, 3, 4 };
    auto take_every = lz::take_every(array, 3);
    auto iterator = take_every.begin();

    SECTION("Operator++") {
        REQUIRE(*iterator == 1);
        ++iterator;
        REQUIRE(*iterator == 4);
    }

    SECTION("Operator++ (bidirectional)") {
        std::list<int> lst = { 1, 2, 3, 4 };
        auto te = lz::take_every(lst, 3);
        auto iter = te.begin();
        REQUIRE(*iter == 1);
        ++iter;
        REQUIRE(*iter == 4);
        ++iter;
        REQUIRE(iter == te.end());
    }

    SECTION("Operator--") {
        auto end = take_every.end();
        REQUIRE(*--end == 4);
        ++iterator;
        --iterator;
        REQUIRE(*iterator == 1);
    }

    SECTION("Operator-- (bidirectional)") {
        std::list<int> lst = { 1, 2, 3, 4 };
        auto te = lz::take_every(lst, 3);
        auto iter = te.end();
        --iter;
        REQUIRE(*iter == 4);
        --iter;
        REQUIRE(*iter == 1);
        REQUIRE(iter == te.begin());
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(iterator != take_every.end());
        iterator = take_every.end();
        REQUIRE(iterator == take_every.end());
    }

    SECTION("Operator+(int), tests += as well") {
        std::array<int, 6> arr = { 1, 2, 3, 4, 5, 6 };
        auto take = lz::take_every(arr, 2);
        auto beg = take.begin();

        REQUIRE(*(beg + 2) == 5);
        REQUIRE(beg + 3 == take.end());
        ++beg;
        REQUIRE(*(beg + -1) == 1);

        auto end = take.end();
        REQUIRE(*(end + -1) == 6);
        REQUIRE(*(end + -2) == 4);
        REQUIRE(*(end + -3) == 2);
        REQUIRE((end + -4) == take.begin());
        --end;
        REQUIRE(*end == 6);
        --end;
        REQUIRE(*end == 4);

        REQUIRE(end + 2 == take.end());
    }

    SECTION("Operator-(int), tests -= as well") {
        REQUIRE(*(take_every.end() - 1) == 4);
        REQUIRE(*(take_every.end() - 2) == 1);
    }

    SECTION("Operator-(Iterator)") {
        REQUIRE(take_every.end() - iterator == 2);
        REQUIRE(take_every.end() - (iterator + 1) == 1);
    }

    SECTION("Operator[]()") {
        REQUIRE(iterator[0] == 1);
        REQUIRE(iterator[1] == 4);
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto distance = std::distance(take_every.begin(), take_every.end());

        REQUIRE(iterator < take_every.end());
        REQUIRE(iterator + distance - 1 > take_every.end() - distance);
        auto d = iterator + distance - 1;
        REQUIRE(d <= take_every.end());
        REQUIRE(iterator + distance - 1 <= take_every.end());
        REQUIRE(iterator + size - 1 >= take_every.end() - 1);
    }
}

TEST_CASE("take_every_iterable to containers", "[take_every_iterable][To container]") {
    constexpr std::size_t size = 4;
    std::array<int, size> array = { 1, 2, 3, 4 };
    constexpr std::size_t offset = 2;
    auto take_every = lz::take_every(array, offset);

    SECTION("To array") {
        std::array<int, static_cast<std::size_t>(size / offset)> actual = take_every.to<std::array<int, offset>>();
        REQUIRE(actual == std::array<int, offset>{ 1, 3 });
    }

    SECTION("To vector") {
        std::vector<int> actual = take_every.to_vector();
        REQUIRE(actual == std::vector<int>{ 1, 3 });
    }

    SECTION("To other container using to<>()") {
        std::list<int> actual = take_every.to<std::list<int>>();
        REQUIRE(actual == std::list<int>{ 1, 3 });
    }

    SECTION("To map") {
        std::map<int, int> actual = take_every.to_map([](const int i) { return std::make_pair(i, i); });
        std::map<int, int> expected = { std::make_pair(1, 1), std::make_pair(3, 3) };
        REQUIRE(actual == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> actual = take_every.to_unordered_map([](const int i) { return std::make_pair(i, i); });
        std::unordered_map<int, int> expected = { std::make_pair(1, 1), std::make_pair(3, 3) };
        REQUIRE(actual == expected);
    }
}
