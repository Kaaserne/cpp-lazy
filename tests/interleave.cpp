#include <Lz/algorithm.hpp>
#include <Lz/interleave.hpp>
#include <Lz/range.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <doctest/doctest.h>
#include <vector>

TEST_CASE("Interleaved with sentinels permutations") {
    auto str3 = lz::c_string("abc");
    auto str4 = lz::c_string("defg");
    const auto str5 = lz::c_string("hijkl");

    SUBCASE("With iterable that yields by value") {
        const auto range = lz::range('c');
        auto interleaved = lz::interleave(str3, str4, range);
        using t1 = decltype(*interleaved.begin());
        static_assert(std::is_same<t1, char>::value, "Should be char");

        auto interleaved2 = lz::interleave(str3, range, str4);
        using t2 = decltype(*interleaved2.begin());
        static_assert(std::is_same<t2, char>::value, "Should be char");
    }

    SUBCASE("Operator=") {
        lz::interleave_iterable<decltype(str3), decltype(str4)> interleaved_3_4 = lz::interleave(str3, str4);
        using t2 = decltype(*interleaved_3_4.begin());
        static_assert(std::is_same<t2, const char&>::value, "Should be const char&");
        auto it_3_4 = interleaved_3_4.begin();
        REQUIRE(it_3_4 == interleaved_3_4.begin());
        it_3_4 = interleaved_3_4.end();
        REQUIRE(it_3_4 == interleaved_3_4.end());
        REQUIRE(it_3_4 == interleaved_3_4.end());

        auto interleaved_4_3 = str4 | lz::interleave(str3);
        auto it_4_3 = interleaved_4_3.begin();
        REQUIRE(it_4_3 == interleaved_4_3.begin());
        it_4_3 = interleaved_4_3.end();
        REQUIRE(it_4_3 == interleaved_4_3.end());
        REQUIRE(interleaved_4_3.end() == it_4_3);

        auto interleaved_3_4_5 = lz::interleave(str3, str4, str5);
        using t = decltype(*interleaved_3_4_5.begin());
        static_assert(std::is_same<t, const char&>::value, "Should be const char&");

        auto it_3_4_5 = interleaved_3_4_5.begin();
        REQUIRE(it_3_4_5 == interleaved_3_4_5.begin());
        it_3_4_5 = interleaved_3_4_5.end();
        REQUIRE(it_3_4_5 == interleaved_3_4_5.end());

        auto interleaved_5_4_3 = lz::interleave(str5, str4, str3);
        auto it_5_4_3 = interleaved_5_4_3.begin();
        REQUIRE(it_5_4_3 == interleaved_5_4_3.begin());
        it_5_4_3 = interleaved_5_4_3.end();
        REQUIRE(it_5_4_3 == interleaved_5_4_3.end());
    }

    SUBCASE("Permutation 1: 3 vs 4 characters and 4 vs 3 characters") {
        auto interleaved = lz::interleave(str3, str4);
        static_assert(!std::is_same<decltype(interleaved.begin()), decltype(interleaved.end())>::value, "Must be sentinel");

        std::vector<char> expected = { 'a', 'd', 'b', 'e', 'c', 'f' };
        REQUIRE(lz::equal(interleaved, expected));

        expected = { 'd', 'a', 'e', 'b', 'f', 'c' };
        interleaved = lz::interleave(str4, str3);
        REQUIRE(lz::equal(interleaved, expected));
    }

    SUBCASE("Permutation 2: 4 vs 5 characters and 5 vs 4 characters") {
        auto interleaved1 = lz::interleave(str4, str5);
        static_assert(!std::is_same<decltype(interleaved1.begin()), decltype(interleaved1.end())>::value, "Must be sentinel");

        std::vector<char> expected = { 'd', 'h', 'e', 'i', 'f', 'j', 'g', 'k' };
        REQUIRE(lz::equal(interleaved1, expected));

        expected = { 'h', 'd', 'i', 'e', 'j', 'f', 'k', 'g' };
        auto interleaved2 = lz::interleave(str5, str4);
        REQUIRE(lz::equal(interleaved2, expected));
    }

    SUBCASE("Permutation 3: 5 vs 3 characters and 3 vs 5 characters") {
        auto interleaved1 = lz::interleave(str5, str3);
        static_assert(!std::is_same<decltype(interleaved1.begin()), decltype(interleaved1.end())>::value, "Must be sentinel");

        std::vector<char> expected = { 'h', 'a', 'i', 'b', 'j', 'c' };
        REQUIRE(lz::equal(interleaved1, expected));

        expected = { 'a', 'h', 'b', 'i', 'c', 'j' };
        auto interleaved2 = lz::interleave(str3, str5);
        REQUIRE(lz::equal(interleaved2, expected));
    }

    SUBCASE("Permutation 4: 3 vs 4 vs 5 characters and 5 vs 4 vs 3 characters") {
        auto interleaved1 = lz::interleave(str3, str4, str5);
        static_assert(!std::is_same<decltype(interleaved1.begin()), decltype(interleaved1.end())>::value, "Must be sentinel");

        std::vector<char> expected = { 'a', 'd', 'h', 'b', 'e', 'i', 'c', 'f', 'j' };
        REQUIRE(lz::equal(interleaved1, expected));

        expected = { 'h', 'd', 'a', 'i', 'e', 'b', 'j', 'f', 'c' };
        auto interleaved2 = lz::interleave(str5, str4, str3);
        REQUIRE(lz::equal(interleaved2, expected));
    }

    SUBCASE("Permutation 5: 4 vs 3 vs 5 characters and 5 vs 3 vs 4 characters") {
        auto interleaved1 = lz::interleave(str4, str3, str5);
        static_assert(!std::is_same<decltype(interleaved1.begin()), decltype(interleaved1.end())>::value, "Must be sentinel");

        std::vector<char> expected = { 'd', 'a', 'h', 'e', 'b', 'i', 'f', 'c', 'j' };
        REQUIRE(lz::equal(interleaved1, expected));

        expected = { 'h', 'a', 'd', 'i', 'b', 'e', 'j', 'c', 'f' };
        auto interleaved2 = lz::interleave(str5, str3, str4);
        REQUIRE(lz::equal(interleaved2, expected));
    }

    SUBCASE("Permutation 6: 3 vs 5 vs 4 characters and 4 vs 5 vs 3 characters") {
        auto interleaved1 = lz::interleave(str3, str5, str4);
        static_assert(!std::is_same<decltype(interleaved1.begin()), decltype(interleaved1.end())>::value, "Must be sentinel");

        std::vector<char> expected = { 'a', 'h', 'd', 'b', 'i', 'e', 'c', 'j', 'f' };
        REQUIRE(lz::equal(interleaved1, expected));

        expected = { 'd', 'h', 'a', 'e', 'i', 'b', 'f', 'j', 'c' };
        auto interleaved2 = lz::interleave(str4, str5, str3);
        REQUIRE(lz::equal(interleaved2, expected));
    }
}

