#include <Lz/c_string.hpp>
#include <Lz/enumerate.hpp>
#include <Lz/take.hpp>
#include <Lz/map.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Enumerate with sentinels") {
    const char* str = "Hello";
    auto c_string = lz::c_string(str);
    auto enumerated = lz::enumerate(c_string);
    static_assert(!std::is_same<decltype(enumerated.begin()), decltype(enumerated.end())>::value,
                  "Begin and end should not be the same type");
    auto taken = lz::take(enumerated, 3);
    std::vector<std::pair<int, char>> expected = { { 0, 'H' }, { 1, 'e' }, { 2, 'l' } };

    using ref_taken = lz::ref_iterable_t<decltype(taken)>;
    using ref_expected = lz::ref_iterable_t<decltype(expected)>;
    REQUIRE(lz::equal(taken, expected, [](const ref_taken& a, const ref_expected& b) { return a.first == b.first && a.second == b.second; }));
}

TEST_CASE("Enumerate correct size()") {
    std::list<int> list = { 1, 2, 3, 4, 5 };
    auto enumerated = lz::enumerate(list);
    REQUIRE(enumerated.size() == 5);
    enumerated = lz::enumerate(list, 2);
    REQUIRE(enumerated.size() == 5);
    auto end = enumerated.end();
    --end;
    REQUIRE(end->first == 6);
    --end, --end, --end, --end;
    REQUIRE(end->first == 2);
    REQUIRE(end->second == 1);
}

TEST_CASE("Enumerate changing and creating elements") {
    constexpr std::size_t size = 2;
    std::array<int, size> array = { 1, 2 };

    SECTION("Enumerate should create pair with {idx, elm}") {
        auto enumerate = lz::enumerate(array);
        static_assert(std::is_same<decltype(enumerate.begin()), decltype(enumerate.end())>::value, "Should not be sentinel");
        auto element = *enumerate.begin();

        REQUIRE(element.first == 0);  // Idx
        REQUIRE(element.second == 1); // Element
    }

    SECTION("Enumerate should create pair with {idx, elm} with offset") {
        auto enumerate = lz::enumerate(array, 2);
        auto element = *enumerate.begin();

        REQUIRE(element.first == 2);  // Idx
        REQUIRE(element.second == 1); // Element
    }

    SECTION("Enumerate should be by reference") {
        auto enumerate = lz::enumerate(array);
        auto element = *enumerate.begin();
        element.second = 500;

        REQUIRE(array[0] == 500);
    }
}

TEST_CASE("Empty or one element enumerate") {
    SECTION("Empty") {
        std::string a;
        auto enumerate = lz::enumerate(a);
        REQUIRE(lz::empty(enumerate));
        REQUIRE(!lz::has_one(enumerate));
        REQUIRE(!lz::has_many(enumerate));
    }

    SECTION("One element") {
        std::string a = "h";
        auto enumerate = lz::enumerate(a);
        REQUIRE(!lz::empty(enumerate));
        REQUIRE(lz::has_one(enumerate));
        REQUIRE(!lz::has_many(enumerate));
    }
}

TEST_CASE("Enumerate binary operations") {
    constexpr std::size_t size = 3;
    std::array<int, size> array = { 1, 2, 3 };
    auto enumerate = lz::enumerate(array);

    SECTION("Operator++") {
        auto begin = enumerate.begin();
        ++begin;
        REQUIRE(begin->first == 1);
        REQUIRE(begin->second == 2);
    }

    SECTION("Operator--") {
        auto end = enumerate.end();
        --end;
        REQUIRE(end->first == 2);
        REQUIRE(end->second == 3);
        --end;
        REQUIRE(end->first == 1);
        REQUIRE(end->second == 2);
        --end;
        REQUIRE(end->first == 0);
        REQUIRE(end->second == 1);
    }

    SECTION("Operator== & operator!=") {
        auto begin = enumerate.begin();
        REQUIRE(begin != enumerate.end());
        begin = enumerate.end();
        REQUIRE(begin == enumerate.end());
    }

    SECTION("Operator+") {
        auto begin = enumerate.begin();
        auto end = enumerate.end();

        std::vector<std::pair<int, int>> expected = { { 0, 1 }, { 1, 2 }, { 2, 3 } };
        for (std::size_t i = 0; i < lz::size(enumerate) - 1; ++i) {
            INFO("With i = " << i);
            REQUIRE((*(begin + i)).first == (*(expected.begin() + i)).first);
            REQUIRE((*(begin + i)).second == (*(expected.begin() + i)).second);
        }
        REQUIRE(begin + lz::size(enumerate) == enumerate.end());
        for (std::size_t i = 1; i <= lz::size(enumerate); ++i) {
            INFO("With i = " << i);
            REQUIRE((*(end - i)).first == (*(expected.end() - i)).first);
            REQUIRE((*(end - i)).second == (*(expected.end() - i)).second);
        }
        REQUIRE(end - lz::size(enumerate) == enumerate.begin());

        std::advance(begin, lz::size(enumerate));
        std::advance(end, -static_cast<std::ptrdiff_t>(lz::size(enumerate)));

        for (std::size_t i = 1; i <= lz::size(enumerate); ++i) {
            INFO("With i = " << i);
            REQUIRE((*(begin - i)).first == (*(expected.end() - i)).first);
            REQUIRE((*(begin - i)).second == (*(expected.end() - i)).second);
        }
        REQUIRE(end + lz::size(enumerate) == enumerate.end());
        for (std::size_t i = 0; i < lz::size(enumerate) - 1; ++i) {
            INFO("With i = " << i);
            REQUIRE((*(end + i)).first == (*(expected.begin() + i)).first);
            REQUIRE((*(end + i)).second == (*(expected.begin() + i)).second);
        }
        REQUIRE(begin - lz::size(enumerate) == enumerate.begin());
    }

    SECTION("Operator-") {
        auto begin = enumerate.begin();
        auto end = enumerate.end();
        for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(lz::size(enumerate)); ++i) {
            INFO("With i = " << i);
            REQUIRE((end - i) - begin == static_cast<std::ptrdiff_t>(lz::size(enumerate) - i));
            REQUIRE(end - (begin + i) == static_cast<std::ptrdiff_t>(lz::size(enumerate) - i));
            REQUIRE((begin + i) - end == -static_cast<std::ptrdiff_t>(lz::size(enumerate) - i));
            REQUIRE(begin - (end - i) == -static_cast<std::ptrdiff_t>(lz::size(enumerate) - i));
        }

        for (std::size_t i = 0; i < lz::size(enumerate); ++i) {
            INFO("With i = " << i);
            REQUIRE((end - i) - (begin + i) == static_cast<std::ptrdiff_t>(lz::size(enumerate) - 2 * i));
            REQUIRE((begin + i) - (end - i) == -static_cast<std::ptrdiff_t>(lz::size(enumerate) - 2 * i));
        }
    }
}

