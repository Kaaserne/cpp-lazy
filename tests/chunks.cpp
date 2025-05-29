#include <Lz/c_string.hpp>
#include <Lz/chunks.hpp>
#include <Lz/map.hpp>
#include <Lz/stream.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <vector>

TEST_CASE("Chunks changing and creating elements") {
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7 };
    lz::chunks_iterable<std::vector<int>> chunked = lz::chunks(v, 3);
    auto it = chunked.begin();
    std::vector<int> expected = { 1, 2, 3 };
    REQUIRE(lz::equal(*it, expected));
    ++it;
    expected = { 4, 5, 6 };
    REQUIRE(lz::equal(*it, expected));
    ++it;
    expected = { 7 };
    REQUIRE(lz::equal(*it, expected));
    ++it;
    REQUIRE(it == chunked.end());

    it = chunked.begin();
    REQUIRE(it != chunked.end());
    it = chunked.end();
    REQUIRE(it == chunked.end());
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

TEST_CASE("Chunks binary operations random access") {
    std::vector<int> even_size = { 1, 2, 3, 4, 5, 6, 7, 8 };
    std::vector<int> uneven_size = { 1, 2, 3, 4, 5, 6, 7 };

    auto uneven_chunksize_uneven_size = lz::chunks(uneven_size, 3);
    auto even_chunksize_uneven_size = lz::chunks(uneven_size, 2);

    auto uneven_chunksize_even_size = lz::chunks(even_size, 3);
    auto even_chunksize_even_size = lz::chunks(even_size, 2);

    REQUIRE(uneven_chunksize_even_size.size() ==
            static_cast<std::size_t>(lz::distance(uneven_chunksize_even_size.begin(), uneven_chunksize_even_size.end())));
    REQUIRE(even_chunksize_even_size.size() ==
            static_cast<std::size_t>(lz::distance(even_chunksize_even_size.begin(), even_chunksize_even_size.end())));
    REQUIRE(uneven_chunksize_uneven_size.size() ==
            static_cast<std::size_t>(lz::distance(uneven_chunksize_uneven_size.begin(), uneven_chunksize_uneven_size.end())));
    REQUIRE(even_chunksize_uneven_size.size() ==
            static_cast<std::size_t>(lz::distance(even_chunksize_uneven_size.begin(), even_chunksize_uneven_size.end())));

    static_assert(std::is_same<decltype(uneven_chunksize_even_size.begin()), decltype(uneven_chunksize_even_size.end())>::value,
                  "Should not be sentinel");

    auto it = uneven_chunksize_even_size.begin();
    auto it_end = uneven_chunksize_even_size.end();
    auto it2 = even_chunksize_even_size.begin();
    auto it_end2 = even_chunksize_even_size.end();

    auto it3 = uneven_chunksize_uneven_size.begin();
    auto it_end3 = uneven_chunksize_uneven_size.end();
    auto it4 = even_chunksize_uneven_size.begin();
    auto it_end4 = even_chunksize_uneven_size.end();

    SECTION("Operator++") {
        std::vector<int> expected = { 1, 2, 3 };
        REQUIRE(lz::equal(*it, expected));
        ++it;
        expected = { 4, 5, 6 };
        REQUIRE(lz::equal(*it, expected));
        ++it;
        expected = { 7, 8 };
        REQUIRE(lz::equal(*it, expected));
        ++it;
        REQUIRE(it == uneven_chunksize_even_size.end());

        expected = { 1, 2 };
        REQUIRE(lz::equal(*it2, expected));
        ++it2;
        expected = { 3, 4 };
        REQUIRE(lz::equal(*it2, expected));
        ++it2;
        expected = { 5, 6 };
        REQUIRE(lz::equal(*it2, expected));
        ++it2;
        expected = { 7, 8 };
        REQUIRE(lz::equal(*it2, expected));
        ++it2;
        REQUIRE(it2 == even_chunksize_even_size.end());

        expected = { 1, 2, 3 };
        REQUIRE(lz::equal(*it3, expected));
        ++it3;
        expected = { 4, 5, 6 };
        REQUIRE(lz::equal(*it3, expected));
        ++it3;
        expected = { 7 };
        REQUIRE(lz::equal(*it3, expected));
        ++it3;
        REQUIRE(it3 == uneven_chunksize_uneven_size.end());

        expected = { 1, 2 };
        REQUIRE(lz::equal(*it4, expected));
        ++it4;
        expected = { 3, 4 };
        REQUIRE(lz::equal(*it4, expected));
        ++it4;
        expected = { 5, 6 };
        REQUIRE(lz::equal(*it4, expected));
        ++it4;
        expected = { 7 };
        REQUIRE(lz::equal(*it4, expected));
        ++it4;
        REQUIRE(it4 == even_chunksize_uneven_size.end());
    }

    SECTION("Operator--") {
        --it_end;
        std::vector<int> expected = { 7, 8 };
        REQUIRE(lz::equal(*it_end, expected));
        --it_end;
        expected = { 4, 5, 6 };
        REQUIRE(lz::equal(*it_end, expected));
        --it_end;
        expected = { 1, 2, 3 };
        REQUIRE(lz::equal(*it_end, expected));
        REQUIRE(it_end == uneven_chunksize_even_size.begin());

        --it_end2;
        expected = { 7, 8 };
        REQUIRE(lz::equal(*it_end2, expected));
        --it_end2;
        expected = { 5, 6 };
        REQUIRE(lz::equal(*it_end2, expected));
        --it_end2;
        expected = { 3, 4 };
        REQUIRE(lz::equal(*it_end2, expected));
        --it_end2;
        expected = { 1, 2 };
        REQUIRE(lz::equal(*it_end2, expected));
        REQUIRE(it2 == even_chunksize_even_size.begin());

        --it_end3;
        expected = { 7 };
        REQUIRE(lz::equal(*it_end3, expected));
        --it_end3;
        expected = { 4, 5, 6 };
        REQUIRE(lz::equal(*it_end3, expected));
        --it_end3;
        expected = { 1, 2, 3 };
        REQUIRE(lz::equal(*it_end3, expected));
        REQUIRE(it_end3 == uneven_chunksize_uneven_size.begin());

        --it_end4;
        expected = { 7 };
        REQUIRE(lz::equal(*it_end4, expected));
        --it_end4;
        expected = { 5, 6 };
        REQUIRE(lz::equal(*it_end4, expected));
        --it_end4;
        expected = { 3, 4 };
        REQUIRE(lz::equal(*it_end4, expected));
        --it_end4;
        expected = { 1, 2 };
        REQUIRE(lz::equal(*it_end4, expected));
        REQUIRE(it_end4 == even_chunksize_uneven_size.begin());
    }

    SECTION("Operator+") {
        auto test_iterator = [](const std::vector<std::vector<int>>& expected,
                                lz::detail::chunks_iterable<std::vector<int>> chunked) {
            auto begin = chunked.begin();
            auto end = chunked.end();

            for (std::ptrdiff_t i = 0; i < lz::ssize(chunked) - 1; ++i) {
                INFO("With i = " << i);
                REQUIRE(lz::equal(*(begin + i), *(expected.begin() + i)));
            }
            REQUIRE(begin + lz::ssize(chunked) == chunked.end());
            for (std::ptrdiff_t i = 1; i <= lz::ssize(chunked); ++i) {
                INFO("With i = " << i);
                REQUIRE(lz::equal(*(end - i), *(expected.end() - i)));
            }
            REQUIRE(end - lz::ssize(chunked) == chunked.begin());

            std::advance(begin, lz::ssize(chunked));
            std::advance(end, -lz::ssize(chunked));
            REQUIRE(begin + 0 == begin);
            REQUIRE(end + 0 == end);

            for (std::ptrdiff_t i = 0; i < lz::ssize(chunked) - 1; ++i) {
                INFO("With i = " << i);
                REQUIRE(lz::equal(*(end + i), *(expected.begin() + i)));
            }
            REQUIRE(end + lz::ssize(chunked) == chunked.end());
            for (std::ptrdiff_t i = 1; i <= lz::ssize(chunked); ++i) {
                INFO("With i = " << i);
                REQUIRE(lz::equal(*(begin - i), *(expected.end() - i)));
            }
            REQUIRE(begin - lz::ssize(chunked) == chunked.begin());
        };

        std::vector<std::vector<int>> expected;

        expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8 } };
        test_iterator(expected, uneven_chunksize_even_size);
        expected = { { 1, 2, 3 }, { 4, 5, 6 }, { 7 } };
        test_iterator(expected, uneven_chunksize_uneven_size);

        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } };
        test_iterator(expected, even_chunksize_even_size);
        expected = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7 } };
        test_iterator(expected, even_chunksize_uneven_size);
    }

    SECTION("Operator-") {
        auto test_operator_minus = [](decltype(uneven_chunksize_even_size) chunked) {
            auto begin = chunked.begin();
            auto end = chunked.end();

            for (std::ptrdiff_t i = 0; i < lz::ssize(chunked); ++i) {
                INFO("With i = " << i);
                REQUIRE((end - i) - begin == lz::ssize(chunked) - i);
                REQUIRE(end - (begin + i) == lz::ssize(chunked) - i);
                REQUIRE((begin + i) - end == -(lz::ssize(chunked) - i));
                REQUIRE(begin - (end - i) == -(lz::ssize(chunked) - i));
            }

            for (std::ptrdiff_t i = 0; i < lz::ssize(chunked); ++i) {
                INFO("With i = " << i);
                REQUIRE((end - i) - (begin + i) == lz::ssize(chunked) - 2 * i);
                REQUIRE((begin + i) - (end - i) == -(lz::ssize(chunked) - 2 * i));
            }
        };

        INFO("test_operator_minus(it, it_end, lz::size(uneven_chunksize_even_size))")
        test_operator_minus(uneven_chunksize_even_size);

        INFO("test_operator_minus(it2, it_end2, lz::size(even_chunksize_even_size))")
        test_operator_minus(even_chunksize_even_size);

        INFO("test_operator_minus(it3, it_end3, lz::size(uneven_chunksize_uneven_size))")
        test_operator_minus(uneven_chunksize_uneven_size);

        INFO("test_operator_minus(it4, it_end4, lz::size(even_chunksize_uneven_size))")
        test_operator_minus(even_chunksize_uneven_size);
    }
}

