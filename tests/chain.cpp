#include <Lz/lz.hpp>
#include <algorithm>
#include <catch2/catch.hpp>
#include <cctype>

template class lz::chain_iterable<decltype(std::declval<std::vector<int>&>().begin())>;

// TODO fix this
TEST_CASE("Iterator chaining") {
}
