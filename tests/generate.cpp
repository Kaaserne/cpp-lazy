#include <Lz/generate.hpp>
#include <catch2/catch.hpp>
#include <list>

#ifdef LZ_HAS_CXX_11
#include <Lz/common.hpp>
#endif // LZ_HAS_CXX_11

TEST_CASE("Generate changing and creating elements", "[Generate][Basic functionality]") {
    auto compile_test = lz::generate([]() { return 0; });
    static_assert(!std::is_same<decltype(compile_test.begin()), decltype(compile_test.end())>::value, "Should be sentinel");
    static_cast<void>(compile_test);

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
        generator.for_each([&expected](std::size_t i) {
            CHECK(i == expected);
            ++expected;
        });
    }
}

TEST_CASE("Generate binary operations", "[Generate][Binary ops]") {
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
        ++begin;
        CHECK(lz::distance(begin, generator.end()) == amount - 1);
    }

    SECTION("Operator== & Operator!=") {
        CHECK(begin != generator.end());
        while (begin != generator.end()) {
            ++begin;
        }
        CHECK(begin == generator.end());
    }
}

TEST_CASE("Empty or one element generate") {
    SECTION("Empty") {
        auto generator = lz::generate([]() { return 0; }, 0);
        CHECK(lz::empty(generator));
        CHECK(!lz::has_one(generator));
        CHECK(!lz::has_many(generator));
    }

    SECTION("One element") {
        auto generator = lz::generate([]() { return 0; }, 1);
        CHECK(!lz::empty(generator));
        CHECK(lz::has_one(generator));
        CHECK(!lz::has_many(generator));
    }
}

TEST_CASE("Generate to containers", "[Generate][To container]") {
    constexpr std::size_t amount = 4;
    std::size_t counter = 0;

    auto generator = lz::generate(
        [&counter]() {
            auto tmp{ counter++ };
            return tmp;
        },
        amount);

    SECTION("To array") {
        auto array = generator.to<std::array<std::size_t, amount>>();
        std::array<std::size_t, amount> expected = { 0, 1, 2, 3 };

        CHECK(array == expected);
    }

    SECTION("To vector") {
        std::vector<std::size_t> vector = generator.to_vector();
        std::vector<std::size_t> expected = { 0, 1, 2, 3 };

        CHECK(vector == expected);
    }

    SECTION("To other container using to<>()") {
        std::list<std::size_t> vector = generator.to<std::list<std::size_t>>();
        std::list<std::size_t> expected = { 0, 1, 2, 3 };

        CHECK(vector == expected);
    }

    SECTION("To map") {
        std::map<std::size_t, std::size_t> map =
            generator.to_map([](const std::size_t elm) { return std::make_pair(elm * 10, elm); });

        std::map<std::size_t, std::size_t> expected = { { 0, 0 }, { 10, 1 }, { 20, 2 }, { 30, 3 } };

        CHECK(map == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<std::size_t, std::size_t> map =
            generator.to_unordered_map([](const std::size_t elm) { return std::make_pair(elm * 10, elm); });

        std::unordered_map<std::size_t, std::size_t> expected = { { 0, 0 }, { 10, 1 }, { 20, 2 }, { 30, 3 } };

        CHECK(map == expected);
    }
}