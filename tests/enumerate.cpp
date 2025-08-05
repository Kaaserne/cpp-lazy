#include <Lz/enumerate.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <Lz/take.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <cpp-lazy-ut-helper/repeat.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <doctest/doctest.h>
#include <list>
#include <map>
#include <unordered_map>

struct equal_fn {
    template<typename T, typename U>
    bool operator()(const T& a, const U& b) const {
        INFO("Comparing pairs: " << a.first << ", " << a.second << " with " << b.first << ", " << b.second);
        return a.first == b.first && a.second == b.second;
    }
};

TEST_CASE("Enumerate with sentinels") {
    const char* str = "Hello";
    auto c_string = lz::c_string(str);
    lz::enumerate_iterable<decltype(c_string)> enumerated = lz::enumerate(c_string);
    static_assert(!std::is_same<decltype(enumerated.begin()), decltype(enumerated.end())>::value,
                  "Begin and end should not be the same type");
    auto taken = lz::take(enumerated, 3);
    std::vector<std::pair<int, char>> expected = { { 0, 'H' }, { 1, 'e' }, { 2, 'l' } };
    REQUIRE(lz::equal(taken, expected, equal_fn{}));

    SUBCASE("Operator=") {
        auto it = enumerated.begin();
        REQUIRE(it == enumerated.begin());
        REQUIRE(enumerated.begin() == it);
        REQUIRE(it != enumerated.end());
        REQUIRE(enumerated.end() != it);
        it = enumerated.end();
        REQUIRE(it == enumerated.end());
        REQUIRE(it != enumerated.begin());
        REQUIRE(enumerated.end() == it);
        REQUIRE(enumerated.begin() != it);
    }
}

TEST_CASE("Empty or one element enumerate") {
    SUBCASE("Empty") {
        std::string a;
        auto enumerate = lz::enumerate(a);
        REQUIRE(lz::empty(enumerate));
        REQUIRE_FALSE(lz::has_one(enumerate));
        REQUIRE_FALSE(lz::has_many(enumerate));
    }

    SUBCASE("One element") {
        std::string a = "h";
        auto enumerate = lz::enumerate(a);
        REQUIRE_FALSE(lz::empty(enumerate));
        REQUIRE(lz::has_one(enumerate));
        REQUIRE_FALSE(lz::has_many(enumerate));
    }
}

TEST_CASE("Enumerate binary operations") {
    constexpr std::size_t size = 3;
    std::array<int, size> array = { 1, 2, 3 };
    auto enumerate = lz::enumerate(array, 2);

    SUBCASE("Operator++") {
        auto expected = std::vector<std::pair<int, int>>{ { 2, 1 }, { 3, 2 }, { 4, 3 } };
        REQUIRE(lz::equal(enumerate, expected, equal_fn{}));
    }

    SUBCASE("Operator--") {
        auto reverse_expected = std::vector<std::pair<int, int>>{ { 4, 3 }, { 3, 2 }, { 2, 1 } };
        REQUIRE(lz::equal(lz::reverse(enumerate), reverse_expected, equal_fn{}));
    }

    SUBCASE("Operator== & operator!=") {
        auto it = enumerate.begin();
        REQUIRE(it != enumerate.end());
        REQUIRE(it == enumerate.begin());
        REQUIRE(enumerate.begin() == it);
        REQUIRE(it != enumerate.end());
        it = enumerate.end();
        REQUIRE(it == enumerate.end());
        REQUIRE(it != enumerate.begin());
        REQUIRE(enumerate.end() == it);
        REQUIRE(enumerate.begin() != it);
    }

    SUBCASE("Operator+") {
        std::vector<std::pair<int, int>> expected = { { 2, 1 }, { 3, 2 }, { 4, 3 } };
        test_procs::test_operator_plus(enumerate, expected, equal_fn{});
    }

    SUBCASE("Operator-") {
        test_procs::test_operator_minus(enumerate);
    }

    SUBCASE("Operator-(default_sentinel_t)") {
        auto repeat = lz::repeat(1, 5);
        test_procs::test_operator_minus(repeat);
    }
}

TEST_CASE("Enumerate to containers") {
    constexpr std::size_t size = 3;
    std::array<int, size> array = { 1, 2, 3 };
    std::vector<int> vec = { 1, 2, 3 };

    SUBCASE("To array") {
        auto actual_array = array | lz::enumerate | lz::to<std::array<std::pair<int, int>, size>>();
        auto expected = { std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 3) };
        REQUIRE(lz::equal(actual_array, expected, equal_fn{}));
    }

    SUBCASE("To vector") {
        auto actual_vec = lz::enumerate(vec) | lz::to<std::vector>();
        auto expected = { std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 3) };
        REQUIRE(lz::equal(actual_vec, expected, equal_fn{}));
    }

    SUBCASE("To other container using to<>()") {
        auto actual_list = lz::enumerate(vec) | lz::to<std::list>();
        auto expected = { std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 3) };
        REQUIRE(lz::equal(actual_list, expected, equal_fn{}));
    }

    SUBCASE("To map") {
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

    SUBCASE("To unordered map") {
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

    SUBCASE("Bidirectional to container") {
        std::list<int> to_enumerate = { 1, 2, 3 };
        auto enumerated = lz::enumerate(to_enumerate);
        std::array<std::pair<int, int>, 3> expected = { std::make_pair(0, 1), std::make_pair(1, 2), std::make_pair(2, 3) };
        using ref_enumerated = lz::ref_iterable_t<decltype(enumerated)>;    
        using ref_expected = lz::ref_iterable_t<decltype(expected)>;
        REQUIRE(lz::equal(enumerated, expected,
                          [](ref_enumerated a, ref_expected b) { return a.first == b.first && a.second == b.second; }));
    }
}