TEST_CASE("Empty or one element") {
    SUBCASE("All empty") {
        std::vector<int> a, b, c;
        auto interleaved = lz::interleave(a, b, c);
        using t = decltype(*interleaved.begin());
        static_assert(std::is_same<t, int&>::value, "Should be int&");

        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE_FALSE(lz::has_one(interleaved));
        REQUIRE_FALSE(lz::has_many(interleaved));
    }

    SUBCASE("One empty first") {
        std::vector<int> a = { 1 }, b, c;
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE_FALSE(lz::has_one(interleaved));
        REQUIRE_FALSE(lz::has_many(interleaved));
    }

    SUBCASE("One empty second") {
        std::vector<int> a, b = { 1 }, c;
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE_FALSE(lz::has_one(interleaved));
        REQUIRE_FALSE(lz::has_many(interleaved));
    }

    SUBCASE("One empty third") {
        std::vector<int> a, b, c = { 1 };
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE_FALSE(lz::has_one(interleaved));
        REQUIRE_FALSE(lz::has_many(interleaved));
    }

    SUBCASE("One empty third") {
        std::vector<int> a, b, c = { 1 };
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE_FALSE(lz::has_one(interleaved));
        REQUIRE_FALSE(lz::has_many(interleaved));
    }

    SUBCASE("First empty, second and third one") {
        std::vector<int> a, b = { 1 }, c = { 2 };
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE_FALSE(lz::has_one(interleaved));
        REQUIRE_FALSE(lz::has_many(interleaved));
    }

    SUBCASE("First one, second empty and third one") {
        std::vector<int> a = { 1 }, b, c = { 2 };
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE_FALSE(lz::has_one(interleaved));
        REQUIRE_FALSE(lz::has_many(interleaved));
    }

    SUBCASE("First one, second one and third empty") {
        std::vector<int> a = { 1 }, b = { 2 }, c;
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE_FALSE(lz::has_one(interleaved));
        REQUIRE_FALSE(lz::has_many(interleaved));
    }

    SUBCASE("All one") {
        std::vector<int> a = { 1 }, b = { 2 }, c = { 3 };
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 3);
        REQUIRE_FALSE(lz::empty(interleaved));
        REQUIRE_FALSE(lz::has_one(interleaved));
        REQUIRE(lz::has_many(interleaved));
    }
}

