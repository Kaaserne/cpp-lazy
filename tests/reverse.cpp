#include <Lz/algorithm.hpp>
#include <Lz/reverse.hpp>
#include <catch2/catch.hpp>

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

namespace {
template<class ReversedIterable>
void test_operator_minus(const ReversedIterable& rev) {
    auto begin = rev.begin();
    auto end = rev.end();

    for (std::ptrdiff_t i = 0; i < lz::ssize(rev); ++i) {
        INFO("With i = " << i);
        REQUIRE((end - i) - begin == lz::ssize(rev) - i);
        REQUIRE(end - (begin + i) == lz::ssize(rev) - i);
        REQUIRE((begin + i) - end == -(lz::ssize(rev) - i));
        REQUIRE(begin - (end - i) == -(lz::ssize(rev) - i));
    }

    for (std::ptrdiff_t i = 0; i < lz::ssize(rev); ++i) {
        INFO("With i = " << i);
        REQUIRE((end - i) - (begin + i) == lz::ssize(rev) - 2 * i);
        REQUIRE((begin + i) - (end - i) == -(lz::ssize(rev) - 2 * i));
    }
}

template<class ReversedIterable, class ExpectedIterable>
void test_operator_plus(const ReversedIterable& rev, const ExpectedIterable& expected) {
    auto begin = rev.begin();
    auto end = rev.end();

    for (std::ptrdiff_t i = 0; i < lz::ssize(rev) - 1; ++i) {
        INFO("With i = " << i);
        REQUIRE(*(begin + i) == *(expected.begin() + i));
    }
    REQUIRE(begin + lz::ssize(rev) == rev.end());
    for (std::ptrdiff_t i = 1; i <= lz::ssize(rev); ++i) {
        INFO("With i = " << i);
        REQUIRE(*(end - i) == *(expected.end() - i));
    }
    REQUIRE(end - lz::ssize(rev) == rev.begin());

    std::advance(begin, lz::ssize(rev));
    std::advance(end, -lz::ssize(rev));
    REQUIRE(begin + 0 == begin);
    REQUIRE(end + 0 == end);

    for (std::ptrdiff_t i = 0; i < lz::ssize(rev) - 1; ++i) {
        INFO("With i = " << i);
        REQUIRE(*(end + i) == *(expected.begin() + i));
    }
    REQUIRE(end + lz::ssize(rev) == rev.end());
    for (std::ptrdiff_t i = 1; i <= lz::ssize(rev); ++i) {
        INFO("With i = " << i);
        REQUIRE(*(begin - i) == *(expected.end() - i));
    }
    REQUIRE(begin - lz::ssize(rev) == rev.begin());
}

} // namespace

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
        test_operator_plus(reversed, expected | lz::reverse);
    }

    SECTION("Operator-") {
        test_operator_minus(reversed);
    }
}
