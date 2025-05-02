#include <Lz/map.hpp>
#include <Lz/repeat.hpp>
#include <array>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("repeat_iterable changing and creating elements") {
    int to_repeat = 20;
    auto repeater = lz::repeat(to_repeat, 5);

    SECTION("Should be 5 times 20") {
        std::size_t counter = 0;
        lz::for_each(repeater, [&counter, to_repeat](int i) {
            REQUIRE(i == to_repeat);
            ++counter;
        });
        REQUIRE(counter == 5);
    }

    SECTION("Should not be by reference") {
        auto start = repeater.begin();
        REQUIRE(&(*start) != &to_repeat);
    }
}

TEST_CASE("repeat_iterable binary operations") {
    const int amount = 5;
    auto repeater = lz::repeat(20, amount);
    auto begin = repeater.begin();

    SECTION("Operator++") {
        ++begin;
        REQUIRE(lz::distance(begin, repeater.end()) == amount - 1);
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(begin != repeater.end());
        while (begin != repeater.end()) {
            ++begin;
        }
        REQUIRE(begin == repeater.end());
    }

    SECTION("Operator=") {
        begin = repeater.begin();
        REQUIRE(begin == repeater.begin());
        begin = repeater.end();
        REQUIRE(begin == repeater.end());
    }
}

TEST_CASE("Empty or one element repeat") {
    SECTION("Empty") {
        auto repeater = lz::repeat(20, 0);
        REQUIRE(lz::empty(repeater));
        REQUIRE(!lz::has_one(repeater));
        REQUIRE(!lz::has_many(repeater));
    }

    SECTION("One element with result") {
        auto repeater = lz::repeat(20, 1);
        REQUIRE(!lz::empty(repeater));
        REQUIRE(lz::has_one(repeater));
        REQUIRE(!lz::has_many(repeater));
    }
}

TEST_CASE("repeat_iterable to containers") {
    constexpr auto times = 5;
    const int to_repeat = 20;
    auto repeater = lz::repeat(to_repeat, times);

    SECTION("To array") {
        std::array<int, times> array = repeater | lz::to<std::array<int, times>>();
        for (int i : array) {
            REQUIRE(i == to_repeat);
        }
    }

    SECTION("To vector") {
        std::vector<int> vec = repeater | lz::to<std::vector>();
        for (int i : vec) {
            REQUIRE(i == to_repeat);
        }
        REQUIRE(vec.size() == times);
    }

    SECTION("To other container using to<>()") {
        std::list<int> lst = repeater | lz::to<std::list>();
        for (int i : lst) {
            REQUIRE(i == to_repeat);
        }
        REQUIRE(lst.size() == times);
    }

    SECTION("To map") {
        auto actual = repeater | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();
        std::map<int, int> expected;

        for (int i = 0; i < times; i++) {
            expected.insert(std::make_pair(20, 20));
        }

        REQUIRE(actual == expected);
    }

    SECTION("To unordered map") {
        auto actual =
            repeater | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        std::unordered_map<int, int> expected;

        for (int i = 0; i < times; i++) {
            expected.insert(std::make_pair(20, 20));
        }

        REQUIRE(actual == expected);
    }
}

TEST_CASE("repeat_iterable infinite") {
    int to_repeat = 20;
    auto repeater = lz::repeat(to_repeat);

    SECTION("Should be infinite") {
        std::size_t counter = 0;
        lz::for_each_while(repeater, [&counter](int i) {
            REQUIRE(i == 20);
            ++counter;
            if (counter == 100) {
                return false;
            }
            return true;
        });
        REQUIRE(counter == 100);
    }

    SECTION("Should not be by reference") {
        auto start = repeater.begin();
        REQUIRE(&(*start) != &to_repeat);
    }
}

TEST_CASE("repeat_iterable infinite binary operations") {
    auto repeater = lz::repeat(20);
    auto begin = repeater.begin();

    SECTION("Operator++") {
        ++begin;
        REQUIRE(*begin == 20);
    }

    SECTION("Not empty, has many, not has one") {
        REQUIRE(!lz::empty(repeater));
        REQUIRE(lz::has_many(repeater));
        REQUIRE(!lz::has_one(repeater));
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(begin != repeater.end());
        std::size_t counter = 0;
        while (begin != repeater.end()) {
            ++begin;
            ++counter;
            if (counter == 100) {
                break;
            }
        }
        REQUIRE(counter == 100);
        REQUIRE(begin != repeater.end());
        REQUIRE(begin != begin);
    }
}