#include <Lz/any_iterable.hpp>
#include <Lz/enumerate.hpp>
#include <Lz/iter_tools.hpp>
#include <Lz/take.hpp>
#include <c_string/c_string_forward_decl.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Any iterable with sentinels") {
    lz::any_iterable<char, const char&> iterable = "Hello, World!" | lz::c_string;
    auto expected = "Hello, World!";
    REQUIRE(lz::equal(iterable, lz::c_string(expected)));
}

TEST_CASE("Empty or one element any iterable") {
    SECTION("Empty") {
        std::vector<int> vec;
        lz::any_iterable<int, int&> iterable = vec;
        REQUIRE(lz::empty(iterable));
        REQUIRE(!lz::has_one(iterable));
        REQUIRE(!lz::has_many(iterable));
    }

    SECTION("One element") {
        std::vector<int> vec = { 1 };
        lz::any_iterable<int, int&> iterable = vec;
        REQUIRE(!lz::empty(iterable));
        REQUIRE(lz::has_one(iterable));
        REQUIRE(!lz::has_many(iterable));
    }
}

TEST_CASE("Creating a basic any iterable from std::vector, random access iterator") {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    lz::any_iterable<int, int&, std::random_access_iterator_tag> view = vec;
    auto begin = view.begin();
    auto another_begin = view.begin();
    auto end = view.end();
    REQUIRE(begin == another_begin);
    ++another_begin;
    REQUIRE(begin != another_begin);
    REQUIRE(begin < another_begin);
    REQUIRE(another_begin > begin);

    --another_begin;
    REQUIRE(begin <= another_begin);
    REQUIRE(another_begin >= begin);

    REQUIRE(*begin == 1);
    --end;
    REQUIRE(*end == 5);
    REQUIRE(begin[0] == 1);
    REQUIRE(begin[1] == 2);

    ++end;

    REQUIRE(static_cast<std::size_t>(std::distance(begin, end)) == vec.size());
    REQUIRE(static_cast<std::size_t>(end - begin) == vec.size());

    REQUIRE(*(begin + 2) == 3);
    begin += 2;
    REQUIRE(*(begin - 2) == 1);
    REQUIRE(*begin == 3);
    begin -= 2;
    REQUIRE(*begin == 1);
}

TEST_CASE("Creating a basic any iterable from std::list, forward iterator") {
    std::list<int> lst = { 1, 2, 3, 4, 5 };
    lz::any_iterable<int, int&> view = lst;
    auto begin = view.begin();
    auto another_begin = view.begin();
    auto end = view.end();
    REQUIRE(begin == another_begin);
    ++another_begin;
    REQUIRE(begin != another_begin);

    REQUIRE(*begin == 1);
    auto last = std::next(begin, 4);
    REQUIRE(*last == 5);
    ++last;

    REQUIRE(static_cast<std::size_t>(std::distance(begin, last)) == lst.size());
}

TEST_CASE("Creating a basic any iterable from std::list, bidirectional iterator") {
    std::list<int> lst = { 1, 2, 3, 4, 5 };
    lz::any_iterable<int, int&, std::bidirectional_iterator_tag> view = lst;
    auto begin = view.begin();
    auto another_begin = view.begin();
    auto end = view.end();
    REQUIRE(begin == another_begin);
    ++another_begin;
    REQUIRE(begin != another_begin);

    REQUIRE(*begin == 1);
    --end;
    REQUIRE(*end == 5);
    ++end;

    REQUIRE(static_cast<std::size_t>(std::distance(begin, end)) == lst.size());
}

TEST_CASE("Creating a complex any iterable, std::forward_iterator_tag") {
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
    
    lz::any_iterable<std::pair<int, int>, std::pair<int, int&>> view =
#ifdef LZ_HAS_CXX_11
        vec | lz::as<int&>{} | lz::enumerate | lz::take(vec.size());
#else
        vec | lz::as<int&> | lz::enumerate | lz::take(vec.size());
#endif
    std::pair<int, int&> pair = *view.begin();
    REQUIRE(pair.first == 0);
    REQUIRE(pair.second == vec[0]);
}

TEST_CASE("Creating a complex any iterable, std::bidirectional_iterator_tag") {
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
    lz::any_iterable<std::pair<int, int>, std::pair<int, int&>, std::bidirectional_iterator_tag> view =
#ifdef LZ_HAS_CXX_11
        vec | lz::as<int&>{} | lz::enumerate | lz::take(vec.size());
#else
        vec | lz::as<int&> | lz::enumerate | lz::take(vec.size());
#endif
    std::pair<int, int&> pair = *view.begin();
    REQUIRE(pair.first == 0);
    REQUIRE(pair.second == vec[0]);
}

TEST_CASE("Creating a complex any iterable, std::random_access_iterator_tag") {
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
    lz::any_iterable<std::pair<int, int>, std::pair<int, int&>, std::random_access_iterator_tag> view =
#ifdef LZ_HAS_CXX_11
        vec | lz::as<int&>{} | lz::enumerate | lz::take(vec.size());
#else
        vec | lz::as<int&> | lz::enumerate | lz::take(vec.size());
#endif
    std::pair<int, int&> pair = *view.begin();
    REQUIRE(view.size() == vec.size());
    REQUIRE(pair.first == 0);
    REQUIRE(pair.second == vec[0]);
}