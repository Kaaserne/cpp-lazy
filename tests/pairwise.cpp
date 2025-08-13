#include <Lz/algorithm.hpp>
#include <Lz/pairwise.hpp>
#include <Lz/range.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <doctest/doctest.h>
#include <list>
#include <vector>

TEST_CASE("Empty or one element") {
    SUBCASE("Empty vector") {
        std::vector<int> vec;
        auto it = lz::pairwise(vec, 2);
        REQUIRE(lz::size(it) == 0);
        REQUIRE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE_FALSE(lz::has_one(it));
        REQUIRE(lz::distance(it) == 0); // TODO add lz::distance everywhere for empty
    }

    SUBCASE("Single element vector 1") {
        std::vector<int> vec{ 1 };
        auto it = lz::pairwise(vec, 1);
        static_assert(!lz::detail::has_sentinel<decltype(vec)>::value, "");
        REQUIRE(lz::size(it) == 1);
        REQUIRE(lz::has_one(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE_FALSE(lz::empty(it));
        auto expected = { 1 };
        REQUIRE(lz::equal(*it.begin(), expected));
        REQUIRE(lz::distance(it) == 1);
    }

    SUBCASE("Single element vector 2") {
        std::vector<int> vec{ 1 };
        auto it = lz::pairwise(vec, 2);
        REQUIRE(lz::size(it) == 0);
        REQUIRE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE_FALSE(lz::has_one(it));
        REQUIRE(lz::distance(it) == 0);
    }

    SUBCASE("Two elements vector 2") {
        std::vector<int> vec{ 1, 2 };
        auto it = lz::pairwise(vec, 2);
        REQUIRE(lz::size(it) == 1);
        REQUIRE_FALSE(lz::empty(it));
        REQUIRE_FALSE(lz::has_many(it));
        REQUIRE(lz::has_one(it));
        auto expected = { 1, 2 };
        REQUIRE(lz::equal(*it.begin(), expected));
        REQUIRE(lz::distance(it) == 1);
    }

    // SUBCASE("Forward non sized sentinelled") {
    //     auto cstr = lz::c_string("");
    //     auto it = lz::pairwise(cstr, 2);
    //     static_assert(lz::detail::has_sentinel<decltype(cstr)>::value, "");
    //     REQUIRE(lz::empty(it));
    //     REQUIRE_FALSE(lz::has_many(it));
    //     REQUIRE_FALSE(lz::has_one(it));
    // }

    // SUBCASE("Forward non sized sentinelled 1") {
    //     auto cstr = lz::c_string("a");
    //     auto it = lz::pairwise(cstr, 1);
    //     REQUIRE_FALSE(lz::empty(it));
    //     REQUIRE_FALSE(lz::has_many(it));
    //     REQUIRE(lz::has_one(it));
    //     auto expected = { 'a' };
    //     REQUIRE(lz::equal(*it.begin(), expected));
    // }

    // SUBCASE("Forward non sized sentinelled 2") {
    //     auto cstr = lz::c_string("a");
    //     auto it = lz::pairwise(cstr, 2);
    //     REQUIRE(lz::empty(it));
    //     REQUIRE_FALSE(lz::has_many(it));
    //     REQUIRE_FALSE(lz::has_one(it));
    // }

    // SUBCASE("Two elements forward non sized sentinelled") {
    //     auto cstr = lz::c_string("ab");
    //     auto it = lz::pairwise(cstr, 2);
    //     REQUIRE_FALSE(lz::empty(it));
    //     REQUIRE_FALSE(lz::has_many(it));
    //     REQUIRE(lz::has_one(it));
    //     auto expected = { 'a', 'b' };
    //     REQUIRE(lz::equal(*it.begin(), expected));
    // }

    // SUBCASE("Empty bidi iterable") {
    //     std::list<int> lst;
    //     auto it = lz::pairwise(lst, 2);
    //     REQUIRE(lz::size(it) == 0);
    //     REQUIRE(lz::empty(it));
    //     REQUIRE_FALSE(lz::has_many(it));
    //     REQUIRE_FALSE(lz::has_one(it));
    // }

    // SUBCASE("Single element bidi iterable 1") {
    //     std::list<int> lst{ 1 };
    //     auto it = lz::pairwise(lst, 1);
    //     REQUIRE(lz::size(it) == 1);
    //     REQUIRE(lz::has_one(it));
    //     REQUIRE_FALSE(lz::has_many(it));
    //     REQUIRE_FALSE(lz::empty(it));
    //     auto expected = { 1 };
    //     REQUIRE(lz::equal(*it.begin(), expected));
    // }

    // SUBCASE("Single element bidi iterable 2") {
    //     std::list<int> lst{ 1 };
    //     auto it = lz::pairwise(lst, 2);
    //     REQUIRE(lz::size(it) == 0);
    //     REQUIRE(lz::empty(it));
    //     REQUIRE_FALSE(lz::has_many(it));
    //     REQUIRE_FALSE(lz::has_one(it));
    // }

    // SUBCASE("Two elements bidi iterable 2") {
    //     std::list<int> lst{ 1, 2 };
    //     auto it = lz::pairwise(lst, 2);
    //     REQUIRE(lz::size(it) == 1);
    //     REQUIRE_FALSE(lz::empty(it));
    //     REQUIRE_FALSE(lz::has_many(it));
    //     REQUIRE(lz::has_one(it));
    //     auto expected = { 1, 2 };
    //     REQUIRE(lz::equal(*it.begin(), expected));
    // }
}

TEST_CASE("pairwise non sentinelled iterator random access") {
    std::vector<int> vec{ 1, 2, 3, 4, 5 };

    SUBCASE("Forward") {
        std::vector<std::vector<int>> expected = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 } };
        auto it = lz::pairwise(vec, 1);
        using value_type = typename decltype(it.begin())::value_type;

        REQUIRE(lz::size(it) == vec.size());
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 5 } };
        it = lz::pairwise(vec, 2);
        REQUIRE(lz::size(it) == vec.size() - 1);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2, 3 }, { 2, 3, 4 }, { 3, 4, 5 } };
        it = vec | lz::pairwise(3);
        REQUIRE(lz::size(it) == vec.size() - 2);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2, 3, 4 }, { 2, 3, 4, 5 } };
        it = vec | lz::pairwise(4);
        REQUIRE(lz::size(it) == vec.size() - 3);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2, 3, 4, 5 } };
        it = vec | lz::pairwise(5);
        REQUIRE(lz::size(it) == vec.size() - 4);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));
    }

    SUBCASE("Backward") {
        std::vector<std::vector<int>> expected = { { 5 }, { 4 }, { 3 }, { 2 }, { 1 } };
        auto it = lz::pairwise(vec, 1) | lz::reverse;
        using value_type = typename decltype(it.begin())::value_type;

        REQUIRE(lz::size(it) == vec.size());
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 4, 5 }, { 3, 4 }, { 2, 3 }, { 1, 2 } };
        it = lz::pairwise(vec, 2) | lz::reverse;
        REQUIRE(lz::size(it) == vec.size() - 1);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 3, 4, 5 }, { 2, 3, 4 }, { 1, 2, 3 } };
        it = vec | lz::pairwise(3) | lz::reverse;
        REQUIRE(lz::size(it) == vec.size() - 2);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 2, 3, 4, 5 }, { 1, 2, 3, 4 } };
        it = vec | lz::pairwise(4) | lz::reverse;
        REQUIRE(lz::size(it) == vec.size() - 3);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 2, 3, 4, 5 } };
        it = vec | lz::pairwise(5) | lz::reverse;
        REQUIRE(lz::size(it) == vec.size() - 4);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));
    }
}

TEST_CASE("pairwise sentinelled iterator random access") {
    auto repeater = lz::repeat(1, 5);

    SUBCASE("Forward") {
        std::vector<std::vector<int>> expected = { { 1 }, { 1 }, { 1 }, { 1 }, { 1 } };
        auto it = lz::pairwise(repeater, 1);
        using value_type = typename decltype(it.begin())::value_type;

        REQUIRE(lz::size(it) == repeater.size());
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 } };
        it = lz::pairwise(repeater, 2);
        REQUIRE(lz::size(it) == repeater.size() - 1);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } };
        it = repeater | lz::pairwise(3);
        REQUIRE(lz::size(it) == repeater.size() - 2);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 1, 1, 1 }, { 1, 1, 1, 1 } };
        it = repeater | lz::pairwise(4);
        REQUIRE(lz::size(it) == repeater.size() - 3);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

        expected = { { 1, 1, 1, 1, 1 } };
        it = repeater | lz::pairwise(5);
        REQUIRE(lz::size(it) == repeater.size() - 4);
        REQUIRE(lz::equal(it, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));
    }
}
