#include <Lz/algorithm.hpp>
#include <Lz/reverse.hpp>
#include <catch2/catch.hpp>
#include <test_procs.hpp>

TEST_CASE("Reverse basic test") {
    const std::vector<int> v = { 1, 2, 3, 4, 5 };
    const std::vector<int> expected = { 5, 4, 3, 2, 1 };

    lz::reverse_iterable<const std::vector<int>> reversed = lz::reverse(v);
    REQUIRE(lz::size(reversed) == 5);
    REQUIRE(lz::equal(reversed, expected));

    reversed = v | lz::reverse;
    REQUIRE(lz::equal(reversed, expected));
    REQUIRE(lz::size(reversed) == 5);
}

TEST_CASE("Cached reverse") {
    const std::vector<int> v = { 1, 2, 3, 4, 5 };
    lz::cached_reverse_iterable<const std::vector<int>> reversed = lz::cached_reverse(v);

    SECTION("Reverse empty") {
        std::vector<int> empty;
        auto rev = lz::cached_reverse(empty);
        REQUIRE(lz::size(rev) == 0);
        REQUIRE(lz::empty(rev));
        REQUIRE(!lz::has_many(rev));
        REQUIRE(!lz::has_one(rev));
    }

    SECTION("Opertor== sentinel") {
        REQUIRE(reversed.begin() != lz::default_sentinel{});
        REQUIRE(reversed.end() == lz::default_sentinel{});
    }
    
    SECTION("Operator--") {
        auto expected = { 5, 4, 3, 2, 1 };
        REQUIRE(lz::equal(reversed, expected));
    }

    SECTION("Operator++") {
        auto expected = { 1, 2, 3, 4, 5 };
        REQUIRE(lz::equal(reversed | lz::cached_reverse, expected));
    }

    SECTION("Operator+") {
        std::vector<int> expected = { 1, 2, 3, 4, 5 };
        test_procs::test_operator_plus(reversed, expected | lz::reverse);
    }

    SECTION("Operator-") {
        test_procs::test_operator_minus(reversed);
    }
}
