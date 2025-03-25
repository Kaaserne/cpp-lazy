#include <Lz/lz.hpp>
#include <algorithm>
#include <array>
#include <catch2/catch.hpp>
#include <cctype>


TEST_CASE("Iterator chaining") {
    SECTION("test 1") {
        int dummy = 0;
        auto iterable = lz::range(10) | lz::map([dummy](int i) mutable {
                            ++dummy;
                            return std::to_string(i);
                        }) |
                        lz::take(5) | lz::drop(1) | lz::filter([](const std::string& str) { return std::isdigit(str[0]); });
        std::array<std::string, 4> expected = { "1", "2", "3", "4" };
        REQUIRE(lz::equal(iterable, expected));
    }

    SECTION("test 2") {
        int dummy = 0;
        auto chunk_iterable = lz::range(10) | lz::chunk_if([dummy](int i) mutable {
                                  ++dummy;
                                  return i % 2 == 0;
                              });
        using chunk_type = decltype(chunk_iterable.begin())::reference;
        auto mapped_chunk = chunk_iterable | lz::map([](chunk_type chunk) { return lz::format(chunk); }) | lz::take(3);
        std::vector<std::string> expected = { "", "1", "3" };
        REQUIRE(lz::equal(mapped_chunk, expected));
    }

    SECTION("test 3, with to<>") {
        int dummy = 0;
        auto iterable = lz::range(10) | lz::map([dummy](int i) mutable {
                            ++dummy;
                            return std::to_string(i);
                        }) |
                        lz::take(5) | lz::drop(1) | lz::filter([](const std::string& str) { return std::isdigit(str[0]); }) |
                        lz::to<std::vector>();
        std::array<std::string, 4> expected = { "1", "2", "3", "4" };
        REQUIRE(lz::equal(iterable, expected));
    }

    SECTION("test 4, with to<>") {
        auto chunk_iterable = lz::range(10) | lz::chunk_if([](int i) { return i % 2 == 0; });
        int dummy = 0;

        using chunk_type = decltype(chunk_iterable.begin())::reference;
        auto mapped_chunk = chunk_iterable | lz::map([dummy](chunk_type chunk) mutable {
                                ++dummy;
                                return lz::format(chunk);
                            }) |
                            lz::take(3) | lz::to<std::vector>();
        std::vector<std::string> expected = { "", "1", "3" };
        REQUIRE(lz::equal(mapped_chunk, expected));
    }
}
