#include <Lz/c_string.hpp>
#include <Lz/flatten.hpp>
#include <Lz/iter_tools.hpp>
#include <catch2/catch.hpp>
#include <forward_list>
#include <list>

TEST_CASE("Flatten with sentinels") {
    using c_string = decltype(lz::c_string(""));

    std::forward_list<c_string> array = { lz::c_string("Hello"), lz::c_string(", "), lz::c_string("World"), lz::c_string("!") };
    auto flattened = lz::flatten(array);
    auto str = flattened.to<std::string>();
    REQUIRE(str == "Hello, World!");

    auto flatten_one = lz::flatten(lz::c_string("hello, world"));
    REQUIRE(flatten_one.to<std::string>() == "hello, world");

    c_string arr[] = { lz::c_string("Hello"), lz::c_string(", "), lz::c_string("World"), lz::c_string("!") };
    auto flattened_array = lz::flatten(arr);
    REQUIRE(flattened_array.to<std::string>() == "Hello, World!");
}

TEST_CASE("Empty or one element flatten") {
    SECTION("Empty") {
        std::vector<std::vector<int>> empty;
        auto empty_flattened = lz::flatten(empty);
        REQUIRE(lz::empty(empty_flattened));
        REQUIRE(!lz::has_one(empty_flattened));
        REQUIRE(!lz::has_many(empty_flattened));
    }

    SECTION("One element") {
        std::vector<std::vector<int>> one_element = { { 1 } };
        auto one_elm_flattened = lz::flatten(one_element);
        REQUIRE(!lz::empty(one_elm_flattened));
        REQUIRE(lz::has_one(one_elm_flattened));
        REQUIRE(!lz::has_many(one_elm_flattened));
    }
}

TEST_CASE("Should flatten", "[Flatten][Basic functionality]") {
    SECTION("Flatten 1D") {
        std::vector<int> vec = { 1, 2, 3, 4 };
        auto flattened = lz::flatten(vec);
        REQUIRE(flattened.to_vector() == std::vector<int>{ 1, 2, 3, 4 });
        REQUIRE(lz::reverse(flattened).to_vector() == std::vector<int>{ 4, 3, 2, 1 });
    }

    SECTION("Flatten 2D") {
        std::vector<std::list<int>> nested = { { 1, 2, 3 }, {}, { 1 }, { 4, 5, 6 }, {} };
        auto flattened = lz::flatten(nested);
        REQUIRE(flattened.to_vector() == std::vector<int>{ 1, 2, 3, 1, 4, 5, 6 });
        REQUIRE(lz::reverse(flattened).to_vector() == std::vector<int>{ 6, 5, 4, 1, 3, 2, 1 });
    }

    SECTION("Flatten 3D") {
        std::vector<std::vector<std::vector<int>>> vectors = { { { 1, 2, 3 }, {} }, { { 4, 5 }, { 6 } }, { { 7 }, {} } };

        auto flattened = lz::flatten(vectors);
        REQUIRE(flattened.to_vector() == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 });
        REQUIRE(lz::reverse(flattened).to_vector() == std::vector<int>{ 7, 6, 5, 4, 3, 2, 1 });
    }

    SECTION("Should be by ref") {
        std::vector<std::vector<std::vector<int>>> vectors = {
            { { 0 }, { 1, 2, 3 }, {}, { 4 } }, { {} }, { { 5, 6 }, { 7 }, {} }, { {} }, { {} }
        };
        auto flattened = lz::flatten(vectors);
        *flattened.begin() = -382753;
        REQUIRE(vectors[0][0][0] == -382753);
    }
}

TEST_CASE("Flatten binary operations", "[Flatten][Binary ops]") {
    std::vector<std::list<int>> nested = { { 1, 2, 3 }, {}, { 1 }, { 4, 5, 6 }, {} };
    auto flattened = lz::flatten(nested);

    SECTION("Operator++") {
        auto begin = flattened.begin();
        REQUIRE(*begin == 1);
        ++begin;
        REQUIRE(*begin == 2);
        ++begin, ++begin;
        REQUIRE(*begin == 1);
    }

    SECTION("Operator--") {
        auto begin = flattened.begin();
        ++begin;
        REQUIRE(*begin == 2);
        --begin;
        REQUIRE(*begin == 1);
        REQUIRE(begin == flattened.begin());
        ++begin, ++begin, ++begin;
        REQUIRE(*begin == 1);
        --begin;
        REQUIRE(*begin == 3);
    }

    SECTION("Operator== & operator!=") {
        auto begin = flattened.begin();
        REQUIRE(begin == flattened.begin());
        REQUIRE(begin != flattened.end());
        begin = flattened.end();
        REQUIRE(begin == flattened.end());
        REQUIRE(begin != flattened.begin());
    }

    SECTION("Operator-(Iterator)") {
        // REQUIRE(flattened.end() - flattened.begin() == 7);
    }
}

TEST_CASE("Flatten to container", "[Flatten][To container]") {
    std::vector<std::vector<int>> vecs = { { 1, 2, 3 }, { 4, 5 }, {}, { 6, 7 } };
    auto flattened = lz::flatten(vecs);

    SECTION("To array") {
        REQUIRE(flattened.to<std::array<int, 7>>() == std::array<int, 7>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To vector") {
        REQUIRE(flattened.to_vector() == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To other container using to<>()") {
        REQUIRE(flattened.to<std::list<int>>() == std::list<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To map") {
        std::map<int, int> actual = flattened.to_map([](const int i) { return std::make_pair(i, i); });

        std::map<int, int> expected = {
            std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3), std::make_pair(4, 4),
            std::make_pair(5, 5), std::make_pair(6, 6), std::make_pair(7, 7),
        };

        REQUIRE(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> actual = flattened.to_unordered_map([](const int i) { return std::make_pair(i, i); });

        std::unordered_map<int, int> expected = {
            std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3), std::make_pair(4, 4),
            std::make_pair(5, 5), std::make_pair(6, 6), std::make_pair(7, 7),
        };

        REQUIRE(expected == actual);
    }
}