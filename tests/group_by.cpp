#include <Lz/c_string.hpp>
#include <Lz/common.hpp>
#include <Lz/group_by.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Group by with sentinels") {
    auto cstr = lz::c_string("aaabbccccd");
    auto grouper = lz::group_by(cstr, [](char a, char b) { return a == b; });
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
}

TEST_CASE("Empty or one element group by") {
    SECTION("Empty") {
        auto generator = lz::group_by(lz::c_string(""), [](char a, char b) { return a == b; });
        REQUIRE(lz::empty(generator));
        REQUIRE(!lz::has_one(generator));
        REQUIRE(!lz::has_many(generator));
    }

    SECTION("One element") {
        auto generator = lz::group_by(lz::c_string("a"), [](char a, char b) { return a == b; });
        REQUIRE(!lz::empty(generator));
        REQUIRE(lz::has_one(generator));
        REQUIRE(!lz::has_many(generator));
    }
}

TEST_CASE("group_by changing and creating elements", "[group_by][Basic functionality]") {
    std::vector<std::string> vec = { "hello", "hellp", "i'm", "done" };

    std::sort(vec.begin(), vec.end(), [](const std::string& a, const std::string& b) { return a.length() < b.length(); });
    auto grouper = lz::group_by(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });

    SECTION("Should be correct chunks") {
        std::size_t str_len = 3;

        using value_type = lz::val_iterable_t<decltype(grouper)>;
        grouper.for_each([&str_len](const value_type& g) {
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

TEST_CASE("group_by binary operations", "[group_by][Binary ops]") {
    std::vector<std::string> vec = { "hello", "hellp", "i'm", "done" };

    std::sort(vec.begin(), vec.end(), [](const std::string& a, const std::string& b) { return a.length() < b.length(); });
    auto grouper = lz::group_by(vec, [](const std::string& a, const std::string& b) { return a.length() == b.length(); });

    SECTION("Operator++") {
        auto it = grouper.begin();
        REQUIRE(it->first.length() == 3);
        REQUIRE(*it->second.begin() == "i'm");
        ++it;

        REQUIRE(it->first.length() == 4);
        REQUIRE(*it->second.begin() == "done");
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