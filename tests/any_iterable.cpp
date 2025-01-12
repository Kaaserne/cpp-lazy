#include <Lz/lz.hpp>
#include <Lz/c_string.hpp>
#include <catch2/catch.hpp>
#include <iostream>
#include <list>


TEST_CASE("Any iterable with sentinels") {
    auto cstr = lz::c_string("Hello, world!");
    lz::any_iterable<char, const char&> iterable = cstr;
    auto expected = "Hello, world!";
    CHECK(lz::equal(iterable, lz::c_string(expected)));
}

TEST_CASE("Empty or one element any iterable") {
    SECTION("Empty") {
        std::vector<int> vec;
        lz::any_iterable<int, int&> iterable = vec;
        CHECK(lz::empty(iterable));
        CHECK(!lz::has_one(iterable));
        CHECK(!lz::has_many(iterable));
    }

    SECTION("One element") {
        std::vector<int> vec = { 1 };
        lz::any_iterable<int, int&> iterable = vec;
        CHECK(!lz::empty(iterable));
        CHECK(lz::has_one(iterable));
        CHECK(!lz::has_many(iterable));
    }
}

TEST_CASE("Creating a basic any iterable from std::vector, random access iterator") {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    lz::any_iterable<int, int&, std::random_access_iterator_tag> view = vec;
    auto begin = view.begin();
    auto another_begin = view.begin();
    auto end = view.end();
    CHECK(begin == another_begin);
    ++another_begin;
    CHECK(begin != another_begin);
    CHECK(begin < another_begin);
    CHECK(another_begin > begin);

    --another_begin;
    CHECK(begin <= another_begin);
    CHECK(another_begin >= begin);

    CHECK(*begin == 1);
    --end;
    REQUIRE(*end == 5);
    REQUIRE(begin[0] == 1);
    REQUIRE(begin[1] == 2);

    ++end;

    CHECK(static_cast<std::size_t>(std::distance(begin, end)) == vec.size());
    CHECK(static_cast<std::size_t>(end - begin) == vec.size());

    CHECK(*(begin + 2) == 3);
    begin += 2;
    CHECK(*(begin - 2) == 1);
    CHECK(*begin == 3);
    begin -= 2;
    CHECK(*begin == 1);
}

TEST_CASE("Creating a basic any iterable from std::list, forward iterator") {
    std::list<int> lst = { 1, 2, 3, 4, 5 };
    lz::any_iterable<int, int&> view = lst;
    auto begin = view.begin();
    auto another_begin = view.begin();
    auto end = view.end();
    CHECK(begin == another_begin);
    ++another_begin;
    CHECK(begin != another_begin);

    CHECK(*begin == 1);
    auto last = std::next(begin, 4);
    REQUIRE(*last == 5);
    ++last;

    CHECK(static_cast<std::size_t>(std::distance(begin, last)) == lst.size());
}

TEST_CASE("Creating a basic any iterable from std::list, bidirectional iterator") {
    std::list<int> lst = { 1, 2, 3, 4, 5 };
    lz::any_iterable<int, int&, std::bidirectional_iterator_tag> view = lst;
    auto begin = view.begin();
    auto another_begin = view.begin();
    auto end = view.end();
    CHECK(begin == another_begin);
    ++another_begin;
    CHECK(begin != another_begin);

    CHECK(*begin == 1);
    --end;
    REQUIRE(*end == 5);
    ++end;

    CHECK(static_cast<std::size_t>(std::distance(begin, end)) == lst.size());
}

TEST_CASE("Creating a complex any iterable, std::forward_iterator_tag") {
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
    using Pair = std::pair<int, int&>;
    lz::any_iterable<Pair, Pair> view = lz::chain(vec).as<int&>().enumerate().take(static_cast<std::ptrdiff_t>(vec.size()));
    CHECK(vec.size() == static_cast<std::size_t>(view.distance()));
    std::pair<int, int&> pair = *view.begin();
    CHECK(pair.first == 0);
    CHECK(pair.second == vec[0]);
}