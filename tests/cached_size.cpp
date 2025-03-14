#include <Catch2/catch.hpp>
#include <Lz/cached_size.hpp>
#include <Lz/chunks.hpp>
#include <Lz/enumerate.hpp>
#include <Lz/filter.hpp>
#include <Lz/range.hpp>

TEST_CASE("Correct size") {
    auto to_filter = lz::range(10);
    auto filtered = to_filter | lz::filter([](int i) { return i % 2 == 0; });
    static_assert(!lz::sized<decltype(filtered)>::value, "Filtered should not be sized");
    auto cached = filtered | lz::cache_size;

    REQUIRE(cached.size() == 5);
    auto expected = { 0, 2, 4, 6, 8 };
    REQUIRE(lz::equal(cached, expected));
}

TEST_CASE("Correct size i.c.m. with other iterators") {
    auto to_filter = lz::range(10);
    auto filtered = to_filter | lz::filter([](int i) { return i % 2 == 0; });

    auto iterable = filtered | lz::cache_size | lz::chunks(3) | lz::enumerate;

    REQUIRE(iterable.size() == 2);
    std::vector<std::pair<std::size_t, std::vector<int>>> expected{ { 0, { 0, 2, 4 } }, { 1, { 6, 8 } } };
    REQUIRE(lz::equal(iterable, expected));
}
