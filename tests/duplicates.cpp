#include <Lz/algorithm.hpp>
#include <Lz/c_string.hpp>
#include <Lz/duplicates.hpp>
#include <Lz/reverse.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Equal to different category of iterables") {
    SECTION("With random access iterator") {
        std::vector<int> input{ 1, 2, 2, 3, 4, 4, 5 };
        std::vector<int> expected{ 2, 2, 4, 4 };

        REQUIRE(lz::equal(input | lz::duplicates, expected));
        REQUIRE(lz::equal(input | lz::duplicates | lz::reverse, expected | lz::reverse));

        input = { 1, 2, 3, 4, 5 };
        REQUIRE(lz::empty(input | lz::duplicates));
        REQUIRE(lz::empty(input | lz::duplicates | lz::reverse));

        input = { 1, 1, 1, 1, 1 };
        expected = { 1, 1, 1, 1, 1 };
        REQUIRE(lz::equal(input | lz::duplicates, input));
        REQUIRE(lz::equal(input | lz::duplicates | lz::reverse, input | lz::reverse));

        input = { 1, 1, 2, 3, 3, 4 };
        expected = { 1, 1, 3, 3 };
        REQUIRE(lz::equal(input | lz::duplicates, expected));
        REQUIRE(lz::equal(input | lz::duplicates | lz::reverse, expected | lz::reverse));

        input = { 1, 2, 3, 4, 5, 5 };
        expected = { 5, 5 };
        REQUIRE(lz::equal(input | lz::duplicates, expected));
        REQUIRE(lz::equal(input | lz::duplicates | lz::reverse, expected | lz::reverse));
    }

    SECTION("With bidi access iterator") {
        std::list<int> v{ 1, 2, 2, 3, 4, 4, 5 };
        std::vector<int> expected{ 2, 2, 4, 4 };

        REQUIRE(lz::equal(v | lz::duplicates, expected));
        REQUIRE(lz::equal(v | lz::duplicates | lz::reverse, expected | lz::reverse));

        v = { 1, 2, 3, 4, 5 };
        REQUIRE(lz::empty(v | lz::duplicates));
        REQUIRE(lz::empty(v | lz::duplicates | lz::reverse));

        v = { 1, 1, 1, 1, 1 };
        expected = { 1, 1, 1, 1, 1 };
        REQUIRE(lz::equal(v | lz::duplicates, v));
        REQUIRE(lz::equal(v | lz::duplicates | lz::reverse, v | lz::reverse));

        v = { 1, 1, 2, 3, 3, 4 };
        expected = { 1, 1, 3, 3 };
        REQUIRE(lz::equal(v | lz::duplicates, expected));
        REQUIRE(lz::equal(v | lz::duplicates | lz::reverse, expected | lz::reverse));

        v = { 1, 2, 3, 4, 5, 5 };
        expected = { 5, 5 };
        REQUIRE(lz::equal(v | lz::duplicates, expected));
        REQUIRE(lz::equal(v | lz::duplicates | lz::reverse, expected | lz::reverse));
    }

    SECTION("With (sentinelled) forward only") {
        auto c_str = lz::c_string("abbcdde");
        std::vector<char> expected = { 'b', 'b', 'd', 'd' };
        REQUIRE(lz::equal(c_str | lz::duplicates, expected));

        c_str = lz::c_string("abcdef");
        REQUIRE(lz::empty(c_str | lz::duplicates));

        c_str = lz::c_string("aaaaa");
        expected = { 'a', 'a', 'a', 'a', 'a' };
        REQUIRE(lz::equal(c_str | lz::duplicates, expected));

        c_str = lz::c_string("aabccde");
        expected = { 'a', 'a', 'c', 'c' };
        REQUIRE(lz::equal(c_str | lz::duplicates, expected));

        c_str = lz::c_string("abcdeee");
        expected = { 'e', 'e', 'e' };
        REQUIRE(lz::equal(c_str | lz::duplicates, expected));

        c_str = lz::c_string("aabcdde");
        expected = { 'a', 'a', 'd', 'd' };
        REQUIRE(lz::equal(c_str | lz::duplicates, expected));
    }
}
