#include <Lz/c_string.hpp>
#include <Lz/chunks.hpp>
#include <Lz/map.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Chunks changing and creating elements", "[Chunks][Basic functionality]") {
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7 };
    auto chunked = lz::chunks(v, 3);

    SECTION("Should be by ref") {
        auto it = chunked.begin();
        *it->begin() = 0;
        REQUIRE(v[0] == 0);
    }

    SECTION("Should be correct length") {
        REQUIRE(std::distance(chunked.begin(), chunked.end()) == 3);
        CHECK(chunked.size() == static_cast<std::size_t>(std::distance(chunked.begin(), chunked.end())));
    }
}

TEST_CASE("Chunks with sentinels") {
    const char* str = "Hello, world!";
    auto str_view = lz::c_string(str);
    auto chunked = lz::chunks(str_view, 3);
    static_assert(!std::is_same<decltype(chunked.begin()), decltype(chunked.end())>::value, "Must be sentinel");
    REQUIRE(lz::distance(chunked.begin(), chunked.end()) == 5);
    using View = typename decltype(chunked.begin())::value_type;
    std::vector<std::string> expected = { "Hel", "lo,", " wo", "rld", "!" };
    auto mapped = chunked | lz::map([](View view) { return std::string(view.begin(), view.end()); });
    REQUIRE(lz::equal(mapped, expected));
}

TEST_CASE("empty or one element chunks") {
    SECTION("Empty") {
        std::vector<int> vec;
        auto chunked = lz::chunks(vec, 3);
        REQUIRE(lz::empty(chunked));
        REQUIRE(!lz::has_one(chunked));
        REQUIRE(!lz::has_many(chunked));
        REQUIRE(chunked.size() == 0);
    }

    SECTION("One element") {
        std::vector<int> vec = { 1 };
        auto chunked = lz::chunks(vec, 3);
        REQUIRE(!lz::empty(chunked));
        REQUIRE(lz::has_one(chunked));
        REQUIRE(!lz::has_many(chunked));
        REQUIRE(chunked.size() == 1);
    }
}

