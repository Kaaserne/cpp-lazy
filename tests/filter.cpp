#include <Lz/c_string.hpp>
#include <Lz/filter.hpp>
#include <Lz/map.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

TEST_CASE("Filter with sentinels") {
    const char* str = "Hello, World!";
    auto c_str = lz::c_string(str);
    std::function<bool(char)> predicate = [](char c) {
        return c != 'o';
    };
    lz::filter_iterable<decltype(c_str), decltype(predicate)> filter = lz::filter(c_str, std::move(predicate));
    static_assert(!std::is_same<decltype(filter.begin()), decltype(filter.end())>::value, "Must be sentinel");
    std::vector<char> expected = { 'H', 'e', 'l', 'l', ',', ' ', 'W', 'r', 'l', 'd', '!' };
    REQUIRE((filter | lz::to<std::vector>()) == expected);
}

TEST_CASE("operator=(default_sentinel_t)") {
    SUBCASE("forward") {
        std::forward_list<int> lst = { 1, 2, 3, 4, 5 };
        auto f = lz::filter(lst, [](int i) { return i % 2 == 0; });
        auto common = make_sentinel_assign_op_tester(f);
        auto expected = { 2, 4 };
        REQUIRE(lz::equal(expected, common));
    }

    SUBCASE("bidirectional") {
        std::vector<int> vec = { 1, 2, 3, 4, 5 };
        auto vec_sent = make_bidi_sentinelled(vec);
        auto f = lz::filter(vec_sent, [](int i) { return i % 2 == 0; });
        auto common = make_sentinel_assign_op_tester(f);
        auto expected = { 2, 4 };
        REQUIRE(lz::equal(expected, common));
        REQUIRE(lz::equal(expected | lz::reverse, common | lz::reverse));
    }
}

TEST_CASE("Empty or one element filter") {
    SUBCASE("Empty") {
        std::list<int> empty;
        auto filter = lz::filter(empty, [](int i) { return i != 0; });
        REQUIRE(lz::empty(filter));
        REQUIRE_FALSE(lz::has_one(filter));
        REQUIRE_FALSE(lz::has_many(filter));
    }

    SUBCASE("One element") {
        std::list<int> one_element = { 1 };
        auto filter = lz::filter(one_element, [](int i) { return i != 0; });
        REQUIRE_FALSE(lz::empty(filter));
        REQUIRE(lz::has_one(filter));
        REQUIRE_FALSE(lz::has_many(filter));
        REQUIRE(*filter.begin() == 1);
    }
}

TEST_CASE("Filter filters and is by reference") {
    constexpr size_t size = 3;
    std::array<int, size> array{ 1, 2, 3 };

    SUBCASE("Should filter out element") {
        std::function<bool(int)> f = [](int element) {
            return element != 3;
        };
        auto filter = array | lz::filter(std::move(f));
        static_assert(std::is_same<decltype(filter.begin()), decltype(filter.end())>::value, "Must not be sentinel");

        auto it = filter.begin();

        int expected = array[0];
        REQUIRE(*it == expected);

        ++it;
        expected = array[1];
        REQUIRE(*it == expected);

        ++it;
        REQUIRE(it == filter.end());
    }

    SUBCASE("Should be by reference") {
        auto filter = lz::filter(array, [](int element) { return element != 3; });
        auto it = filter.begin();

        *it = 50;
        REQUIRE(array[0] == 50);
    }
}

TEST_CASE("Filter binary operations") {
    constexpr std::size_t size = 3;
    std::array<int, size> array{ 1, 2, 3 };
    std::function<bool(int)> f = [](int i) {
        return i != 3;
    };
    auto filter = lz::filter(array, std::move(f));
    auto it = filter.begin();

    SUBCASE("Operator++") {
        auto expected = { 1, 2 };
        REQUIRE(lz::equal(filter, expected));
    }

    SUBCASE("Operator== & operator!=") {
        REQUIRE(it != filter.end());
        it = filter.end();
        REQUIRE(it == filter.end());
    }

    SUBCASE("Operator--") {
        auto expected = { 2, 1 };
        REQUIRE(lz::equal(filter | lz::reverse, expected));
    }
}

TEST_CASE("Filter to container") {
    constexpr std::size_t size = 3;
    std::array<int, size> array{ 1, 2, 3 };

    SUBCASE("To array") {
        constexpr std::size_t filter_size = 2;
        auto filtered = lz::filter(array, [](int i) { return i != 3; }) | lz::to<std::array<int, filter_size>>();
        std::array<int, filter_size> expected = { 1, 2 };
        REQUIRE(filtered == expected);
    }

    SUBCASE("To vector") {
        auto filtered_vec = lz::filter(array, [](int i) { return i != 3; }) | lz::to<std::vector>();
        std::vector<int> expected = { 1, 2 };
        REQUIRE(filtered_vec == expected);
    }

    SUBCASE("To other container using to<>()") {
        auto filtered_list = lz::filter(array, [](int i) { return i != 3; }) | lz::to<std::list<int>>();
        std::list<int> expected = { 1, 2 };
        REQUIRE(filtered_list == expected);
    }

    SUBCASE("To map") {
        auto filtered = lz::filter(array, [](const int i) { return i != 3; });
        auto actual = filtered | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();
        std::map<int, int> expected = { std::make_pair(1, 1), std::make_pair(2, 2) };
        REQUIRE(expected == actual);
    }

    SUBCASE("To unordered map") {
        auto filtered = lz::filter(array, [](const int i) { return i != 3; });
        auto actual =
            filtered | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        std::unordered_map<int, int> expected = { std::make_pair(1, 1), std::make_pair(2, 2) };
        REQUIRE(expected == actual);
    }

    SUBCASE("To reverse container") {
        auto filtered = lz::filter(array, [](int i) { return i != 3; });
        using iter = decltype(filtered)::iterator;
        using sentinel = decltype(filtered.end());

        auto it_rev = std::reverse_iterator<iter>(filtered.end());
        auto end_rev = std::reverse_iterator<sentinel>(filtered.begin());
        std::vector<int> reversed(it_rev, end_rev);
        std::vector<int> expected = { 2, 1 };
        REQUIRE(reversed == expected);
    }
}
