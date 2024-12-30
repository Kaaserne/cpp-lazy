#include <Lz/c_string.hpp>
#include <Lz/flatten.hpp>
#include <Lz/iter_tools.hpp>
#include <catch2/catch.hpp>
#include <list>


TEST_CASE("Flatten with sentinels") {
    using c_string = decltype(lz::c_string(""));

    std::forward_list<c_string> array = { lz::c_string("Hello"), lz::c_string(", "), lz::c_string("World"), lz::c_string("!") };
    auto flattened = lz::flatten(array);
    auto str = flattened.to<std::string>();
    CHECK(str == "Hello, World!");

    auto flatten_one = lz::flatten(lz::c_string("hello, world"));
    CHECK(flatten_one.to<std::string>() == "hello, world");

    c_string arr[] = { lz::c_string("Hello"), lz::c_string(", "), lz::c_string("World"), lz::c_string("!") };
    auto flattened_array = lz::flatten(arr);
    CHECK(flattened_array.to<std::string>() == "Hello, World!");
}

TEST_CASE("Should flatten", "[Flatten][Basic functionality]") {
    SECTION("Flatten 1D") {
        std::vector<int> vec = { 1, 2, 3, 4 };
        auto flattened = lz::flatten(vec);
        CHECK(flattened.to_vector() == std::vector<int>{ 1, 2, 3, 4 });
        CHECK(lz::reverse(flattened).to_vector() == std::vector<int>{ 4, 3, 2, 1 });
    }

    SECTION("Flatten 2D") {
        std::vector<std::list<int>> nested = { { 1, 2, 3 }, {}, { 1 }, { 4, 5, 6 }, {} };
        auto flattened = lz::flatten(nested);
        CHECK(flattened.to_vector() == std::vector<int>{ 1, 2, 3, 1, 4, 5, 6 });
        CHECK(lz::reverse(flattened).to_vector() == std::vector<int>{ 6, 5, 4, 1, 3, 2, 1 });
    }

    SECTION("Flatten 3D") {
        std::vector<std::vector<std::vector<int>>> vectors = { { { 1, 2, 3 }, {} }, { { 4, 5 }, { 6 } }, { { 7 }, {} } };

        auto flattened = lz::flatten(vectors);
        CHECK(flattened.to_vector() == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 });
        CHECK(lz::reverse(flattened).to_vector() == std::vector<int>{ 7, 6, 5, 4, 3, 2, 1 });
    }

    SECTION("Should be by ref") {
        std::vector<std::vector<std::vector<int>>> vectors = {
            { { 0 }, { 1, 2, 3 }, {}, { 4 } }, { {} }, { { 5, 6 }, { 7 }, {} }, { {} }, { {} }
        };
        auto flattened = lz::flatten(vectors);
        *flattened.begin() = -382753;
        CHECK(vectors[0][0][0] == -382753);
    }
}

TEST_CASE("Flatten binary operations", "[Flatten][Binary ops]") {
    std::vector<std::list<int>> nested = { { 1, 2, 3 }, {}, { 1 }, { 4, 5, 6 }, {} };
    auto flattened = lz::flatten(nested);

    SECTION("Operator++") {
        auto begin = flattened.begin();
        CHECK(*begin == 1);
        ++begin;
        CHECK(*begin == 2);
        ++begin, ++begin;
        CHECK(*begin == 1);
    }

    SECTION("Operator--") {
        auto begin = flattened.begin();
        ++begin;
        CHECK(*begin == 2);
        --begin;
        CHECK(*begin == 1);
        CHECK(begin == flattened.begin());
        ++begin, ++begin, ++begin;
        CHECK(*begin == 1);
        --begin;
        CHECK(*begin == 3);
    }

    SECTION("Operator== & operator!=") {
        auto begin = flattened.begin();
        CHECK(begin == flattened.begin());
        CHECK(begin != flattened.end());
        begin = flattened.end();
        CHECK(begin == flattened.end());
        CHECK(begin != flattened.begin());
    }

    SECTION("Operator-(Iterator)") {
        // CHECK(flattened.end() - flattened.begin() == 7);
    }
}

TEST_CASE("Flatten to container", "[Flatten][To container]") {
    std::vector<std::vector<int>> vecs = { { 1, 2, 3 }, { 4, 5 }, {}, { 6, 7 } };
    auto flattened = lz::flatten(vecs);

    SECTION("To array") {
        CHECK(flattened.to<std::array<int,7>>() == std::array<int, 7>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To vector") {
        CHECK(flattened.to_vector() == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To other container using to<>()") {
        CHECK(flattened.to<std::list<int>>() == std::list<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To map") {
        std::map<int, int> actual = flattened.to_map([](const int i) { return std::make_pair(i, i); });

        std::map<int, int> expected = {
            std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3), std::make_pair(4, 4),
            std::make_pair(5, 5), std::make_pair(6, 6), std::make_pair(7, 7),
        };

        CHECK(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> actual = flattened.to_unordered_map([](const int i) { return std::make_pair(i, i); });

        std::unordered_map<int, int> expected = {
            std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3), std::make_pair(4, 4),
            std::make_pair(5, 5), std::make_pair(6, 6), std::make_pair(7, 7),
        };

        CHECK(expected == actual);
    }
}