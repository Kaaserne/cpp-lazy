#include <Lz/exclude.hpp>
#include <Lz/iter_tools.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <cpp-lazy-ut-helper/repeat.hpp>
#include <list>
#include <map>
#include <test_procs.hpp>
#include <unordered_map>

TEST_CASE("Exclude with sentinels") {
    auto cstr = lz::c_string("a string to exclude");
    lz::exclude_iterable<decltype(cstr)> excluded = lz::exclude(cstr, 3, 5);
    REQUIRE((excluded | lz::to<std::string>()) == "a sing to exclude");
    static_assert(std::is_same<decltype(excluded.end()), lz::default_sentinel_t>::value, "Should be default sentinel");
}

TEST_CASE("Exclude changing and creating elements") {
    std::array<int, 10> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto excluded1 = lz::exclude(arr, 3, 5);
    static_assert(!std::is_same<decltype(excluded1.end()), lz::default_sentinel_t>::value, "Should not be default sentinel");
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
        REQUIRE(lz::distance(excluded1) == signed_size);
        REQUIRE(lz::distance(excluded2) == signed_size);
        REQUIRE(lz::distance(excluded3) == signed_size);
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

    SECTION("Operator++") {
        std::vector<int> expected = { 1, 2, 3, 6, 7, 8, 9, 10 };
        REQUIRE(lz::equal(excluded1, expected));

        expected = { 3, 4, 5, 6, 7, 8, 9, 10 };
        REQUIRE(lz::equal(excluded2, expected));

        expected = { 1, 2, 3, 4, 5, 6, 7 };
        REQUIRE(lz::equal(excluded3, expected));
    }

    SECTION("Operator--") {
        std::vector<int> expected = { 10, 9, 8, 7, 6, 3, 2, 1 };
        REQUIRE(lz::equal(lz::reverse(excluded1), expected));

        expected = { 10, 9, 8, 7, 6, 5, 4, 3 };
        REQUIRE(lz::equal(lz::reverse(excluded2), expected));

        expected = { 7, 6, 5, 4, 3, 2, 1 };
        REQUIRE(lz::equal(lz::reverse(excluded3), expected));
    }

    SECTION("Operator+") {
        std::vector<int> expected = { 1, 2, 3, 6, 7, 8, 9, 10 };
        test_procs::test_operator_plus(excluded1, expected);

        expected = { 3, 4, 5, 6, 7, 8, 9, 10 };
        test_procs::test_operator_plus(excluded2, expected);

        expected = { 1, 2, 3, 4, 5, 6, 7 };
        test_procs::test_operator_plus(excluded3, expected);
    }

    SECTION("Operator-") {
        test_procs::test_operator_minus(excluded1);
        test_procs::test_operator_minus(excluded2);
        test_procs::test_operator_minus(excluded3);
    }

    SECTION("Operator-(default_sentinel_t)") {
        auto excluded1_sent = lz::exclude(lz::repeat(0, 10), 3, 5);
        auto excluded2_sent = lz::exclude(lz::repeat(0, 10), 0, 2);
        auto excluded3_sent = lz::exclude(lz::repeat(0, 10), 8, 10);
        test_procs::test_operator_minus(excluded1_sent);
        test_procs::test_operator_minus(excluded2_sent);
        test_procs::test_operator_minus(excluded3_sent);
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