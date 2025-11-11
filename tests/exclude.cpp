#include <Lz/algorithm/equal.hpp>
#include <Lz/c_string.hpp>
#include <Lz/exclude.hpp>
#include <Lz/map.hpp>
#include <Lz/procs/to.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

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

    SUBCASE("Should Exclude out element") {
        constexpr auto signed_size = static_cast<std::ptrdiff_t>(arr.size() - 2);
        REQUIRE(lz::distance(excluded1) == signed_size);
        REQUIRE(lz::distance(excluded2) == signed_size);
        REQUIRE(lz::distance(excluded3) == signed_size);
    }

    SUBCASE("Should be by reference") {
        static_assert(std::is_same<decltype(*excluded1.begin()), int&>::value, "Should be int&");
    }
}

TEST_CASE("Operator=(default_sentinel_t)") {
    SUBCASE("forward") {
        auto cstr = lz::c_string("Hello, World!");
        auto excluded = lz::exclude(cstr, 3, 5);
        auto common = make_sentinel_assign_op_tester(excluded);
        auto expected = lz::c_string("Hel, World!");
        REQUIRE(lz::equal(common, expected));
    }

    SUBCASE("bidirectional") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto vec_sent = make_sized_bidi_sentinelled(vec);
        auto excluded = lz::exclude(vec_sent, 1, 3);
        auto common = make_sentinel_assign_op_tester(excluded);
        auto expected = { 1, 4, 5 };
        REQUIRE(lz::equal(common, expected));
        REQUIRE(lz::equal(common | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("random access") {
        auto repeater = lz::repeat(1, 5);
        auto excluded = make_sentinel_assign_op_tester(lz::exclude(repeater, 1, 3));
        auto expected = { 1, 1, 1 };
        REQUIRE(lz::equal(excluded, expected));
        REQUIRE(lz::equal(excluded | lz::reverse, expected | lz::reverse));
        test_procs::test_operator_plus(excluded, expected);
        test_procs::test_operator_minus(excluded);
    }
}
TEST_CASE("Exclude binary operations") {
    std::array<int, 10> arr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto excluded1 = arr | lz::exclude(3, 5); // { 1, 2, 3, 6, 7, 8, 9, 10 }
    auto excluded2 = lz::exclude(arr, 0, 2);  // { 3, 4, 5, 6, 7, 8, 9, 10 }
    auto excluded3 = lz::exclude(arr, 7, 10); // { 1, 2, 3, 4, 5, 6, 7 }

    SUBCASE("Operator++") {
        std::vector<int> expected = { 1, 2, 3, 6, 7, 8, 9, 10 };
        REQUIRE(lz::equal(excluded1, expected));

        expected = { 3, 4, 5, 6, 7, 8, 9, 10 };
        REQUIRE(lz::equal(excluded2, expected));

        expected = { 1, 2, 3, 4, 5, 6, 7 };
        REQUIRE(lz::equal(excluded3, expected));
    }

    SUBCASE("Operator--") {
        std::vector<int> expected = { 10, 9, 8, 7, 6, 3, 2, 1 };
        REQUIRE(lz::equal(lz::reverse(excluded1), expected));

        expected = { 10, 9, 8, 7, 6, 5, 4, 3 };
        REQUIRE(lz::equal(lz::reverse(excluded2), expected));

        expected = { 7, 6, 5, 4, 3, 2, 1 };
        REQUIRE(lz::equal(lz::reverse(excluded3), expected));
    }

    SUBCASE("Operator+") {
        std::vector<int> expected = { 1, 2, 3, 6, 7, 8, 9, 10 };
        test_procs::test_operator_plus(excluded1, expected);

        expected = { 3, 4, 5, 6, 7, 8, 9, 10 };
        test_procs::test_operator_plus(excluded2, expected);

        expected = { 1, 2, 3, 4, 5, 6, 7 };
        test_procs::test_operator_plus(excluded3, expected);
    }

    SUBCASE("Operator-") {
        test_procs::test_operator_minus(excluded1);
        test_procs::test_operator_minus(excluded2);
        test_procs::test_operator_minus(excluded3);
    }

    SUBCASE("Operator-(default_sentinel_t)") {
        auto excluded1_sent = lz::exclude(lz::repeat(0, 10), 3, 5);
        auto excluded2_sent = lz::exclude(lz::repeat(0, 10), 0, 2);
        auto excluded3_sent = lz::exclude(lz::repeat(0, 10), 8, 10);
        test_procs::test_operator_minus(excluded1_sent);
        test_procs::test_operator_minus(excluded2_sent);
        test_procs::test_operator_minus(excluded3_sent);
    }

    SUBCASE("Operator+(default_sentinel_t)") {
        auto excluded1_sent = lz::exclude(lz::repeat(0, 10), 3, 5);
        auto excluded2_sent = lz::exclude(lz::repeat(0, 10), 0, 2);
        auto excluded3_sent = lz::exclude(lz::repeat(0, 10), 8, 10);

        std::vector<int> expected1 = { 0, 0, 0, 0, 0, 0, 0, 0 };
        test_procs::test_operator_plus(excluded1_sent, expected1);

        expected1 = { 0, 0, 0, 0, 0, 0, 0, 0 };
        test_procs::test_operator_plus(excluded2_sent, expected1);

        expected1 = { 0, 0, 0, 0, 0, 0, 0, 0 };
        test_procs::test_operator_plus(excluded3_sent, expected1);
    }
}

TEST_CASE("Empty or one element exclude") {
    SUBCASE("Empty") {
        std::array<int, 0> arr = {};
        auto excluded = lz::exclude(arr, 0, 1);
        REQUIRE(excluded.begin() == excluded.end());
    }

    SUBCASE("One element") {
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

    SUBCASE("To array") {
        REQUIRE((excluded1 | lz::to<std::array<int, 8>>()) == std::array<int, 8>{ 1, 2, 3, 6, 7, 8, 9, 10 });
        REQUIRE((excluded2 | lz::to<std::array<int, 8>>()) == std::array<int, 8>{ 3, 4, 5, 6, 7, 8, 9, 10 });
        REQUIRE((excluded3 | lz::to<std::array<int, 7>>()) == std::array<int, 7>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SUBCASE("To vector") {
        REQUIRE((excluded1 | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 6, 7, 8, 9, 10 });
        REQUIRE((excluded2 | lz::to<std::vector>()) == std::vector<int>{ 3, 4, 5, 6, 7, 8, 9, 10 });
        REQUIRE((excluded3 | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SUBCASE("To other container using to<>()") {
        REQUIRE((excluded1 | lz::to<std::list>()) == std::list<int>{ 1, 2, 3, 6, 7, 8, 9, 10 });
        REQUIRE((excluded2 | lz::to<std::list>()) == std::list<int>{ 3, 4, 5, 6, 7, 8, 9, 10 });
        REQUIRE((excluded3 | lz::to<std::list>()) == std::list<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SUBCASE("To map") {
        REQUIRE((excluded1 | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>()) ==
                std::map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });

        REQUIRE((excluded2 | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>()) ==
                std::map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });

        REQUIRE((excluded3 | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>()) ==
                std::map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 } });
    }

    SUBCASE("To unordered map") {
        REQUIRE((excluded1 | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>()) ==
                std::unordered_map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });

        REQUIRE((excluded2 | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>()) ==
                std::unordered_map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 }, { 8, 8 }, { 9, 9 }, { 10, 10 } });

        REQUIRE((excluded3 | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>()) ==
                std::unordered_map<int, int>{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 7, 7 } });
    }
}
