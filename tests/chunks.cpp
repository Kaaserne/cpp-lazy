#include <Lz/c_string.hpp>
#include <Lz/chunks.hpp>
#include <Lz/map.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

TEST_CASE("empty or one element chunks") {
    SUBCASE("Empty") {
        std::vector<int> vec;
        auto chunked = lz::chunks(vec, 3);
        REQUIRE(lz::empty(chunked));
        REQUIRE_FALSE(lz::has_one(chunked));
        REQUIRE_FALSE(lz::has_many(chunked));
        REQUIRE(chunked.size() == 0);
    }

    SUBCASE("One element") {
        std::vector<int> vec = { 1 };
        auto chunked = lz::chunks(vec, 3);
        REQUIRE_FALSE(lz::empty(chunked));
        REQUIRE(lz::has_one(chunked));
        REQUIRE_FALSE(lz::has_many(chunked));
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

    REQUIRE(lz::ssize(uneven_chunksize_even_size) == lz::distance(uneven_chunksize_even_size));
    REQUIRE(lz::ssize(even_chunksize_even_size) == lz::distance(even_chunksize_even_size));
    REQUIRE(lz::ssize(uneven_chunksize_uneven_size) == lz::distance(uneven_chunksize_uneven_size));
    REQUIRE(lz::ssize(even_chunksize_uneven_size) == lz::distance(even_chunksize_uneven_size));

    static_assert(std::is_same<decltype(uneven_chunksize_even_size.begin()), decltype(uneven_chunksize_even_size.end())>::value,
                  "Should not be sentinel");

    using iterable = typename decltype(uneven_chunksize_even_size)::value_type;
    const auto equal_fn = [](iterable a, const std::vector<int>& b) {
        return lz::equal(a, b);
    };

    SUBCASE("Operator++") {
        std::vector<std::vector<int>> expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8 } };
        REQUIRE(lz::equal(uneven_chunksize_even_size, expected, equal_fn));

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } };
        REQUIRE(lz::equal(even_chunksize_even_size, expected, equal_fn));

        expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7 } };
        REQUIRE(lz::equal(uneven_chunksize_uneven_size, expected, equal_fn));

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7 } };
        REQUIRE(lz::equal(even_chunksize_uneven_size, expected, equal_fn));
    }

    SUBCASE("Operator--") {
        std::vector<std::vector<int>> expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8 } };
        REQUIRE(lz::equal(uneven_chunksize_even_size | lz::cached_reverse, expected | lz::reverse, equal_fn));

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } };
        REQUIRE(lz::equal(even_chunksize_even_size | lz::cached_reverse, expected | lz::reverse, equal_fn));

        expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7 } };
        REQUIRE(lz::equal(uneven_chunksize_uneven_size | lz::cached_reverse, expected | lz::reverse, equal_fn));

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7 } };
        REQUIRE(lz::equal(even_chunksize_uneven_size | lz::cached_reverse, expected | lz::reverse, equal_fn));
    }

    SUBCASE("Operator+") {
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

    SUBCASE("Operator-") {
        test_procs::test_operator_minus(uneven_chunksize_even_size);
        test_procs::test_operator_minus(even_chunksize_even_size);
        test_procs::test_operator_minus(uneven_chunksize_uneven_size);
        test_procs::test_operator_minus(even_chunksize_uneven_size);
    }

    SUBCASE("Operator-(default_sentinel_t)") {
        auto uneven_chunksize_uneven_size_repeat = lz::chunks(lz::repeat(0, 3), 3);
        auto even_chunksize_uneven_size_repeat = lz::chunks(lz::repeat(0, 3), 2);
        auto uneven_chunksize_even_size_repeat = lz::chunks(lz::repeat(0, 2), 3);
        auto even_chunksize_even_size_repeat = lz::chunks(lz::repeat(0, 2), 2);

        test_procs::test_operator_minus(uneven_chunksize_uneven_size_repeat);
        test_procs::test_operator_minus(even_chunksize_uneven_size_repeat);
        test_procs::test_operator_minus(uneven_chunksize_even_size_repeat);
        test_procs::test_operator_minus(even_chunksize_even_size_repeat);
    }

    SUBCASE("Operator+(default_sentinel_t)") {
        auto uneven_chunksize_uneven_size_repeat = lz::chunks(lz::repeat(0, 7), 3);
        auto even_chunksize_uneven_size_repeat = lz::chunks(lz::repeat(0, 7), 2);
        auto uneven_chunksize_even_size_repeat = lz::chunks(lz::repeat(0, 6), 3);
        auto even_chunksize_even_size_repeat = lz::chunks(lz::repeat(0, 4), 2);

        using value_type = typename decltype(uneven_chunksize_even_size_repeat)::value_type;
        const auto equal_fn2 = [](value_type a, const std::vector<int>& b) {
            return lz::equal(a, b);
        };

        std::vector<std::vector<int>> expected;

        expected = { { 0, 0, 0 }, { 0, 0, 0 }, { 0 } };
        test_procs::test_operator_plus(uneven_chunksize_uneven_size_repeat, expected, equal_fn2);

        expected = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0 } };
        test_procs::test_operator_plus(even_chunksize_uneven_size_repeat, expected, equal_fn2);

        expected = { { 0, 0, 0 }, { 0, 0, 0 } };
        test_procs::test_operator_plus(uneven_chunksize_even_size_repeat, expected, equal_fn2);

        expected = { { 0, 0 }, { 0, 0 } };
        test_procs::test_operator_plus(even_chunksize_even_size_repeat, expected, equal_fn2);
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

    SUBCASE("Operator++") {
        std::vector<std::vector<int>> expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8 } };
        REQUIRE(lz::equal(uneven_chunksize_even_size, expected, equal_fn));

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } };
        REQUIRE(lz::equal(even_chunksize_even_size, expected, equal_fn));

        expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7 } };
        REQUIRE(lz::equal(uneven_chunksize_uneven_size, expected, equal_fn));

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7 } };
        REQUIRE(lz::equal(even_chunksize_uneven_size, expected, equal_fn));
    }

    SUBCASE("Operator--") {
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

TEST_CASE("Operator=(default_sentinel_t)") {
    std::vector<std::vector<int>> expected;

    SUBCASE("forward") {
        std::forward_list<int> fwd = { 1, 2, 3, 4, 5 };
        auto chunked = lz::chunks(fwd, 2);
        using value_type = lz::val_iterable_t<decltype(chunked)>;
        auto common = make_sentinel_assign_op_tester(chunked);
        expected = { { 1, 2 }, { 3, 4 }, { 5 } };
        REQUIRE(lz::equal(common, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));
    }

    SUBCASE("random access") {
        auto repeater = lz::repeat(20, 5);
        auto repeated_chunk = make_sentinel_assign_op_tester(lz::chunks(repeater, 2));
        using value_type_2 = lz::val_iterable_t<decltype(repeated_chunk)>;
        expected = { { 20, 20 }, { 20, 20 }, { 20 } };
        REQUIRE(lz::equal(repeated_chunk, expected, [](value_type_2 a, const std::vector<int>& b) { return lz::equal(a, b); }));
        REQUIRE(lz::equal(repeated_chunk | lz::reverse, expected | lz::reverse,
                          [](value_type_2 a, const std::vector<int>& b) { return lz::equal(a, b); }));
        test_procs::test_operator_minus(repeated_chunk);
        test_procs::test_operator_plus(repeated_chunk, expected,
                                       [](value_type_2 a, const std::vector<int>& b) { return lz::equal(a, b); });
    }

    SUBCASE("bidirectional") {
        std::list<int> lst = { 1, 2, 3, 4, 5, 6 };
        auto bidi_sentinel = make_bidi_sentinelled(lst);
        auto lst_chunked = make_sentinel_assign_op_tester(lz::chunks(bidi_sentinel, 3));
        using value_type_3 = lz::val_iterable_t<decltype(lst_chunked)>;
        expected = { { 1, 2, 3 }, { 4, 5, 6 } };
        REQUIRE(lz::equal(lst_chunked, expected, [](value_type_3 a, const std::vector<int>& b) { return lz::equal(a, b); }));
        REQUIRE(lz::equal(lst_chunked | lz::reverse, expected | lz::reverse,
                          [](value_type_3 a, const std::vector<int>& b) { return lz::equal(a, b); }));
    }
}

TEST_CASE("Chunks with sentinels / fwd") {
    auto even_size = lz::c_string("12345678");
    auto uneven_size = lz::c_string("1234567");

    auto uneven_chunksize_uneven_size = lz::chunks(uneven_size, 3);
    auto even_chunksize_uneven_size = lz::chunks(uneven_size, 2);

    auto uneven_chunksize_even_size = lz::chunks(even_size, 3);
    auto even_chunksize_even_size = lz::chunks(even_size, 2);

    static_assert(!std::is_same<decltype(uneven_chunksize_even_size.begin()), decltype(uneven_chunksize_even_size.end())>::value,
                  "Should not be sentinel");

    using iterable = typename decltype(uneven_chunksize_even_size)::value_type;
    const auto equal_fn = [](iterable a, const std::vector<char>& b) {
        return lz::equal(a, b);
    };

    SUBCASE("Operator++") {
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

    SUBCASE("To array") {
        auto arrays = chunked | lz::map([](value_type_t chunk) { return chunk | lz::to<std::array<int, 3>>(); }) |
                      lz::to<std::array<std::array<int, 3>, 3>>();

        std::array<std::array<int, 3>, 3> expected = { std::array<int, 3>{ 1, 2, 3 }, std::array<int, 3>{ 4, 5, 6 },
                                                       std::array<int, 3>{ 7, 8 } };

        REQUIRE(arrays == expected);
    }

    SUBCASE("To vector") {
        auto vectors =
            chunked | lz::map([](value_type_t chunk) { return chunk | lz::to<std::vector>(); }) | lz::to<std::vector>();

        std::vector<std::vector<int>> expected = { std::vector<int>{ 1, 2, 3 }, std::vector<int>{ 4, 5, 6 },
                                                   std::vector<int>{ 7, 8 } };

        REQUIRE(vectors == expected);
    }

    SUBCASE("To other container using to()") {
        auto lists = chunked | lz::map([](value_type_t chunk) { return chunk | lz::to<std::list>(); }) | lz::to<std::list>();

        std::list<std::list<int>> expected = { std::list<int>{ 1, 2, 3 }, std::list<int>{ 4, 5, 6 }, std::list<int>{ 7, 8 } };

        REQUIRE(lists == expected);
    }
}
