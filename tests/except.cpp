#include <Lz/algorithm.hpp>
#include <Lz/c_string.hpp>
#include <Lz/common.hpp>
#include <Lz/except.hpp>
#include <Lz/map.hpp>
#include <Lz/range.hpp>
#include <array>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Except tests with sentinels") {
    const char* str = "Hello, World!";
    const char* to_except = "eo";
    auto c_str = lz::c_string(str);
    auto c_str_to_except = lz::c_string(to_except);
    auto except = lz::except(c_str, c_str_to_except);
    static_assert(!std::is_same<decltype(except.begin()), decltype(except.end())>::value, "Must be sentinel");
    REQUIRE((except | lz::to<std::string>()) == "Hll, Wrld!");
}

TEST_CASE("Empty or one element except") {
    SECTION("Empty") {
        std::string a;
        std::string b;
        auto except = lz::except(a, b);
        REQUIRE(lz::empty(except));
        REQUIRE(!lz::has_one(except));
        REQUIRE(!lz::has_many(except));
    }

    SECTION("One element 1") {
        std::string a = "h";
        std::string b;
        auto except = lz::except(a, b);
        REQUIRE(!lz::empty(except));
        REQUIRE(lz::has_one(except));
        REQUIRE(!lz::has_many(except));
    }

    SECTION("One element 2") {
        std::string a;
        std::string b = "w";
        auto except = lz::except(a, b);
        REQUIRE(lz::empty(except));
        REQUIRE(!lz::has_one(except));
        REQUIRE(!lz::has_many(except));
    }

    SECTION("One element both") {
        std::string a = "h";
        std::string b = "w";
        auto except = lz::except(a, b);
        REQUIRE(!lz::empty(except));
        REQUIRE(lz::has_one(except));
        REQUIRE(!lz::has_many(except));
    }

    SECTION("One element both 2") {
        std::string a = "h";
        std::string b = "h";
        auto except = lz::except(a, b);
        REQUIRE(lz::empty(except));
        REQUIRE(!lz::has_one(except));
        REQUIRE(!lz::has_many(except));
    }
}

TEST_CASE("Except excepts elements and is by reference") {
    std::vector<int> array{ 1, 2, 3, 4, 5 };
    std::vector<int> to_except{ 3, 5 };

    auto except = array | lz::except(to_except);
    auto it = except.begin();
    REQUIRE(*it == 1);

    SECTION("For-loop") {
        constexpr std::size_t s = 32;
        constexpr std::size_t es = 16;

        auto large_arr = lz::range(static_cast<int>(s)) | lz::to<std::array<int, s>>();
        auto to_large_except = lz::range(static_cast<int>(es)) | lz::to<std::array<int, es>>();

        auto ex = lz::except(large_arr, to_large_except);
        auto current = 16;
        lz::for_each(ex, [&current](int i) {
            REQUIRE(i == current);
            ++current;
        });
        current = 0;
    }

    SECTION("Excepts elements") {
        REQUIRE((except | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 4 });
    }

    SECTION("Is by reference") {
        *it = 0;
        REQUIRE(*it == array[0]);
    }

    SECTION("Excepted with >") {
        std::sort(to_except.begin(), to_except.end(), std::greater<int>());
        auto except_greater = lz::except(array, to_except, std::greater<int>());
        REQUIRE((except_greater | lz::to<std::array<int, 3>>()) == std::array<int, 3>{ 1, 2, 4 });
    }
}

TEST_CASE("Except binary operations") {
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<int> b = { 2, 3 };

    auto except = lz::except(a, b);
    auto it = except.begin();
    REQUIRE(*it == 1);

    SECTION("Operator++") {
        ++it;
        REQUIRE(*it == 4);
    }

    SECTION("Operator== & operator!=") {
        REQUIRE(it != except.end());
        while (it != except.end()) {
            ++it;
        }
        REQUIRE(it == except.end());
    }
}

TEST_CASE("Except to containers") {
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<int> b = { 1, 3 };
    auto except = lz::except(a, b);

    SECTION("To array") {
        auto excepted = except | lz::to<std::array<int, 2>>();
        REQUIRE(excepted == std::array<int, 2>{ 2, 4 });
    }

    SECTION("To vector") {
        auto excepted = except | lz::to<std::vector>();
        REQUIRE(excepted == std::vector<int>{ 2, 4 });
    }

    SECTION("To other container using to<>()") {
        auto excepted = except | lz::to<std::list>();
        REQUIRE(excepted == std::list<int>{ 2, 4 });
    }

    SECTION("To map") {
        auto actual = except | lz::map([](const int i) { return std::make_pair(i, i); }) |
                      lz::to<std::map<int, int>>();

        std::map<int, int> expected = {
            std::make_pair(2, 2),
            std::make_pair(4, 4),
        };

        REQUIRE(actual == expected);
    }

    SECTION("To unordered map") {
        auto actual = except | lz::map([](const int i) { return std::make_pair(i, i); }) |
                      lz::to<std::unordered_map<int, int>>();

        std::unordered_map<int, int> expected = {
            std::make_pair(2, 2),
            std::make_pair(4, 4),
        };

        REQUIRE(actual == expected);
    }
}
