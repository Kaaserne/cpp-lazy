#include "Lz/algorithm.hpp"

#include <Lz/c_string.hpp>
#include <Lz/interleaved.hpp>
#include <catch2/catch.hpp>

TEST_CASE("Interleaved with sentinels permutations") {
    auto str3 = lz::c_string("abc");
    auto str4 = lz::c_string("defg");
    auto str5 = lz::c_string("hijkl");

    SECTION("Operator=") {
        auto interleaved_3_4 = lz::interleave(str3, str4);
        auto it_3_4 = interleaved_3_4.begin();
        REQUIRE(it_3_4 == interleaved_3_4.begin());
        it_3_4 = interleaved_3_4.end();
        REQUIRE(it_3_4 == interleaved_3_4.end());

        auto interleaved_4_3 = lz::interleave(str4, str3);
        auto it_4_3 = interleaved_4_3.begin();
        REQUIRE(it_4_3 == interleaved_4_3.begin());
        it_4_3 = interleaved_4_3.end();
        REQUIRE(it_4_3 == interleaved_4_3.end());

        auto interleaved_3_4_5 = lz::interleave(str3, str4, str5);
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

    SECTION("Permutation 1: 3 vs 4 characters and 4 vs 3 characters") {
        auto interleaved = lz::interleave(str3, str4);
        static_assert(!std::is_same<decltype(interleaved.begin()), decltype(interleaved.end())>::value, "Must be sentinel");

        std::vector<char> expected = { 'a', 'd', 'b', 'e', 'c', 'f' };
        REQUIRE(lz::equal(interleaved, expected));

        expected = { 'd', 'a', 'e', 'b', 'f', 'c' };
        interleaved = lz::interleave(str4, str3);
        REQUIRE(lz::equal(interleaved, expected));
    }

    SECTION("Permutation 2: 4 vs 5 characters and 5 vs 4 characters") {
        auto interleaved = lz::interleave(str4, str5);
        static_assert(!std::is_same<decltype(interleaved.begin()), decltype(interleaved.end())>::value, "Must be sentinel");

        std::vector<char> expected = { 'd', 'h', 'e', 'i', 'f', 'j', 'g', 'k' };
        REQUIRE(lz::equal(interleaved, expected));

        expected = { 'h', 'd', 'i', 'e', 'j', 'f', 'k', 'g' };
        interleaved = lz::interleave(str5, str4);
        REQUIRE(lz::equal(interleaved, expected));
    }

    SECTION("Permutation 3: 5 vs 3 characters and 3 vs 5 characters") {
        auto interleaved = lz::interleave(str5, str3);
        static_assert(!std::is_same<decltype(interleaved.begin()), decltype(interleaved.end())>::value, "Must be sentinel");

        std::vector<char> expected = { 'h', 'a', 'i', 'b', 'j', 'c' };
        REQUIRE(lz::equal(interleaved, expected));

        expected = { 'a', 'h', 'b', 'i', 'c', 'j' };
        interleaved = lz::interleave(str3, str5);
        REQUIRE(lz::equal(interleaved, expected));
    }

    SECTION("Permutation 4: 3 vs 4 vs 5 characters and 5 vs 4 vs 3 characters") {
        auto interleaved = lz::interleave(str3, str4, str5);
        static_assert(!std::is_same<decltype(interleaved.begin()), decltype(interleaved.end())>::value, "Must be sentinel");

        std::vector<char> expected = { 'a', 'd', 'h', 'b', 'e', 'i', 'c', 'f', 'j' };
        REQUIRE(lz::equal(interleaved, expected));

        expected = { 'h', 'd', 'a', 'i', 'e', 'b', 'j', 'f', 'c' };
        interleaved = lz::interleave(str5, str4, str3);
        REQUIRE(lz::equal(interleaved, expected));
    }

    SECTION("Permutation 5: 4 vs 3 vs 5 characters and 5 vs 3 vs 4 characters") {
        auto interleaved = lz::interleave(str4, str3, str5);
        static_assert(!std::is_same<decltype(interleaved.begin()), decltype(interleaved.end())>::value, "Must be sentinel");

        std::vector<char> expected = { 'd', 'a', 'h', 'e', 'b', 'i', 'f', 'c', 'j' };
        REQUIRE(lz::equal(interleaved, expected));

        expected = { 'h', 'a', 'd', 'i', 'b', 'e', 'j', 'c', 'f' };
        interleaved = lz::interleave(str5, str3, str4);
        REQUIRE(lz::equal(interleaved, expected));
    }

    SECTION("Permutation 6: 3 vs 5 vs 4 characters and 4 vs 5 vs 3 characters") {
        auto interleaved = lz::interleave(str3, str5, str4);
        static_assert(!std::is_same<decltype(interleaved.begin()), decltype(interleaved.end())>::value, "Must be sentinel");

        std::vector<char> expected = { 'a', 'h', 'd', 'b', 'i', 'e', 'c', 'j', 'f' };
        REQUIRE(lz::equal(interleaved, expected));

        expected = { 'd', 'h', 'a', 'e', 'i', 'b', 'f', 'j', 'c' };
        interleaved = lz::interleave(str4, str5, str3);
        REQUIRE(lz::equal(interleaved, expected));
    }
}

TEST_CASE("Empty or one element") {
    SECTION("All empty") {
        std::vector<int> a, b, c;
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE(!lz::has_one(interleaved));
        REQUIRE(!lz::has_many(interleaved));
    }

    SECTION("One empty first") {
        std::vector<int> a = { 1 }, b, c;
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE(!lz::has_one(interleaved));
        REQUIRE(!lz::has_many(interleaved));
    }

    SECTION("One empty second") {
        std::vector<int> a, b = { 1 }, c;
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE(!lz::has_one(interleaved));
        REQUIRE(!lz::has_many(interleaved));
    }

    SECTION("One empty third") {
        std::vector<int> a, b, c = { 1 };
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE(!lz::has_one(interleaved));
        REQUIRE(!lz::has_many(interleaved));
    }

    SECTION("One empty third") {
        std::vector<int> a, b, c = { 1 };
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE(!lz::has_one(interleaved));
        REQUIRE(!lz::has_many(interleaved));
    }

    SECTION("First empty, second and third one") {
        std::vector<int> a, b = { 1 }, c = { 2 };
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE(!lz::has_one(interleaved));
        REQUIRE(!lz::has_many(interleaved));
    }

    SECTION("First one, second empty and third one") {
        std::vector<int> a = { 1 }, b, c = { 2 };
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE(!lz::has_one(interleaved));
        REQUIRE(!lz::has_many(interleaved));
    }

    SECTION("First one, second one and third empty") {
        std::vector<int> a = { 1 }, b = { 2 }, c;
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 0);
        REQUIRE(lz::empty(interleaved));
        REQUIRE(!lz::has_one(interleaved));
        REQUIRE(!lz::has_many(interleaved));
    }

    SECTION("All one") {
        std::vector<int> a = { 1 }, b = { 2 }, c = { 3 };
        auto interleaved = lz::interleave(a, b, c);
        REQUIRE(interleaved.size() == 3);
        REQUIRE(!lz::empty(interleaved));
        REQUIRE(!lz::has_one(interleaved));
        REQUIRE(lz::has_many(interleaved));
    }
}
