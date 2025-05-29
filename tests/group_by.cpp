#include <Lz/c_string.hpp>
#include <Lz/common.hpp>
#include <Lz/group_by.hpp>
#include <Lz/map.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Group by with sentinels") {
    auto cstr = lz::c_string("aaabbccccd");
    std::function<bool(char, char)> equal = [](char a, char b) {
        return a == b;
    };
    lz::group_by_iterable<decltype(cstr), decltype(equal)> grouper = lz::group_by(cstr, std::move(equal));
    auto it = grouper.begin();

    REQUIRE(lz::equal(it->second, lz::c_string("aaa")));
    ++it;
    REQUIRE(lz::equal(it->second, lz::c_string("bb")));
    ++it;
    REQUIRE(lz::equal(it->second, lz::c_string("cccc")));
    ++it;
    REQUIRE(lz::equal(it->second, lz::c_string("d")));
    ++it;
    REQUIRE(it == grouper.end());

    SECTION("Operator=") {
        it = grouper.begin();
        REQUIRE(it == grouper.begin());
        it = grouper.end();
        REQUIRE(it == grouper.end());
    }
}

TEST_CASE("Empty or one element group by") {
    SECTION("Empty") {
        auto grouper = lz::group_by(lz::c_string(""), [](char a, char b) { return a == b; });
        REQUIRE(lz::empty(grouper));
        REQUIRE(!lz::has_one(grouper));
        REQUIRE(!lz::has_many(grouper));
    }

    SECTION("One element") {
        auto grouper = lz::group_by(lz::c_string("a"), [](char a, char b) { return a == b; });
        static_assert(!lz::detail::is_bidi<decltype(grouper.begin())>::value, "Should not be bidirectional iterator");
        REQUIRE(!lz::empty(grouper));
        REQUIRE(lz::has_one(grouper));
        REQUIRE(!lz::has_many(grouper));
    }
}

TEST_CASE("group_by changing and creating elements") {
    std::vector<std::string> vec = { "hello", "hellp", "i'm", "done" };

    std::sort(vec.begin(), vec.end(), [](const std::string& a, const std::string& b) { return a.length() < b.length(); });
    auto grouper = vec | lz::group_by([](const std::string& a, const std::string& b) { return a.length() == b.length(); });
    static_assert(lz::detail::is_bidi<decltype(grouper.begin())>::value, "Should be bidirectional iterator");

    SECTION("Should be correct chunks") {
        std::size_t str_len = 3;

        using value_type = lz::val_iterable_t<decltype(grouper)>;
        lz::for_each(grouper, [&str_len](const value_type& g) {
            REQUIRE(g.first.length() == str_len);
            for (const auto& str : g.second) {
                REQUIRE(str.length() == str_len);
            }
            ++str_len;
        });
    }

    SECTION("Should be by ref") {
        auto begin = grouper.begin();
        *(begin->second.begin()) = "imm";
        REQUIRE(vec[0] == "imm");
    }
}

