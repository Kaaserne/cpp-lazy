#include <Lz/exclude.hpp>
#include <Lz/c_string.hpp>
#include <catch2/catch.hpp>
#include <list>


TEST_CASE("Exclude with sentinels") {
    auto cstr = lz::c_string("a string to exclude");
    auto excluded = lz::exclude(cstr, 3, 5);
    REQUIRE(excluded.to_string() == "a sing to exclude");
}

TEST_CASE("Exclude changing and creating elements", "[Exclude][Basic functionality]") {
    std::array<int, 10> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto excluded1 = lz::exclude(arr, 3, 5);
    auto excluded2 = lz::exclude(arr, 0, 2);
    auto excluded3 = lz::exclude(arr, 8, 10);
    static_assert(!std::is_same<decltype(excluded1.end()), decltype(excluded1.begin())>::value, "Must be sentinel");

    SECTION("Should Exclude out element") {
        constexpr auto signed_size = static_cast<std::ptrdiff_t>(arr.size() - 2);
        REQUIRE(lz::distance(excluded1.begin(), excluded1.end()) == signed_size);
        REQUIRE(lz::distance(excluded2.begin(), excluded2.end()) == signed_size);
        REQUIRE(lz::distance(excluded3.begin(), excluded3.end()) == signed_size);
    }

    SECTION("Should be by reference") {
        *excluded2.begin() = 0;
        REQUIRE(arr[2] == 0);
    }
}

TEST_CASE("Exclude binary operations", "[Exclude][Binary ops]") {
    std::array<int, 10> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto excluded1 = lz::exclude(arr, 3, 5);
    auto exBeg1 = excluded1.begin();
    auto exEnd1 = excluded1.end();

    auto excluded2 = lz::exclude(arr, 0, 2);
    auto exBeg2 = excluded2.begin();
    auto exEnd2 = excluded2.end();

    auto excluded3 = lz::exclude(arr, 7, 10);
    auto exBeg3 = excluded3.begin();
    auto exEnd3 = excluded3.end();

    SECTION("Operator++") {
        REQUIRE(*exBeg1 == 1);
        ++exBeg1, ++exBeg1, ++exBeg1;
        REQUIRE(*exBeg1 == 6);
        REQUIRE(*++exBeg1 == 7);

        REQUIRE(*exBeg2 == 3);
        REQUIRE(++*exBeg2 == 4);

        REQUIRE(*exBeg3 == 1);
        ++exBeg3, ++exBeg3, ++exBeg3, ++exBeg3, ++exBeg3, ++exBeg3, ++exBeg3;
        REQUIRE(exBeg3 == excluded3.end());
    }

    SECTION("Operator== & operator!=") {
        REQUIRE(exBeg1 != exEnd1);
        REQUIRE(exBeg2 != exEnd2);
        REQUIRE(exBeg3 != exEnd3);
    }

    SECTION("next") {
        REQUIRE(*std::next(exBeg1, 0) == 1);
        REQUIRE(*std::next(exBeg1, 3) == 6);
        REQUIRE(*std::next(exBeg1, 2) == 3);
        REQUIRE(*std::next(exBeg1, 5) == 8);
        REQUIRE(*std::next(exBeg1, 6) == 9);

        REQUIRE(*std::next(exBeg2, 0) == 3);
        REQUIRE(*std::next(exBeg2, 1) == 4);
        REQUIRE(*std::next(exBeg2, 2) == 5);
        REQUIRE(*std::next(exBeg2, 3) == 6);

        REQUIRE(*std::next(exBeg3, 0) == 1);
        REQUIRE(*std::next(exBeg3, 5) == 6);
        REQUIRE(*std::next(exBeg3, 6) == 7);
        REQUIRE(std::next(exBeg3, 7) == exEnd3);
    }

    SECTION("Lz distance") {
        REQUIRE(lz::distance(exBeg1, exEnd1) == 8);
        REQUIRE(lz::distance(std::next(exBeg1, 3), exEnd1) == 5);

        REQUIRE(lz::distance(exBeg2, exEnd2) == 8);
        REQUIRE(lz::distance(std::next(exBeg2, 1), exEnd2) == 7);

        REQUIRE(lz::distance(exBeg3, exEnd3) == 7);
        REQUIRE(lz::distance(std::next(exBeg3, 7), exEnd3) == 0);
    }
}

TEST_CASE("Empty or one element exclude") {
    SECTION("Empty") {
        std::array<int, 0> arr = {};
        auto excluded = lz::exclude(arr, 0, 1);
        REQUIRE(excluded.begin() == excluded.end());
    }

    SECTION("One element") {
        std::array<int, 1> arr = { 1 };
        auto excluded = lz::exclude(arr, 0, 1);
        REQUIRE(excluded.begin() == excluded.end());
    }
}

TEST_CASE("Exclude to containers", "[Exclude][To container]") {
    std::array<int, 10> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto excluded1 = lz::exclude(arr, 3, 5);
    auto excluded2 = lz::exclude(arr, 0, 2);
    auto excluded3 = lz::exclude(arr, 7, 10);

    SECTION("To array") {
        REQUIRE(excluded1.to<std::array<int, 8>>() == std::array<int, 8>{ 1, 2, 3, 6, 7, 8, 9, 10 });
        REQUIRE(excluded2.to<std::array<int, 8>>() == std::array<int, 8>{ 3, 4, 5, 6, 7, 8, 9, 10 });
        REQUIRE(excluded3.to<std::array<int, 7>>() == std::array<int, 7>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To vector") {
        REQUIRE(excluded1.to_vector() == std::vector<int>{ 1, 2, 3, 6, 7, 8, 9, 10 });
        REQUIRE(excluded2.to_vector() == std::vector<int>{ 3, 4, 5, 6, 7, 8, 9, 10 });
        REQUIRE(excluded3.to_vector() == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To other container using to<>()") {
        REQUIRE(excluded1.to<std::list<int>>() == std::list<int>{ 1, 2, 3, 6, 7, 8, 9, 10 });
        REQUIRE(excluded2.to<std::list<int>>() == std::list<int>{ 3, 4, 5, 6, 7, 8, 9, 10 });
        REQUIRE(excluded3.to<std::list<int>>() == std::list<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To map") {
        REQUIRE(excluded1.to_map([](int i) { return std::make_pair(i, i); }) ==
                std::map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });
        REQUIRE(excluded2.to_map([](int i) { return std::make_pair(i, i); }) ==
                std::map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });
        REQUIRE(excluded3.to_map([](int i) { return std::make_pair(i, i); }) ==
                std::map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 } });
    }

    SECTION("To unordered map") {
        REQUIRE(excluded1.to_unordered_map([](int i) { return std::make_pair(i, i); }) ==
                std::unordered_map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });
        REQUIRE(excluded2.to_unordered_map([](int i) { return std::make_pair(i, i); }) ==
                std::unordered_map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });
        REQUIRE(excluded3.to_unordered_map([](int i) { return std::make_pair(i, i); }) ==
                std::unordered_map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 } });
    }
}