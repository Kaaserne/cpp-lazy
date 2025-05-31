#include <Lz/enumerate.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <Lz/take.hpp>
#include <c_string/c_string_forward_decl.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <test_procs.hpp>
#include <unordered_map>

TEST_CASE("Enumerate with sentinels") {
    const char* str = "Hello";
    auto c_string = lz::c_string(str);
    lz::enumerate_iterable<decltype(c_string)> enumerated = lz::enumerate(c_string);
    static_assert(!std::is_same<decltype(enumerated.begin()), decltype(enumerated.end())>::value,
                  "Begin and end should not be the same type");
    auto taken = lz::take(enumerated, 3);
    std::vector<std::pair<int, char>> expected = { { 0, 'H' }, { 1, 'e' }, { 2, 'l' } };

    using ref_taken = lz::ref_iterable_t<decltype(taken)>;
    using ref_expected = lz::ref_iterable_t<decltype(expected)>;
    REQUIRE(lz::equal(taken, expected, [](ref_taken a, ref_expected b) { return a.first == b.first && a.second == b.second; }));

    SECTION("Operator=") {
        auto it = enumerated.begin();
        REQUIRE(it == enumerated.begin());
        it = enumerated.end();
        REQUIRE(it == enumerated.end());
    }
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
        auto expected = std::vector<std::pair<int, int>>{ { 0, 1 }, { 1, 2 }, { 2, 3 } };
        REQUIRE(lz::equal(enumerate, expected, [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.first == b.first && a.second == b.second;
        }));
    }

    SECTION("Operator--") {
        auto reverse_expected = std::vector<std::pair<int, int>>{ { 2, 3 }, { 1, 2 }, { 0, 1 } };
        REQUIRE(
            lz::equal(lz::reverse(enumerate), reverse_expected, [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                return a.first == b.first && a.second == b.second;
            }));
    }

    SECTION("Operator== & operator!=") {
        auto begin = enumerate.begin();
        REQUIRE(begin != enumerate.end());
        begin = enumerate.end();
        REQUIRE(begin == enumerate.end());
    }

    SECTION("Operator+") {
        std::vector<std::pair<int, int>> expected = { { 0, 1 }, { 1, 2 }, { 2, 3 } };
        const auto cmp = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            INFO("Comparing pairs: " << a.first << ", " << a.second << " with " << b.first << ", " << b.second);
            return a.first == b.first && a.second == b.second;
        };
        test_procs::test_operator_plus(enumerate, expected, cmp);
    }

    SECTION("Operator-") {
        test_procs::test_operator_minus(enumerate);
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
        REQUIRE(lz::equal(enumerated, expected,
                          [](ref_enumerated a, ref_expected b) { return a.first == b.first && a.second == b.second; }));
    }
}
