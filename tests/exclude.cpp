#include <Lz/c_string.hpp>
#include <Lz/exclude.hpp>
#include <Lz/iter_tools.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Exclude with sentinels") {
    auto cstr = lz::c_string("a string to exclude");
    auto excluded = lz::exclude(cstr, 3, 5);
    REQUIRE((excluded | lz::to<std::string>()) == "a sing to exclude");
    static_assert(std::is_same<decltype(excluded.end()), lz::default_sentinel>::value, "Should be default sentinel");
}

TEST_CASE("Exclude changing and creating elements") {
    std::array<int, 10> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto excluded1 = lz::exclude(arr, 3, 5);
    static_assert(!std::is_same<decltype(excluded1.end()), lz::default_sentinel>::value, "Should not be default sentinel");
    auto excluded2 = lz::exclude(arr, 0, 2);
    auto excluded3 = lz::exclude(arr, 8, 10);

    std::vector<int> excluded_expected1 = { 1, 2, 3, 6, 7, 8, 9, 10 };
    REQUIRE(lz::equal(excluded1, excluded_expected1));

    std::vector<int> excluded_expected2 = { 3, 4, 5, 6, 7, 8, 9, 10 };
    REQUIRE(lz::equal(excluded2, excluded_expected2));

    std::vector<int> excluded_expected3 = { 1, 2, 3, 4, 5, 6, 7, 8 };
    REQUIRE(lz::equal(excluded3, excluded_expected3));

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

    SECTION("Operator=") {
        auto it = excluded1.begin();
        REQUIRE(it == excluded1.begin());
        it = excluded1.end();
        REQUIRE(it == excluded1.end());

        it = excluded2.begin();
        REQUIRE(it == excluded2.begin());
        it = excluded2.end();
        REQUIRE(it == excluded2.end());

        it = excluded3.begin();
        REQUIRE(it == excluded3.begin());
        it = excluded3.end();
        REQUIRE(it == excluded3.end());

        auto it2 = excluded_expected1.begin();
        REQUIRE(it2 == excluded_expected1.begin());
        it2 = excluded_expected1.end();
        REQUIRE(it2 == excluded_expected1.end());

        it2 = excluded_expected2.begin();
        REQUIRE(it2 == excluded_expected2.begin());
        it2 = excluded_expected2.end();
        REQUIRE(it2 == excluded_expected2.end());

        it2 = excluded_expected3.begin();
        REQUIRE(it2 == excluded_expected3.begin());
        it2 = excluded_expected3.end();
        REQUIRE(it2 == excluded_expected3.end());
    }
}

