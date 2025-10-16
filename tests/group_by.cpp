#include <Lz/algorithm/empty.hpp>
#include <Lz/algorithm/equal.hpp>
#include <Lz/algorithm/has_many.hpp>
#include <Lz/algorithm/has_one.hpp>
#include <Lz/c_string.hpp>
#include <Lz/group_by.hpp>
#include <Lz/map.hpp>
#include <Lz/procs/to.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

struct eq_pair {
    template<class T1, class T2>
    bool operator()(const T1& first, const T2& second) const {
        return first.first == second.first && lz::equal(first.second, second.second);
    }
};

TEST_CASE("Group by with sentinels") {
    auto cstr = lz::c_string("aaabbccccd");
    std::function<bool(char, char)> equal = [](char a, char b) {
        return a == b;
    };

    lz::group_by_iterable<decltype(cstr), decltype(equal)> grouper = lz::group_by(cstr, std::move(equal));
    auto expected = { std::make_pair('a', lz::c_string("aaa")), std::make_pair('b', lz::c_string("bb")),
                      std::make_pair('c', lz::c_string("cccc")), std::make_pair('d', lz::c_string("d")) };

    REQUIRE(lz::equal(grouper, expected, eq_pair{}));
}

TEST_CASE("operator=(default_sentinel_t)") {
    SUBCASE("forward") {
        std::forward_list<int> lst = { 1, 1, 2, 2, 3, 4, 4 };
        auto grouped = lz::group_by(lst, lz::detail::equal_to{});
        auto common = make_sentinel_assign_op_tester(grouped);
        using reference = lz::detail::ref_iterable_t<decltype(common)>;
        std::vector<std::pair<int, std::vector<int>>> expected2 = { std::make_pair(1, std::vector<int>{ 1, 1 }),
                                                                    std::make_pair(2, std::vector<int>{ 2, 2 }),
                                                                    std::make_pair(3, std::vector<int>{ 3 }),
                                                                    std::make_pair(4, std::vector<int>{ 4, 4 }) };
        REQUIRE(lz::equal(common, expected2,
                          [](reference a, const std::pair<int, std::vector<int>>& b) { return a.first == b.first && lz::equal(a.second, b.second); }));
    }

    SUBCASE("bidirectional") {
        std::vector<int> vec = { 1, 1, 2, 2, 3, 4, 4 };
        auto bidi_sentinelled = make_sized_bidi_sentinelled(vec);
        auto grouped = lz::group_by(bidi_sentinelled, lz::detail::equal_to{});
        auto common = make_sentinel_assign_op_tester(grouped);
        using reference = lz::detail::ref_iterable_t<decltype(common)>;
        std::vector<std::pair<int, std::vector<int>>> expected2 = { std::make_pair(1, std::vector<int>{ 1, 1 }),
                                                                    std::make_pair(2, std::vector<int>{ 2, 2 }),
                                                                    std::make_pair(3, std::vector<int>{ 3 }),
                                                                    std::make_pair(4, std::vector<int>{ 4, 4 }) };
        REQUIRE(lz::equal(common, expected2, [](reference a, const std::pair<int, std::vector<int>>& b) {
            return a.first == b.first && lz::equal(a.second, b.second);
        }));
        REQUIRE(
            lz::equal(common | lz::reverse, expected2 | lz::reverse, [](reference a, const std::pair<int, std::vector<int>>& b) {
                return a.first == b.first && lz::equal(a.second, b.second);
            }));
    }
}

TEST_CASE("Empty or one element group by") {
    SUBCASE("Empty") {
        auto grouper = lz::group_by(lz::c_string(""), [](char a, char b) { return a == b; });
        REQUIRE(lz::empty(grouper));
        REQUIRE_FALSE(lz::has_one(grouper));
        REQUIRE_FALSE(lz::has_many(grouper));
    }

    SUBCASE("One element") {
        auto grouper = lz::group_by(lz::c_string("a"), [](char a, char b) { return a == b; });
        static_assert(!lz::detail::is_bidi<decltype(grouper.begin())>::value, "Should not be bidirectional iterator");
        REQUIRE_FALSE(lz::empty(grouper));
        REQUIRE(lz::has_one(grouper));
        REQUIRE_FALSE(lz::has_many(grouper));
    }
}

TEST_CASE("group_by changing and creating elements") {
    std::vector<std::string> vec = { "done", "hello", "hellp", "i'm" };

    auto grouper = vec | lz::group_by([](const std::string& a, const std::string& b) { return a.length() == b.length(); });
    static_assert(lz::detail::is_bidi<decltype(grouper.begin())>::value, "Should be bidirectional iterator");

    SUBCASE("Should be correct chunks") {
        std::size_t str_len = 3;

        using value_type = lz::detail::val_iterable_t<decltype(grouper)>;
        lz::for_each(grouper, [&str_len](const value_type& g) {
            REQUIRE(g.first.length() == str_len);
            for (const auto& str : g.second) {
                REQUIRE(str.length() == str_len);
            }
            ++str_len;
        });
    }

    SUBCASE("Should be by ref") {
        auto begin = grouper.begin();
        *(begin->second.begin()) = "imm";
        REQUIRE(vec[0] == "imm");
    }
}

