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