TEST_CASE("group_by binary operations") {
    std::vector<std::string> vec = { "a", "bb", "ccc", "ccc", "dddd", "dddd" };
    auto grouper = lz::group_by(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });

    SECTION("Operator++") {
        auto it = grouper.begin();
        std::vector<std::string> expected = { "a" };
        REQUIRE(lz::equal(it->second, expected));
        REQUIRE(it->first == "a");
        ++it;
        expected = { "bb" };
        REQUIRE(it->first == "bb");
        REQUIRE(lz::equal(it->second, expected));
        ++it;
        expected = { "ccc", "ccc" };
        REQUIRE(it->first == "ccc");
        REQUIRE(lz::equal(it->second, expected));
        ++it;
        expected = { "dddd", "dddd" };
        REQUIRE(it->first == "dddd");
        REQUIRE(lz::equal(it->second, expected));
        ++it;
        REQUIRE(it == grouper.end());

        vec = { "a", "bb", "ccc", "ccc", "dddd" };
        auto grouper2 = lz::group_by(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });
        auto it2 = grouper2.begin();
        REQUIRE(it2->first == "a");
        expected = { "a" };
        REQUIRE(lz::equal(it2->second, expected));
        ++it2;
        REQUIRE(it2->first == "bb");
        expected = { "bb" };
        REQUIRE(lz::equal(it2->second, expected));
        ++it2;
        REQUIRE(it2->first == "ccc");
        expected = { "ccc", "ccc" };
        REQUIRE(lz::equal(it2->second, expected));
        ++it2;
        REQUIRE(it2->first == "dddd");
        expected = { "dddd" };
        REQUIRE(lz::equal(it2->second, expected));
        ++it2;
        REQUIRE(it2 == grouper2.end());
    }

    SECTION("Operator--") {
        auto it = grouper.end();
        --it;
        std::vector<std::string> expected = { "dddd", "dddd" };
        REQUIRE(it->first == "dddd");
        REQUIRE(lz::equal(it->second, expected));
        --it;
        expected = { "ccc", "ccc" };
        REQUIRE(it->first == "ccc");
        REQUIRE(lz::equal(it->second, expected));
        --it;
        expected = { "bb" };
        REQUIRE(it->first == "bb");
        REQUIRE(lz::equal(it->second, expected));
        --it;
        expected = { "a" };
        REQUIRE(it->first == "a");
        REQUIRE(lz::equal(it->second, expected));
        REQUIRE(it == grouper.begin());

        ++it;
        expected = { "bb" };
        REQUIRE(it->first == "bb");
        REQUIRE(lz::equal(it->second, expected));
        ++it;
        expected = { "ccc", "ccc" };
        REQUIRE(it->first == "ccc");
        REQUIRE(lz::equal(it->second, expected));
        ++it;
        expected = { "dddd", "dddd" };
        REQUIRE(it->first == "dddd");
        REQUIRE(lz::equal(it->second, expected));
        ++it;
        REQUIRE(it == grouper.end());

        vec = { "a", "a", "bb", "ccc", "ccc", "dddd", "dddd" };
        auto grouper2 = lz::group_by(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });
        auto it2 = grouper2.end();
        --it2;

        expected = { "dddd", "dddd" };
        REQUIRE(it2->first == "dddd");
        REQUIRE(lz::equal(it2->second, expected));
        --it2;
        expected = { "ccc", "ccc" };
        REQUIRE(it2->first == "ccc");
        REQUIRE(lz::equal(it2->second, expected));
        --it2;
        expected = { "bb" };
        REQUIRE(it2->first == "bb");
        REQUIRE(lz::equal(it2->second, expected));
        --it2;
        expected = { "a", "a" };
        REQUIRE(it2->first == "a");
        REQUIRE(lz::equal(it2->second, expected));
        REQUIRE(it2 == grouper2.begin());

        ++it2;
        expected = { "bb" };
        REQUIRE(it2->first == "bb");
        REQUIRE(lz::equal(it2->second, expected));
        ++it2;
        expected = { "ccc", "ccc" };
        REQUIRE(it2->first == "ccc");
        REQUIRE(lz::equal(it2->second, expected));
        ++it2;
        expected = { "dddd", "dddd" };
        REQUIRE(it2->first == "dddd");
        REQUIRE(lz::equal(it2->second, expected));
        ++it2;
        REQUIRE(it2 == grouper2.end());
    }

    SECTION("Operator== & operator!=") {
        auto it = grouper.begin();
        REQUIRE(it != grouper.end());
        while (it != grouper.end()) {
            ++it;
        }
        REQUIRE(it == grouper.end());
    }
}

TEST_CASE("To containers group by") {
    std::vector<std::string> vec = { "hello", "hellp", "i'm", "done" };
    auto grouper = vec | lz::group_by([](const std::string& a, const std::string& b) { return a.length() == b.length(); });
    using value_type = lz::val_iterable_t<decltype(grouper)>;
    using pair_type = std::pair<std::string, std::vector<std::string>>;

    SECTION("To array") {
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

    SECTION("To vector") {
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

    SECTION("To list") {
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

    SECTION("To map") {
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

    SECTION("To unordered map") {
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
