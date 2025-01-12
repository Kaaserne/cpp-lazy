#include <Lz/repeat.hpp>
#include <array>
#include <catch2/catch.hpp>
#include <list>

#ifdef LZ_HAS_CXX_11
#include <Lz/common.hpp>
#endif // LZ_HAS_CXX_11

TEST_CASE("repeat_iterable changing and creating elements", "[repeat_iterable][Basic functionality]") {
    int to_repeat = 20;
    auto repeater = lz::repeat(to_repeat, 5);

    SECTION("Should be 5 times 20") {
        std::size_t counter = 0;
        repeater.for_each([&counter, to_repeat](int i) {
            CHECK(i == to_repeat);
            ++counter;
        });
        CHECK(counter == 5);
    }

    SECTION("Should not be by reference") {
        auto start = repeater.begin();
        CHECK(&(*start) != &to_repeat);
    }
}

TEST_CASE("repeat_iterable binary operations", "[repeat_iterable][Binary ops]") {
    const int amount = 5;
    auto repeater = lz::repeat(20, amount);
    auto begin = repeater.begin();

    SECTION("Operator++") {
        ++begin;
        CHECK(lz::distance(begin, repeater.end()) == amount - 1);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(begin != repeater.end());
        while (begin != repeater.end()) {
            ++begin;
        }
        CHECK(begin == repeater.end());
    }
}

TEST_CASE("Empty or one element repeat") {
    SECTION("Empty") {
        auto repeater = lz::repeat(20, 0);
        CHECK(lz::empty(repeater));
        CHECK(!lz::has_one(repeater));
        CHECK(!lz::has_many(repeater));
    }

    SECTION("One element with result") {
        auto repeater = lz::repeat(20, 1);
        CHECK(!lz::empty(repeater));
        CHECK(lz::has_one(repeater));
        CHECK(!lz::has_many(repeater));
    }
}

TEST_CASE("repeat_iterable to containers", "[repeat_iterable][To container]") {
    constexpr auto times = 5;
    const int to_repeat = 20;
    auto repeater = lz::repeat(to_repeat, times);

    SECTION("To array") {
        std::array<int, times> array = repeater.to<std::array<int, times>>();
        for (int i : array) {
            CHECK(i == to_repeat);
        }
    }

    SECTION("To vector") {
        std::vector<int> vec = repeater.to_vector();
        for (int i : vec) {
            CHECK(i == to_repeat);
        }
        CHECK(vec.size() == times);
    }

    SECTION("To other container using to<>()") {
        std::list<int> lst = repeater.to<std::list<int>>();
        for (int i : lst) {
            CHECK(i == to_repeat);
        }
        CHECK(lst.size() == times);
    }

    SECTION("To map") {
        std::map<int, int> actual = repeater.to_map([](const int i) { return std::make_pair(i, i); });
        std::map<int, int> expected;

        for (int i = 0; i < times; i++) {
            expected.insert(std::make_pair(20, 20));
        }

        CHECK(actual == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> actual = repeater.to_unordered_map([](const int i) { return std::make_pair(i, i); });
        std::unordered_map<int, int> expected;

        for (int i = 0; i < times; i++) {
            expected.insert(std::make_pair(20, 20));
        }

        CHECK(actual == expected);
    }
}
