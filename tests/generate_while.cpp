#include <Lz/generate_while.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Generate while changing and creating elements", "[Generate while][Basic functionality]") {
    const auto compile_test1 = lz::generate_while([](bool) { return std::make_pair(false, false); }, false);
    static_cast<void>(compile_test1);
    const auto compile_test2 = lz::generate_while([](const int&) { return std::make_pair(false, false); }, 0);
    static_cast<void>(compile_test2);
    static_assert(!std::is_same<decltype(compile_test1.begin()), decltype(compile_test1.end())>::value, "Should be sentinel");
    auto gen_with_ref = lz::generate_while([](int& i) -> std::pair<bool, int&> { return { i == 4, i }; }, 0);
    static_assert(std::is_same<decltype(*gen_with_ref.begin()), int&>::value,
                  "int& and decltype(*generator.begin()) are not the same");
                  

    SECTION("Should be 0, 1, 2, 3") {
        auto generator = lz::generate_while(
            [](int& i) {
                auto copy = i++;
                return std::make_pair(copy != 4, copy);
            },
            0);
        std::array<int, 4> expected = { 0, 1, 2, 3 };
        auto actual = generator.to<std::array<int, expected.size()>>();
        CHECK(expected == actual);
    }
}

TEST_CASE("Empty or one element generate while") {
    SECTION("Empty") {
        auto generator = lz::generate_while([]() { return std::make_pair(false, 0); });
        CHECK(lz::empty(generator));
        CHECK(!lz::has_one(generator));
        CHECK(!lz::has_many(generator));
    }

    SECTION("One element") {
        bool b = true;
        auto generator = lz::generate_while([&b]() {
            auto p = std::make_pair(b, 0);
            b = false;
            return p;
        });
        CHECK(!lz::empty(generator));
        CHECK(lz::has_one(generator));
        CHECK(!lz::has_many(generator));
    }
}

TEST_CASE("Generate while binary operations", "[Generate while][Binary ops]") {
    auto generator = lz::generate_while(
        [](int& i) {
            auto copy = i++;
            return std::make_pair(copy != 4, copy);
        },
        0);
    static_assert(std::is_same<decltype(*generator.begin()), int>::value,
                  "int and decltype(*generator.begin()) are not the same");
    REQUIRE(*generator.begin() == 0);

    SECTION("Operator++") {
        auto begin = generator.begin();
        ++begin;
        CHECK(*begin == 1);
    }

    SECTION("Operator== & Operator!=") {
        auto begin = generator.begin();
        ++begin;
        CHECK(begin != generator.begin());
        ++begin, ++begin, ++begin;
        CHECK(begin == generator.end());
        begin = generator.begin();
        CHECK(*begin == 0);
        while (begin != generator.end()) {
            ++begin;
        }
        CHECK(begin == generator.end());
    }
}

TEST_CASE("Generate while to containers", "[GenerateWhile][To container]") {
    auto generator = lz::generate_while(
        [](int& f) {
            auto copy = f++;
            return std::pair<int, int>{ copy < 4, copy };
        },
        0);

    SECTION("To array") {
        std::array<int, 4> expected = { 0, 1, 2, 3 };
        auto actual = generator.to<std::array<int, 4>>();
        CHECK(expected == actual);
    }

    SECTION("To vector") {
        std::vector<int> expected = { 0, 1, 2, 3 };
        auto actual = generator.to_vector();
        CHECK(expected == actual);
    }

    SECTION("To other container using to<>()") {
        std::list<int> expected = { 0, 1, 2, 3 };
        auto actual = generator.to<std::list<int>>();
        CHECK(expected == actual);
    }

    SECTION("To map") {
        std::map<int, int> expected = { { 0, 0 }, { 1, 1 }, { 2, 2}, { 3, 3 }};
        auto actual = generator.to_map([](int i) { return std::make_pair(i, i); });
        CHECK(actual == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> expected = { { 0, 0 }, { 1, 1 }, { 2, 2}, { 3, 3 }};
        auto actual = generator.to_unordered_map([](int i) { return std::make_pair(i, i); });
        CHECK(actual == expected);
    }
}