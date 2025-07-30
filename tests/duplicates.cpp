#include <Lz/algorithm.hpp>
#include <Lz/duplicates.hpp>
#include <Lz/reverse.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <list>
#include <vector>

struct equal_pair {
    template<class T1, class T2>
    constexpr bool operator()(const std::pair<T1, std::size_t>& lhs, const std::pair<T2, std::size_t>& rhs) const {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

TEST_CASE("Equal to different category of iterables") {
    SECTION("With random access iterator") {
        std::vector<int> input{ 1, 2, 2, 3, 4, 4, 5 };
        std::vector<std::pair<int, std::size_t>> expected = { { 1, 1 }, { 2, 2 }, { 3, 1 }, { 4, 2 }, { 5, 1 } };

        lz::duplicates_iterable<std::vector<int>> dupes = lz::duplicates(input);
        REQUIRE(lz::equal(dupes, expected, equal_pair{}));
        REQUIRE(lz::equal(input | lz::duplicates | lz::reverse, expected | lz::reverse, equal_pair{}));

        input = { 1, 2, 3, 4, 5 };
        expected = { { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 } };
        REQUIRE(lz::equal(input | lz::duplicates, expected, equal_pair{}));
        REQUIRE(lz::equal(input | lz::duplicates | lz::reverse, expected | lz::reverse, equal_pair{}));

        input = { 1, 1, 1, 1, 1 };
        expected = { { 1, 5 } };
        REQUIRE(lz::equal(input | lz::duplicates, expected, equal_pair{}));
        REQUIRE(lz::equal(input | lz::duplicates | lz::reverse, expected | lz::reverse, equal_pair{}));

        input = { 1, 1, 2, 3, 3, 4 };
        expected = { { 1, 2 }, { 2, 1 }, { 3, 2 }, { 4, 1 } };
        REQUIRE(lz::equal(input | lz::duplicates, expected, equal_pair{}));
        REQUIRE(lz::equal(input | lz::duplicates | lz::reverse, expected | lz::reverse, equal_pair{}));

        input = { 1, 2, 3, 4, 5, 5 };
        expected = { { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 2 } };
        REQUIRE(lz::equal(input | lz::duplicates, expected, equal_pair{}));
        REQUIRE(lz::equal(input | lz::duplicates | lz::reverse, expected | lz::reverse, equal_pair{}));
    }

    SECTION("With bidi access iterator") {
        std::list<int> input{ 1, 2, 2, 3, 4, 4, 5 };
        std::vector<std::pair<int, std::size_t>> expected = { { 1, 1 }, { 2, 2 }, { 3, 1 }, { 4, 2 }, { 5, 1 } };

        REQUIRE(lz::equal(input | lz::duplicates, expected, equal_pair{}));
        REQUIRE(lz::equal(input | lz::duplicates | lz::reverse, expected | lz::reverse, equal_pair{}));

        input = { 1, 2, 3, 4, 5 };
        expected = { { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 } };
        REQUIRE(lz::equal(input | lz::duplicates, expected, equal_pair{}));
        REQUIRE(lz::equal(input | lz::duplicates | lz::reverse, expected | lz::reverse, equal_pair{}));

        input = { 1, 1, 1, 1, 1 };
        expected = { { 1, 5 } };
        REQUIRE(lz::equal(input | lz::duplicates, expected, equal_pair{}));
        REQUIRE(lz::equal(input | lz::duplicates | lz::reverse, expected | lz::reverse, equal_pair{}));

        input = { 1, 1, 2, 3, 3, 4 };
        expected = { { 1, 2 }, { 2, 1 }, { 3, 2 }, { 4, 1 } };
        REQUIRE(lz::equal(input | lz::duplicates, expected, equal_pair{}));
        REQUIRE(lz::equal(input | lz::duplicates | lz::reverse, expected | lz::reverse, equal_pair{}));

        input = { 1, 2, 3, 4, 5, 5 };
        expected = { { 1, 1 }, { 2, 1 }, { 3, 1 }, { 4, 1 }, { 5, 2 } };
        REQUIRE(lz::equal(input | lz::duplicates, expected, equal_pair{}));
        REQUIRE(lz::equal(input | lz::duplicates | lz::reverse, expected | lz::reverse, equal_pair{}));
    }

    SECTION("With (sentinelled) forward only") {
        auto input = lz::c_string("abbcdde");
        std::vector<std::pair<char, std::size_t>> expected = { { 'a', 1 }, { 'b', 2 }, { 'c', 1 }, { 'd', 2 }, { 'e', 1 } };

        REQUIRE(lz::equal(input | lz::duplicates, expected, equal_pair{}));

        input = lz::c_string("abcde");
        expected = { { 'a', 1 }, { 'b', 1 }, { 'c', 1 }, { 'd', 1 }, { 'e', 1 } };
        REQUIRE(lz::equal(input | lz::duplicates, expected, equal_pair{}));

        input = lz::c_string("aaaaa");
        expected = { { 'a', 5 } };
        REQUIRE(lz::equal(input | lz::duplicates, expected, equal_pair{}));

        input = lz::c_string("aabccd");
        expected = { { 'a', 2 }, { 'b', 1 }, { 'c', 2 }, { 'd', 1 } };
        REQUIRE(lz::equal(input | lz::duplicates, expected, equal_pair{}));

        input = lz::c_string("abcdee");
        expected = { { 'a', 1 }, { 'b', 1 }, { 'c', 1 }, { 'd', 1 }, { 'e', 2 } };
        REQUIRE(lz::equal(input | lz::duplicates, expected, equal_pair{}));
    }
}
