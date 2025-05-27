#include <Lz/generate.hpp>
#include <Lz/map.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Generate infinite") {
    auto generator = lz::generate([]() { return 0; });
    static_assert(!std::is_same<decltype(generator.begin()), decltype(generator.end())>::value, "Should be sentinel");
    REQUIRE(generator.begin() != generator.end());
    REQUIRE(generator.begin() != generator.begin());

    auto begin = generator.begin();
    REQUIRE(*begin == 0);
    REQUIRE(begin != generator.end());
    REQUIRE(begin != generator.begin());
    ++begin;
    REQUIRE(*begin == 0);
    REQUIRE(begin != generator.end());
    REQUIRE(begin != generator.begin());

    SECTION("Operator=") {
        REQUIRE(begin != generator.end());
        begin = generator.end();
        // Inf never reaches end
        REQUIRE(begin != generator.end());
    }
}

TEST_CASE("Generate changing and creating elements") {
    constexpr std::size_t amount = 4;
    std::size_t counter = 0;
    auto generator = lz::generate(
        [&counter]() {
            auto tmp{ counter++ };
            return tmp;
        },
        amount);

    SECTION("Should be 0, 1, 2, 3") {
        std::size_t expected = 0;
        lz::for_each(generator, [&expected](std::size_t i) {
            REQUIRE(i == expected);
            ++expected;
        });
    }

    SECTION("Operator=") {
        auto begin = generator.begin();
        REQUIRE(begin != generator.end());
        begin = generator.end();
        REQUIRE(begin == generator.end());
    }
}

TEST_CASE("Generate binary operations") {
    constexpr std::size_t amount = 4;
    std::size_t counter = 0;
    auto generator = lz::generate(
        [&counter]() {
            auto tmp{ counter++ };
            return tmp;
        },
        amount);
    auto begin = generator.begin();

    SECTION("Operator++") {
        auto expected = { 0, 1, 2, 3 };
        REQUIRE(lz::equal(generator, expected));
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(begin != generator.end());
        while (begin != generator.end()) {
            ++begin;
        }
        REQUIRE(begin == generator.end());
    }
}

TEST_CASE("Empty or one element generate") {
    SECTION("Empty") {
        auto generator = lz::generate([]() { return 0; }, 0);
        REQUIRE(lz::empty(generator));
        REQUIRE(!lz::has_one(generator));
        REQUIRE(!lz::has_many(generator));
    }

    SECTION("One element") {
        auto generator = lz::generate([]() { return 0; }, 1);
        REQUIRE(!lz::empty(generator));
        REQUIRE(lz::has_one(generator));
        REQUIRE(!lz::has_many(generator));
    }
}

TEST_CASE("Generate to containers") {
    constexpr std::size_t amount = 4;
    std::size_t counter = 0;

    auto generator = lz::generate(
        [&counter]() {
            auto tmp{ counter++ };
            return tmp;
        },
        amount);

    SECTION("To array") {
        auto array = generator | lz::to<std::array<std::size_t, amount>>();
        std::array<std::size_t, amount> expected = { 0, 1, 2, 3 };

        REQUIRE(array == expected);
    }

    SECTION("To vector") {
        auto vector = generator | lz::to<std::vector>();
        std::vector<std::size_t> expected = { 0, 1, 2, 3 };

        REQUIRE(vector == expected);
    }

    SECTION("To other container using to<>()") {
        auto vector = generator | lz::to<std::list>();
        std::list<std::size_t> expected = { 0, 1, 2, 3 };

        REQUIRE(vector == expected);
    }

    SECTION("To map") {
        auto map = generator | lz::map([](const std::size_t elm) { return std::make_pair(elm * 10, elm); }) |
                   lz::to<std::map<std::size_t, std::size_t>>();

        std::map<std::size_t, std::size_t> expected = { { 0, 0 }, { 10, 1 }, { 20, 2 }, { 30, 3 } };

        REQUIRE(map == expected);
    }

    SECTION("To unordered map") {
        auto map = generator | lz::map([](const std::size_t elm) { return std::make_pair(elm * 10, elm); }) |
                   lz::to<std::unordered_map<std::size_t, std::size_t>>();

        std::unordered_map<std::size_t, std::size_t> expected = { { 0, 0 }, { 10, 1 }, { 20, 2 }, { 30, 3 } };

        REQUIRE(map == expected);
    }
}