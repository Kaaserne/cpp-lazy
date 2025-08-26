#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <Lz/filter.hpp>
#include <Lz/common.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <doctest/doctest.h>
#include <pch.hpp>
#include <Lz/reverse.hpp>
#include <Lz/cached_size.hpp>
#include <Lz/repeat.hpp>

template<class Iterable>
struct bidi_sentinelled : public lz::lazy_view {
    Iterable iterable;

    explicit bidi_sentinelled(Iterable i) : iterable(std::move(i)) {
    }

    lz::iter_t<Iterable> begin() const {
        return iterable.begin();
    }

    lz::default_sentinel_t end() const {
        return lz::default_sentinel;
    }
};

TEST_CASE("Non cached reverse") {
    SUBCASE("Non sentinelled reverse") {
        const std::vector<int> v = { 1, 2, 3, 4, 5 };
        const std::vector<int> expected = { 5, 4, 3, 2, 1 };

        lz::reverse_iterable<const std::vector<int>> reversed = lz::reverse(v);
        REQUIRE(lz::size(reversed) == 5);
        REQUIRE(lz::equal(reversed, expected));

        reversed = v | lz::reverse;
        REQUIRE(lz::equal(reversed, expected));
        REQUIRE(lz::size(reversed) == 5);
    }

    SUBCASE("Sentinelled reverse") {
        auto repeater = lz::repeat(20, 5) | lz::reverse;
        auto expected = { 20, 20, 20, 20, 20 };
        REQUIRE(lz::size(repeater) == 5);
        REQUIRE(lz::equal(repeater, expected));
    }
}

TEST_CASE("Cached reverse") {
    SUBCASE("Sentinelled cached reverse") {
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

        std::vector<int> expected2 = { 20, 20, 20, 20, 20 };
        test_procs::test_operator_plus(r, expected2);
    }

    const std::vector<int> v = { 1, 2, 3, 4, 5 };
    lz::cached_reverse_iterable<const std::vector<int>> reversed = lz::cached_reverse(v);

    SUBCASE("Non sentinelled empty") {
        std::vector<int> empty;
        auto rev = lz::cached_reverse(empty);
        REQUIRE(lz::size(rev) == 0);
        REQUIRE(lz::empty(rev));
        REQUIRE_FALSE(lz::has_many(rev));
        REQUIRE_FALSE(lz::has_one(rev));
    }

    SUBCASE("Non sentinelled one element") {
        std::vector<int> one_element = { 1 };
        auto rev = lz::cached_reverse(one_element);
        REQUIRE(lz::size(rev) == 1);
        REQUIRE_FALSE(lz::empty(rev));
        REQUIRE(lz::has_one(rev));
        REQUIRE_FALSE(lz::has_many(rev));
        auto expected = { 1 };
        REQUIRE(lz::equal(rev, expected));
    }

    SUBCASE("Operator=(default_sentinel_t)") {
        std::list<int> list{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        auto filtered = list | lz::filter([](int i) { return i % 2 == 0; });
        // Make it so that it has a sentinel and is bidirectional
        bidi_sentinelled<decltype(filtered)> t(filtered);
        auto common = lz::cached_reverse(lz::common(t));
        std::vector<int> expected = { 10, 8, 6, 4, 2 };
        REQUIRE(lz::equal(common, expected));
    }

    SUBCASE("Non sentinelled operator== sentinel") {
        REQUIRE(reversed.begin() != lz::default_sentinel);
        REQUIRE(reversed.end() == lz::default_sentinel);
    }

    SUBCASE("Non sentinelled operator--") {
        auto expected = { 5, 4, 3, 2, 1 };
        REQUIRE(lz::equal(reversed, expected));
    }

    SUBCASE("Non sentinelled operator++") {
        auto expected = { 1, 2, 3, 4, 5 };
        REQUIRE(lz::equal(reversed | lz::cached_reverse, expected));
    }

    SUBCASE("Non sentinelled operator+") {
        std::vector<int> expected = { 1, 2, 3, 4, 5 };
        test_procs::test_operator_plus(reversed, expected | lz::reverse);
    }

    SUBCASE("Non sentinelled operator-") {
        test_procs::test_operator_minus(reversed);
    }
}
