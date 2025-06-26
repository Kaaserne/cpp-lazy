#include <Lz/chunks.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <Lz/stream.hpp>
#include <c_string/c_string_forward_decl.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <test_procs.hpp>
#include <vector>

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

TEST_CASE("Chunks binary operations random access") {
    std::vector<int> even_size = { 1, 2, 3, 4, 5, 6, 7, 8 };
    std::vector<int> uneven_size = { 1, 2, 3, 4, 5, 6, 7 };

    auto uneven_chunksize_uneven_size = lz::chunks(uneven_size, 3);
    auto even_chunksize_uneven_size = lz::chunks(uneven_size, 2);

    auto uneven_chunksize_even_size = lz::chunks(even_size, 3);
    auto even_chunksize_even_size = lz::chunks(even_size, 2);

    REQUIRE(lz::ssize(uneven_chunksize_even_size) ==
            lz::distance(uneven_chunksize_even_size.begin(), uneven_chunksize_even_size.end()));
    REQUIRE(lz::ssize(even_chunksize_even_size) ==
            lz::distance(even_chunksize_even_size.begin(), even_chunksize_even_size.end()));
    REQUIRE(lz::ssize(uneven_chunksize_uneven_size) ==
            lz::distance(uneven_chunksize_uneven_size.begin(), uneven_chunksize_uneven_size.end()));
    REQUIRE(lz::ssize(even_chunksize_uneven_size) ==
            lz::distance(even_chunksize_uneven_size.begin(), even_chunksize_uneven_size.end()));

    static_assert(std::is_same<decltype(uneven_chunksize_even_size.begin()), decltype(uneven_chunksize_even_size.end())>::value,
                  "Should not be sentinel");

    using iterable = typename decltype(uneven_chunksize_even_size)::value_type;
    const auto equal_fn = [](iterable a, const std::vector<int>& b) {
        return lz::equal(a, b);
    };

    SECTION("Operator++") {
        std::vector<std::vector<int>> expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8 } };
        REQUIRE(lz::equal(uneven_chunksize_even_size, expected, equal_fn));

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } };
        REQUIRE(lz::equal(even_chunksize_even_size, expected, equal_fn));

        expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7 } };
        REQUIRE(lz::equal(uneven_chunksize_uneven_size, expected, equal_fn));

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7 } };
        REQUIRE(lz::equal(even_chunksize_uneven_size, expected, equal_fn));
    }

    SECTION("Operator--") {
        std::vector<std::vector<int>> expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8 } };
        REQUIRE(lz::equal(uneven_chunksize_even_size | lz::cached_reverse, expected | lz::reverse, equal_fn));

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } };
        REQUIRE(lz::equal(even_chunksize_even_size | lz::cached_reverse, expected | lz::reverse, equal_fn));

        expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7 } };
        REQUIRE(lz::equal(uneven_chunksize_uneven_size | lz::cached_reverse, expected | lz::reverse, equal_fn));

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7 } };
        REQUIRE(lz::equal(even_chunksize_uneven_size | lz::cached_reverse, expected | lz::reverse, equal_fn));
    }

    SECTION("Operator+") {
        std::vector<std::vector<int>> expected;

        expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8 } };
        test_procs::test_operator_plus(uneven_chunksize_even_size, expected, equal_fn);

        expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7 } };
        test_procs::test_operator_plus(uneven_chunksize_uneven_size, expected, equal_fn);

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } };
        test_procs::test_operator_plus(even_chunksize_even_size, expected, equal_fn);

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7 } };
        test_procs::test_operator_plus(even_chunksize_uneven_size, expected, equal_fn);
    }

    SECTION("Operator-") {
        test_procs::test_operator_minus(uneven_chunksize_even_size);
        test_procs::test_operator_minus(even_chunksize_even_size);
        test_procs::test_operator_minus(uneven_chunksize_uneven_size);
        test_procs::test_operator_minus(even_chunksize_uneven_size);
    }
}