TEST_CASE("Chunks binary operations", "[Chunks][Binary ops]") {
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto chunked = lz::chunks(v, 3);
    REQUIRE(chunked.size() == static_cast<std::size_t>(lz::distance(chunked.begin(), chunked.end())));

    static_assert(std::is_same<decltype(chunked.begin()), decltype(chunked.end())>::value, "Should not be sentinel");
    auto it = chunked.begin();

    SECTION("Operator++") {
        REQUIRE(*it->begin() == 1);
        REQUIRE(*it->end() == 4);
        ++it;
        REQUIRE(*it->begin() == 4);
        REQUIRE(*it->end() == 7);
        ++it;
        REQUIRE(*it->begin() == 7);
        REQUIRE(it->end() == chunked.end()->end());
    }

    SECTION("Operator++ (bidirectional)") {
        std::list<int> lst = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto c = lz::chunks(lst, 3);
        auto iter = c.begin();
        REQUIRE(*iter->begin() == 1);
        REQUIRE(*iter->end() == 4);
        ++iter;
        REQUIRE(*iter->begin() == 4);
        REQUIRE(*iter->end() == 7);
        ++iter;
        REQUIRE(*iter->begin() == 7);
        REQUIRE(iter->end() == c.end()->end());
    }

    SECTION("Operator--") {
        auto end = chunked.end();
        --end;
        REQUIRE(*end->begin() == 6);
        REQUIRE(end->end() == chunked.end()->end());
        --end;
        REQUIRE(*end->begin() == 3);
        REQUIRE(*end->end() == 6);
        --end;
        REQUIRE(*end->begin() == 1);
        REQUIRE(*end->end() == 3);
    }

    SECTION("Operator-- (bidirectional)") {
        std::list<int> lst = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto c = lst | lz::chunks(3);
        auto iter = c.end();
        --iter;
        REQUIRE(*iter->begin() == 6);
        REQUIRE(iter->end() == c.end()->end());
        --iter;
        REQUIRE(*iter->begin() == 3);
        REQUIRE(*iter->end() == 6);
        --iter;
        REQUIRE(*iter->begin() == 1);
        REQUIRE(*iter->end() == 3);
    }

    SECTION("Operator== & operator!=") {
        REQUIRE(it == chunked.begin());
        REQUIRE(it != chunked.end());
        it = chunked.end();
        REQUIRE(it != chunked.begin());
        REQUIRE(it == chunked.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        auto beg = chunked.begin();
        auto end = chunked.end();

        REQUIRE(*(beg + 1)->begin() == 4);
        REQUIRE(*(beg + 2)->begin() == 7);
        REQUIRE((beg + 3)->begin() == chunked.end()->end());
        REQUIRE(*(beg + 1)->end() == 7);
        REQUIRE((beg + 2)->end() == chunked.end()->end());
        REQUIRE((beg + 3)->end() == chunked.end()->end());

        REQUIRE(*(end + -1)->begin() == 6);
        REQUIRE((end + -1)->end() == chunked.end()->end());
        REQUIRE(*(end + -2)->begin() == 3);
        REQUIRE(*(end + -2)->end() == 6);
        REQUIRE(*(end + -3)->begin() == 1);
        REQUIRE(*(end + -3)->end() == 3);
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        auto end = chunked.end();
        REQUIRE(*(end - 1)->begin() == 6);
        REQUIRE((end - 1)->end() == chunked.end()->end());
    }

    SECTION("Operator-(Iterator)") {
        auto beg = chunked.begin();
        REQUIRE(std::distance(beg, chunked.end()) == 3);
        ++beg;
        REQUIRE(std::distance(beg, chunked.end()) == 2);
        ++beg;
        REQUIRE(std::distance(beg, chunked.end()) == 1);
        ++beg;
        REQUIRE(std::distance(beg, chunked.end()) == 0);
    }

    SECTION("Operator[]()") {
        auto beg = chunked.begin();
        REQUIRE((beg[0] | lz::to<std::array<int, 3>>()) == std::array<int, 3>{ 1, 2, 3 });
        REQUIRE((beg[1] | lz::to<std::array<int, 3>>()) == std::array<int, 3>{ 4, 5, 6 });
        REQUIRE((beg[2] | lz::to<std::array<int, 2>>()) == std::array<int, 2>{ 7, 8 });
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto b = chunked.begin();
        auto end = chunked.end();
        auto distance = std::distance(b, end);

        REQUIRE(b < end);
        REQUIRE(b + distance - 1 > end - distance);
        REQUIRE(b + distance - 1 <= end);
        REQUIRE(b + distance - 1 >= end - 1);
    }
}

TEST_CASE("Chunks to containers", "[Chunk][To container]") {
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto chunked = lz::chunks(v, 3);
    using value_type_t = decltype(chunked.begin())::value_type;

    SECTION("To array") {
        auto arrays = chunked | lz::map([](value_type_t chunk) { return chunk | lz::to<std::array<int, 3>>(); }) |
                      lz::to<std::array<std::array<int, 3>, 3>>();

        std::array<std::array<int, 3>, 3> expected = { std::array<int, 3>{ 1, 2, 3 }, std::array<int, 3>{ 4, 5, 6 },
                                                       std::array<int, 3>{ 7, 8 } };

        REQUIRE(arrays == expected);
    }

    SECTION("To vector") {
        auto vectors =
            chunked | lz::map([](value_type_t chunk) { return chunk | lz::to<std::vector>(); }) | lz::to<std::vector>();

        std::vector<std::vector<int>> expected = { std::vector<int>{ 1, 2, 3 }, std::vector<int>{ 4, 5, 6 },
                                                   std::vector<int>{ 7, 8 } };

        REQUIRE(vectors == expected);
    }

    SECTION("To other container using to()") {
        auto lists = chunked | lz::map([](value_type_t chunk) { return chunk | lz::to<std::list>(); }) | lz::to<std::list>();

        std::list<std::list<int>> expected = { std::list<int>{ 1, 2, 3 }, std::list<int>{ 4, 5, 6 }, std::list<int>{ 7, 8 } };

        REQUIRE(lists == expected);
    }
}