TEST_CASE("group_by binary operations") {
    std::vector<std::string> vec = { "a", "bb", "ccc", "ccc", "dddd", "dddd" };
    auto grouper = lz::group_by(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });

    SUBCASE("Operator++") {
        std::vector<std::pair<std::string, std::vector<std::string>>> expected = {
            std::make_pair("a", std::vector<std::string>{ "a" }), std::make_pair("bb", std::vector<std::string>{ "bb" }),
            std::make_pair("ccc", std::vector<std::string>{ "ccc", "ccc" }),
            std::make_pair("dddd", std::vector<std::string>{ "dddd", "dddd" })
        };
        REQUIRE(lz::equal(grouper, expected, eq_pair{}));

        vec = { "a", "bb", "ccc", "ccc", "dddd" };
        auto grouper2 = lz::group_by(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });
        expected = { std::make_pair("a", std::vector<std::string>{ "a" }), std::make_pair("bb", std::vector<std::string>{ "bb" }),
                     std::make_pair("ccc", std::vector<std::string>{ "ccc", "ccc" }),
                     std::make_pair("dddd", std::vector<std::string>{ "dddd" }) };
        REQUIRE(lz::equal(grouper2, expected, eq_pair{}));
    }

    SUBCASE("Operator--") {
        std::vector<std::pair<std::string, std::vector<std::string>>> expected = {
            std::make_pair("dddd", std::vector<std::string>{ "dddd", "dddd" }),
            std::make_pair("ccc", std::vector<std::string>{ "ccc", "ccc" }),
            std::make_pair("bb", std::vector<std::string>{ "bb" }), std::make_pair("a", std::vector<std::string>{ "a" })
        };
        REQUIRE(lz::equal(grouper | lz::reverse, expected, eq_pair{}));

        vec = { "a", "a", "bb", "ccc", "ccc", "dddd", "dddd" };
        auto grouper2 = lz::group_by(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });
        expected = { std::make_pair("dddd", std::vector<std::string>{ "dddd", "dddd" }),
                     std::make_pair("ccc", std::vector<std::string>{ "ccc", "ccc" }),
                     std::make_pair("bb", std::vector<std::string>{ "bb" }),
                     std::make_pair("a", std::vector<std::string>{ "a", "a" }) };
        REQUIRE(lz::equal(grouper2 | lz::reverse, expected, eq_pair{}));
    }
}

TEST_CASE("To containers group by") {
    std::vector<std::string> vec = { "hello", "hellp", "i'm", "done" };
    auto grouper = vec | lz::group_by([](const std::string& a, const std::string& b) { return a.length() == b.length(); });
    using value_type = lz::detail::val_iterable_t<decltype(grouper)>;
    using pair_type = std::pair<std::string, std::vector<std::string>>;

    SUBCASE("To array") {
        auto arr = grouper |
                   lz::map([](const value_type& v) { return std::make_pair(v.first, v.second | lz::to<std::vector>()); }) |
                   lz::to<std::array<pair_type, 4>>();

        std::array<pair_type, 4> expected = {
            std::make_pair("hello", std::vector<std::string>{ "hello", "hellp" }),
            std::make_pair("i'm", std::vector<std::string>{ "i'm" }),
            std::make_pair("done", std::vector<std::string>{ "done" }),
        };
        REQUIRE(arr == expected);
    }

    SUBCASE("To vector") {
        auto vec2 = grouper |
                    lz::map([](const value_type& v) { return std::make_pair(v.first, v.second | lz::to<std::vector>()); }) |
                    lz::to<std::vector>();

        std::vector<pair_type> expected = {
            std::make_pair("hello", std::vector<std::string>{ "hello", "hellp" }),
            std::make_pair("i'm", std::vector<std::string>{ "i'm" }),
            std::make_pair("done", std::vector<std::string>{ "done" }),
        };
        REQUIRE(vec2 == expected);
    }

    SUBCASE("To list") {
        auto lst = grouper |
                   lz::map([](const value_type& v) { return std::make_pair(v.first, v.second | lz::to<std::vector>()); }) |
                   lz::to<std::list>();

        std::list<pair_type> expected = {
            std::make_pair("hello", std::vector<std::string>{ "hello", "hellp" }),
            std::make_pair("i'm", std::vector<std::string>{ "i'm" }),
            std::make_pair("done", std::vector<std::string>{ "done" }),
        };
        REQUIRE(lst == expected);
    }

    SUBCASE("To map") {
        auto map = grouper |
                   lz::map([](const value_type& v) { return std::make_pair(v.first, v.second | lz::to<std::vector>()); }) |
                   lz::to<std::map<std::string, std::vector<std::string>>>();

        std::map<std::string, std::vector<std::string>> expected = {
            { "hello", { "hello", "hellp" } },
            { "i'm", { "i'm" } },
            { "done", { "done" } },
        };
        REQUIRE(map == expected);
    }

    SUBCASE("To unordered map") {
        auto map = grouper |
                   lz::map([](const value_type& v) { return std::make_pair(v.first, v.second | lz::to<std::vector>()); }) |
                   lz::to<std::unordered_map<std::string, std::vector<std::string>>>();

        std::unordered_map<std::string, std::vector<std::string>> expected = {
            { "hello", { "hello", "hellp" } },
            { "i'm", { "i'm" } },
            { "done", { "done" } },
        };
        REQUIRE(map == expected);
    }
}
