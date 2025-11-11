#include <Lz/algorithm/empty.hpp>
#include <Lz/algorithm/equal.hpp>
#include <Lz/algorithm/has_many.hpp>
#include <Lz/algorithm/has_one.hpp>
#include <Lz/generate_while.hpp>
#include <Lz/map.hpp>
#include <Lz/procs/to.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

TEST_CASE("Generate while changing and creating elements") {
    const auto compile_test1 = lz::generate_while([]() { return std::make_pair(false, false); });
    static_cast<void>(compile_test1);

    const unsigned i2 = 0;
    const auto compile_test2 = lz::generate_while([&i2]() {
        static_cast<void>(i2);
        return std::make_pair(false, false);
    });
    static_cast<void>(compile_test2);
    static_assert(!std::is_same<decltype(compile_test1.begin()), decltype(compile_test1.end())>::value, "Should be sentinel");

    SUBCASE("Should be 0, 1, 2, 3") {
        unsigned i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        std::array<unsigned, 4> expected = { 0, 1, 2, 3 };
        auto actual = generator | lz::to<std::array<unsigned, expected.size()>>();
        REQUIRE(expected == actual);
        i = 0;
    }

    SUBCASE("Operator=(default_sentinel_t)") {
        unsigned j = 0;
        auto gen = lz::generate_while([&j]() {
            j++;
            return std::make_pair(j, j < 5);
        });
        auto common = make_sentinel_assign_op_tester(gen);
        auto expected = { 1u, 2u, 3u, 4u };
        REQUIRE(lz::equal(common, expected));
    }
}

TEST_CASE("Empty or one element generate while") {
    SUBCASE("Empty") {
        std::function<std::pair<bool, unsigned>()> func = []() {
            return std::make_pair(0, false);
        };
        lz::generate_while_iterable<decltype(func)> generator = lz::generate_while(std::move(func));
        REQUIRE(lz::empty(generator));
        REQUIRE_FALSE(lz::has_one(generator));
        REQUIRE_FALSE(lz::has_many(generator));
    }

    SUBCASE("One element") {
        bool b = true;
        auto generator = lz::generate_while([&b]() {
            auto p = std::make_pair(0, b);
            b = false;
            return p;
        });
        REQUIRE_FALSE(lz::empty(generator));
        REQUIRE(lz::has_one(generator));
        REQUIRE_FALSE(lz::has_many(generator));
    }
}

TEST_CASE("Generate while binary operations") {
    auto test = lz::generate_while([]() { return std::make_pair(0u, true); });
    static_assert(std::is_same<decltype(*test.begin()), unsigned>::value,
                  "unsigned and decltype(*generator.begin()) are not the same");

    SUBCASE("Operator++") {
        unsigned i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        auto expected = std::vector<unsigned>{ 0, 1, 2, 3 };
        REQUIRE(lz::equal(generator, expected));
    }

    SUBCASE("Operator== & Operator!=") {
        unsigned i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        auto begin = generator.begin();
        ++begin;
        REQUIRE(begin != generator.begin());
        REQUIRE(generator.begin() != begin);
        REQUIRE_FALSE(begin == generator.begin());
        REQUIRE_FALSE(generator.begin() == begin);
        ++begin;
        ++begin;
        ++begin;
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
        unsigned i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        std::array<unsigned, 4> expected = { 0, 1, 2, 3 };
        auto actual = generator | lz::to<std::array<unsigned, 4>>();
        REQUIRE(expected == actual);
    }

    SUBCASE("To vector") {
        unsigned i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        auto it = generator.begin();
        it = generator.end();
        REQUIRE(it == generator.end());
        REQUIRE(generator.end() == it);
        REQUIRE_FALSE(it == generator.begin());
        REQUIRE(it != generator.begin());
        REQUIRE(generator.begin() != it);
        std::vector<unsigned> expected = { 0, 1, 2, 3 };
        auto actual = generator | lz::to<std::vector>();
        REQUIRE(expected == actual);
    }

    SUBCASE("To other container using to<>()") {
        unsigned i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        std::list<unsigned> expected = { 0, 1, 2, 3 };
        auto actual = generator | lz::to<std::list<unsigned>>();
        REQUIRE(expected == actual);
    }

    SUBCASE("To map") {
        unsigned i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        std::map<unsigned, unsigned> expected = { { 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 3 } };
        auto actual =
            generator | lz::map([](unsigned x) { return std::make_pair(x, x); }) | lz::to<std::map<unsigned, unsigned>>();
        REQUIRE(actual == expected);
    }

    SUBCASE("To unordered map") {
        unsigned i = 0;
        auto generator = lz::generate_while([&i]() {
            auto copy = i++;
            return std::make_pair(copy, copy != 4);
        });
        std::unordered_map<unsigned, unsigned> expected = { { 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 3 } };
        auto actual = generator | lz::map([](unsigned x) { return std::make_pair(x, x); }) |
                      lz::to<std::unordered_map<unsigned, unsigned>>();
        REQUIRE(actual == expected);
    }
}