TEST_CASE("Non sentinelled, forward, backward") {
    std::vector<int> a = { 1, 2, 3 }, b = { 4, 5, 6, 7 }, c = { 8, 9, 10, 11, 12 };

    SUBCASE("Operator++/--, permutation 1: 3 vs 4 vs 5 items vs 4 vs 3 vs 5 items") {
        std::vector<int> expected = { 1, 4, 8, 2, 5, 9, 3, 6, 10 };
        auto interleaved = lz::interleave(a, b, c);

        static_assert(std::is_same<decltype(interleaved.begin()), decltype(interleaved.end())>::value, "Must not be sentinel");

        REQUIRE(lz::equal(interleaved, expected));
        REQUIRE(lz::equal(interleaved | lz::reverse, expected | lz::reverse));

        expected = { 4, 1, 8, 5, 2, 9, 6, 3, 10 };
        interleaved = lz::interleave(b, a, c);
        REQUIRE(lz::equal(interleaved, expected));
        REQUIRE(lz::equal(interleaved | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("Operator++/--, permutation 2: 4 vs 5 vs 3 items vs 5 vs 4 vs 3 items") {
        std::vector<int> expected = { 4, 8, 1, 5, 9, 2, 6, 10, 3 };
        auto interleaved = lz::interleave(b, c, a);
        REQUIRE(lz::equal(interleaved, expected));
        REQUIRE(lz::equal(interleaved | lz::reverse, expected | lz::reverse));

        expected = { 8, 4, 1, 9, 5, 2, 10, 6, 3 };
        interleaved = lz::interleave(c, b, a);
        REQUIRE(lz::equal(interleaved, expected));
        REQUIRE(lz::equal(interleaved | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("Operator++/--, permutation 3: 5 vs 3 vs 4 items vs 3 vs 5 vs 4 items") {
        std::vector<int> expected = { 8, 1, 4, 9, 2, 5, 10, 3, 6 };
        auto interleaved = lz::interleave(c, a, b);
        REQUIRE(lz::equal(interleaved, expected));
        REQUIRE(lz::equal(interleaved | lz::reverse, expected | lz::reverse));

        expected = { 1, 8, 4, 2, 9, 5, 3, 10, 6 };
        interleaved = lz::interleave(a, c, b);
        REQUIRE(lz::equal(interleaved, expected));
        REQUIRE(lz::equal(interleaved | lz::reverse, expected | lz::reverse));
    }
}

TEST_CASE("Non sentinelled, operator+/-. length 4, 5, 6") {
    std::vector<int> a = { 1, 2, 3, 4 }, b = { 5, 6, 7, 8, 9 }, c = { 10, 11, 12, 13, 14, 15 };

    SUBCASE("Permutation 1: a, b, c") {
        auto interleaved = lz::interleave(a, b, c);
        static_assert(lz::detail::is_ra<decltype(interleaved.begin())>::value, "Must be random access");
        auto expected = { 1, 5, 10, 2, 6, 11, 3, 7, 12, 4, 8, 13 };
        REQUIRE(lz::size(interleaved) == lz::size(expected));
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 2: b, a, c") {
        auto interleaved = lz::interleave(b, a, c);
        auto expected = { 5, 1, 10, 6, 2, 11, 7, 3, 12, 8, 4, 13 };
        REQUIRE(lz::size(interleaved) == lz::size(expected));
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 3: b, c, a") {
        auto interleaved = lz::interleave(b, c, a);
        std::vector<int> expected = { 5, 10, 1, 6, 11, 2, 7, 12, 3, 8, 13, 4 };
        REQUIRE(lz::size(interleaved) == lz::size(expected));
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 4: c, b, a") {
        auto interleaved = lz::interleave(c, b, a);
        auto expected = { 10, 5, 1, 11, 6, 2, 12, 7, 3, 13, 8, 4 };
        REQUIRE(lz::size(interleaved) == lz::size(expected));
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 5: c, a, b") {
        auto interleaved = lz::interleave(c, a, b);
        std::vector<int> expected = { 10, 1, 5, 11, 2, 6, 12, 3, 7, 13, 4, 8 };
        REQUIRE(lz::size(interleaved) == lz::size(expected));
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 6: a, c, b") {
        auto interleaved = lz::interleave(a, c, b);
        auto expected = { 1, 10, 5, 2, 11, 6, 3, 12, 7, 4, 13, 8 };
        REQUIRE(lz::size(interleaved) == lz::size(expected));
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }
}

TEST_CASE("Non sentinelled, operator+/-. length 4, 5, 6, 7") {
    std::vector<int> a = { 1, 2, 3, 4 }, b = { 5, 6, 7, 8, 9 }, c = { 10, 11, 12, 13, 14, 15 },
                     d = { 16, 17, 18, 19, 20, 21, 22 };

    SUBCASE("Permutation 1: a, b, c, d") {
        auto interleaved = lz::interleave(a, b, c, d);
        std::vector<int> expected = { 1, 5, 10, 16, 2, 6, 11, 17, 3, 7, 12, 18, 4, 8, 13, 19 };
        REQUIRE(lz::size(interleaved) == lz::size(expected));
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 2: b, c, d, a") {
        auto interleaved = lz::interleave(b, c, d, a);
        std::vector<int> expected = { 5, 10, 16, 1, 6, 11, 17, 2, 7, 12, 18, 3, 8, 13, 19, 4 };
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 3: c, d, a, b") {
        auto interleaved = lz::interleave(c, d, a, b);
        std::vector<int> expected = { 10, 16, 1, 5, 11, 17, 2, 6, 12, 18, 3, 7, 13, 19, 4, 8 };
        REQUIRE(lz::size(interleaved) == lz::size(expected));
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 4: d, a, b, c") {
        auto interleaved = lz::interleave(d, a, b, c);
        std::vector<int> expected = { 16, 1, 5, 10, 17, 2, 6, 11, 18, 3, 7, 12, 19, 4, 8, 13 };
        REQUIRE(lz::size(interleaved) == lz::size(expected));
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 5: a, d, b, c") {
        auto interleaved = lz::interleave(a, d, b, c);
        std::vector<int> expected = { 1, 16, 5, 10, 2, 17, 6, 11, 3, 18, 7, 12, 4, 19, 8, 13 };
        REQUIRE(lz::size(interleaved) == lz::size(expected));
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 6: d, c, b, a") {
        auto interleaved = lz::interleave(d, c, b, a);
        std::vector<int> expected = { 16, 10, 5, 1, 17, 11, 6, 2, 18, 12, 7, 3, 19, 13, 8, 4 };
        REQUIRE(lz::size(interleaved) == lz::size(expected));
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 7: c, b, a, d") {
        auto interleaved = lz::interleave(c, b, a, d);
        std::vector<int> expected = { 10, 5, 1, 16, 11, 6, 2, 17, 12, 7, 3, 18, 13, 8, 4, 19 };
        REQUIRE(lz::size(interleaved) == lz::size(expected));
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 8: b, a, d, c") {
        auto interleaved = lz::interleave(b, a, d, c);
        std::vector<int> expected = { 5, 1, 16, 10, 6, 2, 17, 11, 7, 3, 18, 12, 8, 4, 19, 13 };
        REQUIRE(lz::size(interleaved) == lz::size(expected));
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 9: a, c, d, b") {
        auto interleaved = lz::interleave(a, c, d, b);
        std::vector<int> expected = { 1, 10, 16, 5, 2, 11, 17, 6, 3, 12, 18, 7, 4, 13, 19, 8 };
        REQUIRE(lz::size(interleaved) == lz::size(expected));
        test_procs::test_operator_plus(interleaved, expected);
        test_procs::test_operator_minus(interleaved);
    }
}

TEST_CASE("Sentinelled, operator+/-. length 4, 5, 6, 7") {
    auto a = lz::repeat(0, 4), b = lz::repeat(0, 5), c = lz::repeat(0, 6), d = lz::repeat(0, 7);

    SUBCASE("Permutation 1: a, b, c, d") {
        auto interleaved = lz::interleave(a, b, c, d);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 2: b, c, d, a") {
        auto interleaved = lz::interleave(b, c, d, a);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 3: c, d, a, b") {
        auto interleaved = lz::interleave(c, d, a, b);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 4: d, a, b, c") {
        auto interleaved = lz::interleave(d, a, b, c);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 5: a, d, b, c") {
        auto interleaved = lz::interleave(a, d, b, c);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 6: d, c, b, a") {
        auto interleaved = lz::interleave(d, c, b, a);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 7: c, b, a, d") {
        auto interleaved = lz::interleave(c, b, a, d);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 8: b, a, d, c") {
        auto interleaved = lz::interleave(b, a, d, c);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 9: a, c, d, b") {
        auto interleaved = lz::interleave(a, c, d, b);
        test_procs::test_operator_minus(interleaved);
    }
}

TEST_CASE("Sentinelled, operator+/-. length 4, 5, 6") {
    auto a = lz::repeat(0, 4), b = lz::repeat(0, 5), c = lz::repeat(0, 6);

    SUBCASE("Permutation 1: a, b, c") {
        auto interleaved = lz::interleave(a, b, c);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 2: b, a, c") {
        auto interleaved = lz::interleave(b, a, c);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 3: b, c, a") {
        auto interleaved = lz::interleave(b, c, a);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 4: c, b, a") {
        auto interleaved = lz::interleave(c, b, a);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 5: c, a, b") {
        auto interleaved = lz::interleave(c, a, b);
        test_procs::test_operator_minus(interleaved);
    }

    SUBCASE("Permutation 6: a, c, b") {
        auto interleaved = lz::interleave(a, c, b);
        test_procs::test_operator_minus(interleaved);
    }
}
