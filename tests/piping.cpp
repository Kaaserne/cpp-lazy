#include <Lz/algorithm/equal.hpp>
#include <Lz/chunk_if.hpp>
#include <Lz/drop.hpp>
#include <Lz/filter.hpp>
#include <Lz/map.hpp>
#include <Lz/procs/to.hpp>
#include <Lz/range.hpp>
#include <Lz/stream.hpp>
#include <Lz/take.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <doctest/doctest.h>

TEST_CASE("Iterator chaining") {
    SUBCASE("test 1") {
        unsigned dummy = 0;
        auto iterable = lz::range(unsigned(10)) | lz::map([dummy](unsigned i) mutable {
                            ++dummy;
                            return std::to_string(i);
                        }) |
                        lz::take(5) | lz::drop(1) | lz::filter([](const std::string& str) { return std::isdigit(str[0]); });
        std::array<std::string, 4> expected = { "1", "2", "3", "4" };
        REQUIRE(lz::equal(iterable, expected));
    }

    SUBCASE("test 2") {
        unsigned dummy = 0;
        auto chunk_iterable = lz::range(unsigned(10)) | lz::chunk_if([dummy](unsigned i) mutable {
                                  ++dummy;
                                  return i % 2 == 0;
                              });
        using chunk_type = typename decltype(chunk_iterable.begin())::reference;
        auto mapped_chunk = chunk_iterable | lz::map([](chunk_type chunk) { return lz::format(chunk); }) | lz::take(3);
        std::vector<std::string> expected = { "", "1", "3" };
        REQUIRE(lz::equal(mapped_chunk, expected));
    }

    SUBCASE("test 3, with to<>") {
        unsigned dummy = 0;
        auto iterable = lz::range(unsigned(10)) | lz::map([dummy](unsigned i) mutable {
                            ++dummy;
                            return std::to_string(i);
                        }) |
                        lz::take(5) | lz::drop(1) | lz::filter([](const std::string& str) { return std::isdigit(str[0]); }) |
                        lz::to<std::vector>();
        std::array<std::string, 4> expected = { "1", "2", "3", "4" };
        REQUIRE(lz::equal(iterable, expected));
    }

    SUBCASE("test 4, with to<>") {
        auto chunk_iterable = lz::range(unsigned(10)) | lz::chunk_if([](unsigned i) { return i % 2 == 0; });
        unsigned dummy = 0;

        using chunk_type = typename decltype(chunk_iterable.begin())::reference;
        auto mapped_chunk = chunk_iterable | lz::map([dummy](chunk_type chunk) mutable {
                                ++dummy;
                                return lz::format(chunk);
                            }) |
                            lz::take(3) | lz::to<std::vector>();
        std::vector<std::string> expected = { "", "1", "3" };
        REQUIRE(lz::equal(mapped_chunk, expected));
    }
}
