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
    REQUIRE(lz::equal(taken, expected));
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

TEST_CASE("Enumerate changing and creating elements", "[Enumerate][Basic functionality]") {
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

TEST_CASE("Enumerate binary operations", "[Enumerate][Binary ops]") {
    constexpr std::size_t size = 3;
    std::array<int, size> array = { 1, 2, 3 };
    auto enumerate = lz::enumerate(array);
    auto begin = enumerate.begin();
    ++begin; // Increment by one

    SECTION("Operator++") {
        REQUIRE(begin->first == 1);  // Index
        REQUIRE(begin->second == 2); // element
    }

    SECTION("Operator--") {
        --begin;
        // Decrement by one, back at begin()
        REQUIRE(begin->first == 0);  // Index
        REQUIRE(begin->second == 1); // element
    }

    SECTION("Operator== & operator!=") {
        REQUIRE(begin != enumerate.end());
        begin = enumerate.end();
        REQUIRE(begin == enumerate.end());
    }

    SECTION("Operator+(int), tests += as well") {
        REQUIRE((begin + 1)->first == 2);  // Index
        REQUIRE((begin + 1)->second == 3); // element
    }

    SECTION("Operator-(int), tests -= as well") {
        REQUIRE((begin - 1)->first == 0);  // Index
        REQUIRE((begin - 1)->second == 1); // element
    }

    SECTION("Operator-(Iterator)") {
        REQUIRE(enumerate.end() - begin == 2);
    }

    SECTION("Operator[]()") {
        REQUIRE(enumerate.begin()[1].first == 1);
        REQUIRE(enumerate.begin()[1].second == 2);
    }

    SECTION("Operator<, '<, <=, >, >='") {
        const auto b = enumerate.begin();
        const auto end = enumerate.end();
        const auto distance = std::distance(b, end);

        REQUIRE(b < end);
        REQUIRE(b + distance - 1 > end - distance);
        REQUIRE(b + distance - 1 <= end);
        REQUIRE(b + size - 1 >= end - 1);
    }
}

TEST_CASE("Enumerate to containers", "[Enumerate][To container]") {
    constexpr std::size_t size = 3;
    std::array<int, size> array = { 1, 2, 3 };
    std::vector<int> vec = { 1, 2, 3 };

    SECTION("To array") {
        auto actual_array = lz::enumerate(array) | lz::to<std::array<std::pair<int, int>, size>>();
        auto expceted_pair = std::make_pair(0, 1);

        for (auto actual_pair : actual_array) {
            REQUIRE(actual_pair == expceted_pair);
            expceted_pair = std::make_pair(++expceted_pair.first, ++expceted_pair.second);
        }
    }

    SECTION("To vector") {
        auto actual_vec = lz::enumerate(vec) | lz::to<std::vector>();
        auto expceted_pair = std::make_pair(0, 1);

        for (const auto& actual_pair : actual_vec) {
            REQUIRE(actual_pair == expceted_pair);
            expceted_pair = std::make_pair(++expceted_pair.first, ++expceted_pair.second);
        }
    }

    SECTION("To other container using to<>()") {
        auto actual_list = lz::enumerate(vec) | lz::to<std::list>();
        auto expceted_pair = std::make_pair(0, 1);

        for (const auto& actual_pair : actual_list) {
            REQUIRE(actual_pair == expceted_pair);
            expceted_pair = std::make_pair(++expceted_pair.first, ++expceted_pair.second);
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
        REQUIRE(lz::equal(enumerated, expected));
    }
}