TEST_CASE("Exclude binary operations") {
    std::array<int, 10> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto excluded1 = arr | lz::exclude(3, 5); // { 1, 2, 3, 6, 7, 8, 9, 10 }
    auto excluded2 = lz::exclude(arr, 0, 2); // { 3, 4, 5, 6, 7, 8, 9, 10 }
    auto excluded3 = lz::exclude(arr, 7, 10); // { 1, 2, 3, 4, 5, 6, 7 }

    auto exBeg1 = excluded1.begin();
    auto exEnd1 = excluded1.end();
    auto exBeg2 = excluded2.begin();
    auto exEnd2 = excluded2.end();
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

    SECTION("Operator--") {
        auto vec = lz::reverse(excluded1) | lz::to<std::vector>();
        std::vector<int> expected = { 10, 9, 8, 7, 6, 3, 2, 1 };
        REQUIRE(lz::equal(vec, expected));

        vec = lz::reverse(excluded2) | lz::to<std::vector>();
        expected = { 10, 9, 8, 7, 6, 5, 4, 3 };
        REQUIRE(lz::equal(vec, expected));

        vec = lz::reverse(excluded3) | lz::to<std::vector>();
        expected = { 7, 6, 5, 4, 3, 2, 1 };
        REQUIRE(lz::equal(vec, expected));
    }

    SECTION("Operator+") {
        std::vector<int> expected = { 1, 2, 3, 6, 7, 8, 9, 10 };
        for (std::size_t i = 0; i < lz::size(excluded1); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(exBeg1 + static_cast<std::ptrdiff_t>(i)) == *(expected.begin() + static_cast<std::ptrdiff_t>(i)));
        }
        for (std::size_t i = 1; i <= lz::size(excluded1); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(exEnd1 - static_cast<std::ptrdiff_t>(i)) == *(expected.end() - static_cast<std::ptrdiff_t>(i)));
        }

        std::advance(exBeg1, static_cast<std::ptrdiff_t>(lz::size(excluded1)));
        std::advance(exEnd1, -static_cast<std::ptrdiff_t>(lz::size(excluded1)));
        for (std::size_t i = 1; i <= lz::size(excluded1); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(exBeg1 - static_cast<std::ptrdiff_t>(i)) == *(expected.end() - static_cast<std::ptrdiff_t>(i)));
        }
        for (std::size_t i = 0; i < lz::size(excluded1); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(exEnd1 + static_cast<std::ptrdiff_t>(i)) == *(expected.begin() + static_cast<std::ptrdiff_t>(i)));
        }

        expected = { 3, 4, 5, 6, 7, 8, 9, 10 };
        for (std::size_t i = 0; i < lz::size(excluded2); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(exBeg2 + static_cast<std::ptrdiff_t>(i)) == *(expected.begin() + static_cast<std::ptrdiff_t>(i)));
        }
        for (std::size_t i = 1; i <= lz::size(excluded2); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(exEnd2 - static_cast<std::ptrdiff_t>(i)) == *(expected.end() - static_cast<std::ptrdiff_t>(i)));
        }

        std::advance(exBeg2, static_cast<std::ptrdiff_t>(lz::size(excluded2)));
        std::advance(exEnd2, -static_cast<std::ptrdiff_t>(lz::size(excluded2)));
        for (std::size_t i = 1; i <= lz::size(excluded2); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(exBeg2 - static_cast<std::ptrdiff_t>(i)) == *(expected.end() - static_cast<std::ptrdiff_t>(i)));
        }
        for (std::size_t i = 0; i < lz::size(excluded2); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(exEnd2 + static_cast<std::ptrdiff_t>(i)) == *(expected.begin() + static_cast<std::ptrdiff_t>(i)));
        }

        expected = { 1, 2, 3, 4, 5, 6, 7 };
        for (std::size_t i = 0; i < lz::size(excluded3); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(exBeg3 + static_cast<std::ptrdiff_t>(i)) == *(expected.begin() + static_cast<std::ptrdiff_t>(i)));
        }
        for (std::size_t i = 1; i <= lz::size(excluded3); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(exEnd3 - static_cast<std::ptrdiff_t>(i)) == *(expected.end() - static_cast<std::ptrdiff_t>(i)));
        }

        std::advance(exBeg3, static_cast<std::ptrdiff_t>(lz::size(excluded3)));
        std::advance(exEnd3, -static_cast<std::ptrdiff_t>(lz::size(excluded3)));

        for (std::size_t i = 1; i <= lz::size(excluded3); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(exBeg3 - static_cast<std::ptrdiff_t>(i)) == *(expected.end() - static_cast<std::ptrdiff_t>(i)));
        }
        for (std::size_t i = 0; i < lz::size(excluded3); ++i) {
            INFO("With i = " << i);
            REQUIRE(*(exEnd3 + static_cast<std::ptrdiff_t>(i)) == *(expected.begin() + static_cast<std::ptrdiff_t>(i)));
        }
    }

    SECTION("Operator-") {
        for (std::size_t i = 0; i < lz::size(excluded1); ++i) {
            INFO("With i = " << i);
            REQUIRE((exBeg1 + static_cast<std::ptrdiff_t>(i)) - exEnd1 == -static_cast<std::ptrdiff_t>(lz::size(excluded1) - i));
            REQUIRE((exEnd1 - static_cast<std::ptrdiff_t>(i)) - exBeg1 == static_cast<std::ptrdiff_t>(lz::size(excluded1) - i));
            REQUIRE(exEnd1 - (exBeg1 + static_cast<std::ptrdiff_t>(i)) == static_cast<std::ptrdiff_t>(lz::size(excluded1) - i));
            REQUIRE(exBeg1 - (exEnd1 - static_cast<std::ptrdiff_t>(i)) == -static_cast<std::ptrdiff_t>(lz::size(excluded1) - i));
        }

        for (std::size_t i = 0; i < lz::size(excluded2); ++i) {
            INFO("With i = " << i);
            REQUIRE((exBeg2 + static_cast<std::ptrdiff_t>(i)) - exEnd2 == -static_cast<std::ptrdiff_t>(lz::size(excluded2) - i));
            REQUIRE((exEnd2 - static_cast<std::ptrdiff_t>(i)) - exBeg2 == static_cast<std::ptrdiff_t>(lz::size(excluded2) - i));
            REQUIRE(exEnd2 - (exBeg2 + static_cast<std::ptrdiff_t>(i)) == static_cast<std::ptrdiff_t>(lz::size(excluded2) - i));
            REQUIRE(exBeg2 - (exEnd2 - static_cast<std::ptrdiff_t>(i)) == -static_cast<std::ptrdiff_t>(lz::size(excluded2) - i));
        }

        for (std::size_t i = 0; i < lz::size(excluded3); ++i) {
            INFO("With i = " << i);
            REQUIRE((exBeg3 + static_cast<std::ptrdiff_t>(i)) - exEnd3 == -static_cast<std::ptrdiff_t>(lz::size(excluded3) - i));
            REQUIRE((exEnd3 - static_cast<std::ptrdiff_t>(i)) - exBeg3 == static_cast<std::ptrdiff_t>(lz::size(excluded3) - i));
            REQUIRE(exEnd3 - (exBeg3 + static_cast<std::ptrdiff_t>(i)) == static_cast<std::ptrdiff_t>(lz::size(excluded3) - i));
            REQUIRE(exBeg3 - (exEnd3 - static_cast<std::ptrdiff_t>(i)) == -static_cast<std::ptrdiff_t>(lz::size(excluded3) - i));
        }

        for (std::size_t i = 0; i < lz::size(excluded1); ++i) {
            INFO("With i = " << i);
            REQUIRE((exEnd1 - static_cast<std::ptrdiff_t>(i)) - (exBeg1 + static_cast<std::ptrdiff_t>(i)) ==
                    static_cast<std::ptrdiff_t>(lz::size(excluded1) - 2 * i));
            REQUIRE((exBeg1 + static_cast<std::ptrdiff_t>(i)) - (exEnd1 - static_cast<std::ptrdiff_t>(i)) ==
                    -static_cast<std::ptrdiff_t>(lz::size(excluded1) - 2 * i));
        }
        for (std::size_t i = 0; i < lz::size(excluded2); ++i) {
            INFO("With i = " << i);
            REQUIRE((exEnd2 - static_cast<std::ptrdiff_t>(i)) - (exBeg2 + static_cast<std::ptrdiff_t>(i)) ==
                    static_cast<std::ptrdiff_t>(lz::size(excluded2) - 2 * i));
            REQUIRE((exBeg2 + static_cast<std::ptrdiff_t>(i)) - (exEnd2 - static_cast<std::ptrdiff_t>(i)) ==
                    -static_cast<std::ptrdiff_t>(lz::size(excluded2) - 2 * i));
        }
        for (std::size_t i = 0; i < lz::size(excluded3); ++i) {
            INFO("With i = " << i);
            REQUIRE((exEnd3 - static_cast<std::ptrdiff_t>(i)) - (exBeg3 + static_cast<std::ptrdiff_t>(i)) ==
                    static_cast<std::ptrdiff_t>(lz::size(excluded3) - 2 * i));
            REQUIRE((exBeg3 + static_cast<std::ptrdiff_t>(i)) - (exEnd3 - static_cast<std::ptrdiff_t>(i)) ==
                    -static_cast<std::ptrdiff_t>(lz::size(excluded3) - 2 * i));
        }
    }

    SECTION("Operator== & operator!=") {
        REQUIRE(exBeg1 != exEnd1);
        exBeg1 = exEnd1;
        REQUIRE(exBeg1 == exEnd1);
        REQUIRE(exBeg2 != exEnd2);
        exBeg2 = exEnd2;
        REQUIRE(exBeg2 == exEnd2);
        REQUIRE(exBeg3 != exEnd3);
        exBeg3 = exEnd3;
        REQUIRE(exBeg3 == exEnd3);
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

TEST_CASE("Exclude to containers") {
    std::array<int, 10> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto excluded1 = lz::exclude(arr, 3, 5);
    auto excluded2 = lz::exclude(arr, 0, 2);
    auto excluded3 = lz::exclude(arr, 7, 10);

    SECTION("To array") {
        REQUIRE((excluded1 | lz::to<std::array<int, 8>>()) == std::array<int, 8>{ 1, 2, 3, 6, 7, 8, 9, 10 });
        REQUIRE((excluded2 | lz::to<std::array<int, 8>>()) == std::array<int, 8>{ 3, 4, 5, 6, 7, 8, 9, 10 });
        REQUIRE((excluded3 | lz::to<std::array<int, 7>>()) == std::array<int, 7>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To vector") {
        REQUIRE((excluded1 | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 6, 7, 8, 9, 10 });
        REQUIRE((excluded2 | lz::to<std::vector>()) == std::vector<int>{ 3, 4, 5, 6, 7, 8, 9, 10 });
        REQUIRE((excluded3 | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To other container using to<>()") {
        REQUIRE((excluded1 | lz::to<std::list>()) == std::list<int>{ 1, 2, 3, 6, 7, 8, 9, 10 });
        REQUIRE((excluded2 | lz::to<std::list>()) == std::list<int>{ 3, 4, 5, 6, 7, 8, 9, 10 });
        REQUIRE((excluded3 | lz::to<std::list>()) == std::list<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To map") {
        REQUIRE((excluded1 | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>()) ==
                std::map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });

        REQUIRE((excluded2 | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>()) ==
                std::map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });

        REQUIRE((excluded3 | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>()) ==
                std::map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 } });
    }

    SECTION("To unordered map") {
        REQUIRE((excluded1 | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>()) ==
                std::unordered_map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });

        REQUIRE((excluded2 | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>()) ==
                std::unordered_map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });

        REQUIRE((excluded3 | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>()) ==
                std::unordered_map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 } });
    }
}