TEST_CASE("Chunks binary operations bidirectional access") {
    std::list<int> even_size = { 1, 2, 3, 4, 5, 6, 7, 8 };
    std::list<int> uneven_size = { 1, 2, 3, 4, 5, 6, 7 };

    auto uneven_chunksize_uneven_size = lz::chunks(uneven_size, 3);
    auto even_chunksize_uneven_size = lz::chunks(uneven_size, 2);

    auto uneven_chunksize_even_size = lz::chunks(even_size, 3);
    auto even_chunksize_even_size = lz::chunks(even_size, 2);

    REQUIRE(lz::ssize(uneven_chunksize_even_size) ==
            lz::distance(uneven_chunksize_even_size.begin(), uneven_chunksize_even_size.end()));
    REQUIRE(lz::ssize(even_chunksize_even_size) ==
            lz::distance(even_chunksize_even_size.begin(), even_chunksize_even_size.end()));
    REQUIRE(lz::ssize(uneven_chunksize_uneven_size) ==
            lz::distance(uneven_chunksize_uneven_size.begin(), uneven_chunksize_uneven_size.end()));
    REQUIRE(lz::ssize(even_chunksize_uneven_size) ==
            lz::distance(even_chunksize_uneven_size.begin(), even_chunksize_uneven_size.end()));

    static_assert(std::is_same<decltype(uneven_chunksize_even_size.begin()), decltype(uneven_chunksize_even_size.end())>::value,
                  "Should not be sentinel");

    using iterable = typename decltype(uneven_chunksize_even_size)::value_type;
    const auto equal_fn = [](iterable a, const std::vector<int>& b) {
        return lz::equal(a, b);
    };

    SECTION("Operator++") {
        std::vector<std::vector<int>> expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8 } };
        REQUIRE(lz::equal(uneven_chunksize_even_size, expected, equal_fn));

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } };
        REQUIRE(lz::equal(even_chunksize_even_size, expected, equal_fn));

        expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7 } };
        REQUIRE(lz::equal(uneven_chunksize_uneven_size, expected, equal_fn));

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7 } };
        REQUIRE(lz::equal(even_chunksize_uneven_size, expected, equal_fn));
    }

    SECTION("Operator--") {
        std::vector<std::vector<int>> expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8 } };
        REQUIRE(lz::equal(uneven_chunksize_even_size | lz::cached_reverse, expected | lz::reverse, equal_fn));

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } };
        REQUIRE(lz::equal(even_chunksize_even_size | lz::cached_reverse, expected | lz::reverse, equal_fn));

        expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7 } };
        REQUIRE(lz::equal(uneven_chunksize_uneven_size | lz::cached_reverse, expected | lz::reverse, equal_fn));

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7 } };
        REQUIRE(lz::equal(even_chunksize_uneven_size | lz::cached_reverse, expected | lz::reverse, equal_fn));
    }
}

TEST_CASE("Chunks with sentinels / fwd") {
    auto even_size = lz::c_string("12345678");
    auto uneven_size = lz::c_string("1234567");

    auto uneven_chunksize_uneven_size = lz::chunks(uneven_size, 3);
    auto even_chunksize_uneven_size = lz::chunks(uneven_size, 2);

    auto uneven_chunksize_even_size = lz::chunks(even_size, 3);
    auto even_chunksize_even_size = lz::chunks(even_size, 2);

    SECTION("Operator=") {
        auto it = even_size.begin();
        REQUIRE(it == even_size.begin());
        REQUIRE(it != even_size.end());
        REQUIRE(even_size.end() != it);
        REQUIRE(even_size.begin() == it);

        it = even_size.end();
        REQUIRE(it == even_size.end());
        REQUIRE(it != even_size.begin());
        REQUIRE(even_size.end() == it);
        REQUIRE(even_size.begin() != it);
    }

    static_assert(!std::is_same<decltype(uneven_chunksize_even_size.begin()), decltype(uneven_chunksize_even_size.end())>::value,
                  "Should not be sentinel");

    using iterable = typename decltype(uneven_chunksize_even_size)::value_type;
    const auto equal_fn = [](iterable a, const std::vector<char>& b) {
        return lz::equal(a, b);
    };

    SECTION("Operator++") {
        std::vector<std::vector<char>> expected = { { '1', '2', '3' }, { '4', '5', '6' }, { '7', '8' } };
        REQUIRE(lz::equal(uneven_chunksize_even_size, expected, equal_fn));

        expected = { { '1', '2' }, { '3', '4' }, { '5', '6' }, { '7', '8' } };
        REQUIRE(lz::equal(even_chunksize_even_size, expected, equal_fn));

        expected = { { '1', '2', '3' }, { '4', '5', '6' }, { '7' } };
        REQUIRE(lz::equal(uneven_chunksize_uneven_size, expected, equal_fn));

        expected = { { '1', '2' }, { '3', '4' }, { '5', '6' }, { '7' } };
        REQUIRE(lz::equal(even_chunksize_uneven_size, expected, equal_fn));
    }
}

TEST_CASE("Chunks to containers") {
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