TEST_CASE("Enumerate to containers") {
    constexpr std::size_t size = 3;
    std::array<int, size> array = { 1, 2, 3 };
    std::vector<int> vec = { 1, 2, 3 };

    SECTION("To array") {
        auto actual_array = array | lz::enumerate | lz::to<std::array<std::pair<int, int>, size>>();
        auto expected_pair = std::make_pair(0, 1);

        for (auto actual_pair : actual_array) {
            REQUIRE(actual_pair == expected_pair);
            expected_pair = std::make_pair(++expected_pair.first, ++expected_pair.second);
        }
    }

    SECTION("To vector") {
        auto actual_vec = lz::enumerate(vec) | lz::to<std::vector>();
        auto expected_pair = std::make_pair(0, 1);

        for (const auto& actual_pair : actual_vec) {
            REQUIRE(actual_pair.first == expected_pair.first);
            REQUIRE(actual_pair.second == expected_pair.second);
            expected_pair = std::make_pair(++expected_pair.first, ++expected_pair.second);
        }
    }

    SECTION("To other container using to<>()") {
        auto actual_list = lz::enumerate(vec) | lz::to<std::list>();
        auto expected_pair = std::make_pair(0, 1);

        for (const auto& actual_pair : actual_list) {
            REQUIRE(actual_pair.first == expected_pair.first);
            REQUIRE(actual_pair.second == expected_pair.second);
            expected_pair = std::make_pair(++expected_pair.first, ++expected_pair.second);
        }
    }

    SECTION("To map") {
        auto enumerator = lz::enumerate(array);
        auto actual =
            enumerator | lz::map([](const std::pair<int, int> pair) { return std::make_pair(pair.second, pair); }) |
            lz::to<std::map<int, std::pair<int, int>>>();

        std::map<int, std::pair<int, int>> expected = {
            std::make_pair(1, std::make_pair(0, 1)),
            std::make_pair(2, std::make_pair(1, 2)),
            std::make_pair(3, std::make_pair(2, 3)),
        };

        REQUIRE(actual == expected);
    }

    SECTION("To unordered map") {
        auto enumerator = lz::enumerate(array);
        auto actual =
            enumerator | lz::map([](const std::pair<int, int> pair) { return std::make_pair(pair.second, pair); }) |
            lz::to<std::unordered_map<int, std::pair<int, int>>>();

        std::unordered_map<int, std::pair<int, int>> expected = {
            std::make_pair(1, std::make_pair(0, 1)),
            std::make_pair(2, std::make_pair(1, 2)),
            std::make_pair(3, std::make_pair(2, 3)),
        };

        REQUIRE(actual == expected);
    }

    SECTION("Bidirectional to container") {
        std::list<int> to_enumerate = { 1, 2, 3 };
        auto enumerated = lz::enumerate(to_enumerate);
        std::array<std::pair<int, int>, 3> expected = { std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 3) };
        using ref_enumerated = lz::ref_iterable_t<decltype(enumerated)>;    
        using ref_expected = lz::ref_iterable_t<decltype(expected)>;
        REQUIRE(lz::equal(enumerated, expected, [](const ref_enumerated& a, const ref_expected& b) { return a.first == b.first && a.second == b.second; }));
    }
}
