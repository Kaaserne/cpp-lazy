#include <Lz/generate_while.hpp>
#include <Lz/map.hpp>
#include <doctest/doctest.h>
#include <functional>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Generate while changing and creating elements") {
    const auto compile_test1 = lz::generate_while([]() { return std::make_pair(false, false); });
    static_cast<void>(compile_test1);

    const int i2 = 0;
    const auto compile_test2 = lz::generate_while([&i2]() {
        static_cast<void>(i2);
        return std::make_pair(false, false);
    });
    static_cast<void>(compile_test2);
    static_assert(!std::is_same<decltype(compile_test1.begin()), decltype(compile_test1.end())>::value, "Should be sentinel");

    int i = 0;
    auto gen_with_ref = lz::generate_while([&i]() -> std::pair<bool, int&> { return { i == 4, i }; });
    static_assert(std::is_same<decltype(*gen_with_ref.begin()), int&>::value,
                  "int& and decltype(*generator.begin()) are not the same");

    SUBCASE("Should be 0, 1, 2, 3") {
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy != 4, copy);
        });
        std::array<int, 4> expected = { 0, 1, 2, 3 };
        auto actual = generator | lz::to<std::array<int, expected.size()>>();
        REQUIRE(expected == actual);
        i = 0;
    }

    SUBCASE("Operator=") {
        auto begin = gen_with_ref.begin();
        REQUIRE(begin == gen_with_ref.end());
        begin = gen_with_ref.end();
        REQUIRE(begin == gen_with_ref.end());
    }
}

TEST_CASE("Empty or one element generate while") {
    SUBCASE("Empty") {
        std::function<std::pair<bool, int>()> func = []() {
            return std::make_pair(false, 0);
        };
        lz::generate_while_iterable<decltype(func)> generator = lz::generate_while(std::move(func));
        REQUIRE(lz::empty(generator));
        REQUIRE_FALSE(lz::has_one(generator));
        REQUIRE_FALSE(lz::has_many(generator));
    }

    SUBCASE("One element") {
        bool b = true;
        auto generator = lz::generate_while([&b]() {
            auto p = std::make_pair(b, 0);
            b = false;
            return p;
        });
        REQUIRE_FALSE(lz::empty(generator));
        REQUIRE(lz::has_one(generator));
        REQUIRE_FALSE(lz::has_many(generator));
    }
}

TEST_CASE("Generate while binary operations") {
    auto test = lz::generate_while([]() { return std::make_pair(true, 0); });
    static_assert(std::is_same<decltype(*test.begin()), int>::value, "int and decltype(*generator.begin()) are not the same");

    SUBCASE("Operator++") {
        int i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy != 4, copy);
        });
        auto expected = std::vector<int>{ 0, 1, 2, 3 };
        REQUIRE(lz::equal(generator, expected));
    }

    SUBCASE("Operator== & Operator!=") {
        int i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy != 4, copy);
        });
        auto begin = generator.begin();
        ++begin;
        REQUIRE(begin != generator.begin());
        ++begin, ++begin, ++begin;
        REQUIRE(begin == generator.end());
        begin = generator.begin();
        i = 0;
        REQUIRE(*begin == 0);
        while (begin != generator.end()) {
            ++begin;
        }
        REQUIRE(begin == generator.end());
    }
}

TEST_CASE("Generate while to containers") {
    SUBCASE("To array") {
        int i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy != 4, copy);
        });
        std::array<int, 4> expected = { 0, 1, 2, 3 };
        auto actual = generator | lz::to<std::array<int, 4>>();
        REQUIRE(expected == actual);
    }

    SUBCASE("To vector") {
        int i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy != 4, copy);
        });
        std::vector<int> expected = { 0, 1, 2, 3 };
        auto actual = generator | lz::to<std::vector>();
        REQUIRE(expected == actual);
    }

    SUBCASE("To other container using to<>()") {
        int i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy != 4, copy);
        });
        std::list<int> expected = { 0, 1, 2, 3 };
        auto actual = generator | lz::to<std::list<int>>();
        REQUIRE(expected == actual);
    }

    SUBCASE("To map") {
        int i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy != 4, copy);
        });
        std::map<int, int> expected = { { 0, 0 }, { 1, 1 }, { 2, 2}, { 3, 3 }};
        auto actual = generator | lz::map([](int x) { return std::make_pair(x, x); }) | lz::to<std::map<int, int>>();
        REQUIRE(actual == expected);
    }

    SUBCASE("To unordered map") {
        int i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy != 4, copy);
        });
        std::unordered_map<int, int> expected = { { 0, 0 }, { 1, 1 }, { 2, 2}, { 3, 3 }};
        auto actual = generator | lz::map([](int x) { return std::make_pair(x, x); }) | lz::to<std::unordered_map<int, int>>();
        REQUIRE(actual == expected);
    }
}