TEST_CASE("Chunks binary operations bidirectional access") {
    std::list<int> even_size = { 1, 2, 3, 4, 5, 6, 7, 8 };
    std::list<int> uneven_size = { 1, 2, 3, 4, 5, 6, 7 };

    auto uneven_chunksize_uneven_size = lz::chunks(uneven_size, 3);
    auto even_chunksize_uneven_size = lz::chunks(uneven_size, 2);

    auto uneven_chunksize_even_size = lz::chunks(even_size, 3);
    auto even_chunksize_even_size = lz::chunks(even_size, 2);

    REQUIRE(uneven_chunksize_even_size.size() ==
            static_cast<std::size_t>(lz::distance(uneven_chunksize_even_size.begin(), uneven_chunksize_even_size.end())));
    REQUIRE(even_chunksize_even_size.size() ==
            static_cast<std::size_t>(lz::distance(even_chunksize_even_size.begin(), even_chunksize_even_size.end())));
    REQUIRE(uneven_chunksize_uneven_size.size() ==
            static_cast<std::size_t>(lz::distance(uneven_chunksize_uneven_size.begin(), uneven_chunksize_uneven_size.end())));
    REQUIRE(even_chunksize_uneven_size.size() ==
            static_cast<std::size_t>(lz::distance(even_chunksize_uneven_size.begin(), even_chunksize_uneven_size.end())));

    static_assert(std::is_same<decltype(uneven_chunksize_even_size.begin()), decltype(uneven_chunksize_even_size.end())>::value,
                  "Should not be sentinel");

    auto it = uneven_chunksize_even_size.begin();
    auto it_end = uneven_chunksize_even_size.end();
    auto it2 = even_chunksize_even_size.begin();
    auto it_end2 = even_chunksize_even_size.end();

    auto it3 = uneven_chunksize_uneven_size.begin();
    auto it_end3 = uneven_chunksize_uneven_size.end();
    auto it4 = even_chunksize_uneven_size.begin();
    auto it_end4 = even_chunksize_uneven_size.end();

    SECTION("Operator++") {
        std::vector<int> expected = { 1, 2, 3 };
        REQUIRE(lz::equal(*it, expected));
        ++it;
        expected = { 4, 5, 6 };
        REQUIRE(lz::equal(*it, expected));
        ++it;
        expected = { 7, 8 };
        REQUIRE(lz::equal(*it, expected));
        ++it;
        REQUIRE(it == uneven_chunksize_even_size.end());

        expected = { 1, 2 };
        REQUIRE(lz::equal(*it2, expected));
        ++it2;
        expected = { 3, 4 };
        REQUIRE(lz::equal(*it2, expected));
        ++it2;
        expected = { 5, 6 };
        REQUIRE(lz::equal(*it2, expected));
        ++it2;
        expected = { 7, 8 };
        REQUIRE(lz::equal(*it2, expected));
        ++it2;
        REQUIRE(it2 == even_chunksize_even_size.end());

        expected = { 1, 2, 3 };
        REQUIRE(lz::equal(*it3, expected));
        ++it3;
        expected = { 4, 5, 6 };
        REQUIRE(lz::equal(*it3, expected));
        ++it3;
        expected = { 7 };
        REQUIRE(lz::equal(*it3, expected));
        ++it3;
        REQUIRE(it3 == uneven_chunksize_uneven_size.end());

        expected = { 1, 2 };
        REQUIRE(lz::equal(*it4, expected));
        ++it4;
        expected = { 3, 4 };
        REQUIRE(lz::equal(*it4, expected));
        ++it4;
        expected = { 5, 6 };
        REQUIRE(lz::equal(*it4, expected));
        ++it4;
        expected = { 7 };
        REQUIRE(lz::equal(*it4, expected));
        ++it4;
        REQUIRE(it4 == even_chunksize_uneven_size.end());
    }

    SECTION("Operator--") {
        --it_end;
        std::vector<int> expected = { 7, 8 };
        REQUIRE(lz::equal(*it_end, expected));
        --it_end;
        expected = { 4, 5, 6 };
        REQUIRE(lz::equal(*it_end, expected));
        --it_end;
        expected = { 1, 2, 3 };
        REQUIRE(lz::equal(*it_end, expected));
        REQUIRE(it_end == uneven_chunksize_even_size.begin());

        --it_end2;
        expected = { 7, 8 };
        REQUIRE(lz::equal(*it_end2, expected));
        --it_end2;
        expected = { 5, 6 };
        REQUIRE(lz::equal(*it_end2, expected));
        --it_end2;
        expected = { 3, 4 };
        REQUIRE(lz::equal(*it_end2, expected));
        --it_end2;
        expected = { 1, 2 };
        REQUIRE(lz::equal(*it_end2, expected));
        REQUIRE(it2 == even_chunksize_even_size.begin());

        --it_end3;
        expected = { 7 };
        REQUIRE(lz::equal(*it_end3, expected));
        --it_end3;
        expected = { 4, 5, 6 };
        REQUIRE(lz::equal(*it_end3, expected));
        --it_end3;
        expected = { 1, 2, 3 };
        REQUIRE(lz::equal(*it_end3, expected));
        REQUIRE(it_end3 == uneven_chunksize_uneven_size.begin());

        --it_end4;
        expected = { 7 };
        REQUIRE(lz::equal(*it_end4, expected));
        --it_end4;
        expected = { 5, 6 };
        REQUIRE(lz::equal(*it_end4, expected));
        --it_end4;
        expected = { 3, 4 };
        REQUIRE(lz::equal(*it_end4, expected));
        --it_end4;
        expected = { 1, 2 };
        REQUIRE(lz::equal(*it_end4, expected));
        REQUIRE(it_end4 == even_chunksize_uneven_size.begin());
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

    auto it = uneven_chunksize_even_size.begin();
    auto it2 = even_chunksize_even_size.begin();

    auto it3 = uneven_chunksize_uneven_size.begin();
    auto it4 = even_chunksize_uneven_size.begin();

    SECTION("Operator++") {
        std::vector<int> expected = { '1', '2', '3' };
        REQUIRE(lz::equal(*it, expected));
        ++it;
        expected = { '4', '5', '6' };
        REQUIRE(lz::equal(*it, expected));
        ++it;
        expected = { '7', '8' };
        REQUIRE(lz::equal(*it, expected));
        ++it;
        REQUIRE(it == uneven_chunksize_even_size.end());

        expected = { '1', '2' };
        REQUIRE(lz::equal(*it2, expected));
        ++it2;
        expected = { '3', '4' };
        REQUIRE(lz::equal(*it2, expected));
        ++it2;
        expected = { '5', '6' };
        REQUIRE(lz::equal(*it2, expected));
        ++it2;
        expected = { '7', '8' };
        REQUIRE(lz::equal(*it2, expected));
        ++it2;
        REQUIRE(it2 == even_chunksize_even_size.end());

        expected = { '1', '2', '3' };
        REQUIRE(lz::equal(*it3, expected));
        ++it3;
        expected = { '4', '5', '6' };
        REQUIRE(lz::equal(*it3, expected));
        ++it3;
        expected = { '7' };
        REQUIRE(lz::equal(*it3, expected));
        ++it3;
        REQUIRE(it3 == uneven_chunksize_uneven_size.end());

        expected = { '1', '2' };
        REQUIRE(lz::equal(*it4, expected));
        ++it4;
        expected = { '3', '4' };
        REQUIRE(lz::equal(*it4, expected));
        ++it4;
        expected = { '5', '6' };
        REQUIRE(lz::equal(*it4, expected));
        ++it4;
        expected = { '7' };
        REQUIRE(lz::equal(*it4, expected));
        ++it4;
        REQUIRE(it4 == even_chunksize_uneven_size.end());
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