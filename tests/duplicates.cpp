#include <Lz/c_string.hpp>

#include <Lz/duplicates.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

struct equal_pair {
    template<class T1, class T2>
    constexpr bool operator()(const std::pair<T1, std::size_t>& lhs, const std::pair<T2, std::size_t>& rhs) const {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

TEST_CASE("Equal to different category of iterables") {
    SUBCASE("With random access iterator") {
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

    SUBCASE("With bidi access iterator") {
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

    SUBCASE("With (sentinelled) forward only") {
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

TEST_CASE("Duplicates operator=(default_sentinel_t)") {
    SUBCASE("forward") {
        std::forward_list<int> fwd = { 1, 1, 2, 2, 3, 4, 4 };
        auto duplicated = lz::duplicates(fwd);

        auto common = make_sentinel_assign_op_tester(duplicated);

        std::vector<std::pair<int, std::size_t>> expected = { std::make_pair(1, 2), std::make_pair(2, 2), std::make_pair(3, 1),
                                                              std::make_pair(4, 2) };
        using reference = decltype(*common.begin());
        REQUIRE(lz::equal(common, expected, [](reference a, const std::pair<int, size_t>& b) {
            return a.first == b.first && a.second == b.second;
        }));
    }

    SUBCASE("bidirectional") {
        std::vector<int> vec = { 10, 10, 20, 20, 20, 30 };
        auto common2 = make_sentinel_assign_op_tester(lz::duplicates(make_sized_bidi_sentinelled(vec)));
        std::vector<std::pair<int, std::size_t>> expected2 = { std::make_pair(10, 2), std::make_pair(20, 3),
                                                               std::make_pair(30, 1) };
        using reference2 = decltype(*common2.begin());
        REQUIRE(lz::equal(common2, expected2, [](reference2 a, const std::pair<int, size_t>& b) {
            return a.first == b.first && a.second == b.second;
        }));
        REQUIRE(lz::equal(lz::reverse(common2), lz::reverse(expected2), [](reference2 a, const std::pair<int, size_t>& b) {
            return a.first == b.first && a.second == b.second;
        }));
    }
}
