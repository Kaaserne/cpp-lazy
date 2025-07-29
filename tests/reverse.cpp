#include <Lz/algorithm.hpp>
#include <Lz/common.hpp>
#include <Lz/reverse.hpp>
#include <catch2/catch.hpp>
#include <cpp-lazy-ut-helper/repeat.hpp>
#include <test_procs.hpp>

TEST_CASE("Non cached reverse") {
    SECTION("Non sentinelled reverse") {
        const std::vector<int> v = { 1, 2, 3, 4, 5 };
        const std::vector<int> expected = { 5, 4, 3, 2, 1 };

        lz::reverse_iterable<const std::vector<int>> reversed = lz::reverse(v);
        REQUIRE(lz::size(reversed) == 5);
        REQUIRE(lz::equal(reversed, expected));

        reversed = v | lz::reverse;
        REQUIRE(lz::equal(reversed, expected));
        REQUIRE(lz::size(reversed) == 5);
    }

    SECTION("Sentinelled reverse") {
        auto repeater = lz::repeat(20, 5) | lz::reverse;
        auto expected = { 20, 20, 20, 20, 20 };
        REQUIRE(lz::size(repeater) == 5);
        REQUIRE(lz::equal(repeater, expected));
    }
}

TEST_CASE("Cached reverse") {
    SECTION("Sentinelled cached reverse") {
        auto repeater = lz::repeat(20, 5);
        auto r = lz::cached_reverse(repeater);

        auto expected = { 20, 20, 20, 20, 20 };
        REQUIRE(lz::equal(r, expected));

        auto it = r.begin();
        it = lz::default_sentinel;

        REQUIRE(it == lz::default_sentinel);
        REQUIRE(lz::default_sentinel == it);

        REQUIRE(it != r.begin());
        REQUIRE(r.begin() != it);
        REQUIRE(it == r.end());
        REQUIRE(r.end() == it);

        test_procs::test_operator_minus(r);
    }

    const std::vector<int> v = { 1, 2, 3, 4, 5 };
    lz::cached_reverse_iterable<const std::vector<int>> reversed = lz::cached_reverse(v);

    SECTION("Non sentinelled empty") {
        std::vector<int> empty;
        auto rev = lz::cached_reverse(empty);
        REQUIRE(lz::size(rev) == 0);
        REQUIRE(lz::empty(rev));
        REQUIRE_FALSE(lz::has_many(rev));
        REQUIRE_FALSE(lz::has_one(rev));
    }

    SECTION("Non sentinelled one element") {
        std::vector<int> one_element = { 1 };
        auto rev = lz::cached_reverse(one_element);
        REQUIRE(lz::size(rev) == 1);
        REQUIRE_FALSE(lz::empty(rev));
        REQUIRE(lz::has_one(rev));
        REQUIRE_FALSE(lz::has_many(rev));
        auto expected = { 1 };
        REQUIRE(lz::equal(rev, expected));
    }

    SECTION("Non sentinelled operator== sentinel") {
        REQUIRE(reversed.begin() != lz::default_sentinel);
        REQUIRE(reversed.end() == lz::default_sentinel);
    }

    SECTION("Non sentinelled operator--") {
        auto expected = { 5, 4, 3, 2, 1 };
        REQUIRE(lz::equal(reversed, expected));
    }

    SECTION("Non sentinelled operator++") {
        auto expected = { 1, 2, 3, 4, 5 };
        REQUIRE(lz::equal(reversed | lz::cached_reverse, expected));
    }

    SECTION("Non sentinelled operator+") {
        std::vector<int> expected = { 1, 2, 3, 4, 5 };
        test_procs::test_operator_plus(reversed, expected | lz::reverse);
    }

    SECTION("Non sentinelled operator-") {
        test_procs::test_operator_minus(reversed);
    }
}
