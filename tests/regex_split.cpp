#include <Lz/algorithm/empty.hpp>
#include <Lz/algorithm/equal.hpp>
#include <Lz/algorithm/has_many.hpp>
#include <Lz/algorithm/has_one.hpp>
#include <Lz/procs/to.hpp>
#include <Lz/regex_split.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

TEST_CASE("regex_split_iterable changing and creating elements") {
    std::regex r1(R"(\s+)");

    SUBCASE("Empty string") {
        std::string s = "";
        lz::regex_split_iterable<std::sregex_token_iterator> splitter = lz::regex_split(s, r1);
        auto actual = splitter | lz::to<std::vector>();
        std::vector<std::string> expected = {};
        REQUIRE(lz::equal(actual, expected));
    }

    SUBCASE("Starting with delimiter") {
        std::string s = "    Hello, world! How are you?";
        auto splitter = s | lz::regex_split(r1);
        auto actual = splitter | lz::to<std::vector>();
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(lz::equal(actual, expected));
    }

    SUBCASE("Ending with delimiter") {
        std::string s = "Hello, world! How are you?    ";
        auto splitter = lz::regex_split(s, r1);
        auto actual = splitter | lz::to<std::vector>();
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(lz::equal(actual, expected));
    }

    SUBCASE("Starting and ending with delimiter") {
        std::string s = "    Hello, world! How are you?    ";
        auto splitter = lz::regex_split(s, r1);
        auto actual = splitter | lz::to<std::vector>();
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(lz::equal(actual, expected));
    }

    SUBCASE("Operator=(default_sentinel_t)") {
        std::string s = "hello, world! This is a test.";
        std::regex r("\\s+");
        auto splitted = lz::regex_split(s, r);
        auto common = make_sentinel_assign_op_tester(splitted);
        auto expected = { "hello,", "world!", "This", "is", "a", "test." };
        REQUIRE(lz::equal(common, expected));
    }
}

TEST_CASE("Empty or one element regex split") {
    SUBCASE("Empty") {
        std::regex r1(R"(\s+)");
        std::string s;
        auto splitter = lz::regex_split(s, r1);
        REQUIRE(lz::empty(splitter));
        REQUIRE_FALSE(lz::has_one(splitter));
        REQUIRE_FALSE(lz::has_many(splitter));
    }

    SUBCASE("One element with result") {
        std::regex r1(R"(\s+)");
        std::string s = "Hello ";
        auto splitter = lz::regex_split(s, r1);
        REQUIRE_FALSE(lz::empty(splitter));
        REQUIRE(lz::has_one(splitter));
        REQUIRE_FALSE(lz::has_many(splitter));
    }

    SUBCASE("One element without result") {
        std::regex r1(R"(\s+)");
        std::string s = "Hello";
        auto splitter = lz::regex_split(s, r1);
        REQUIRE_FALSE(lz::empty(splitter));
        REQUIRE(lz::has_one(splitter));
        REQUIRE_FALSE(lz::has_many(splitter));
    }
}

TEST_CASE("regex_split_iterable binary operations") {
    std::regex r1(R"(\s+)");
    std::string s = "    Hello, world! How are you?    ";
    auto splitter = lz::regex_split(s, r1);
    auto begin = splitter.begin();

    SUBCASE("Operator++") {
        ++begin;
        REQUIRE(lz::distance(begin, splitter.end()) == 4);
    }

    SUBCASE("Operator== & Operator!=") {
        REQUIRE(begin != splitter.end());
        while (begin != splitter.end()) {
            ++begin;
        }
        REQUIRE(begin == splitter.end());
    }
}

TEST_CASE("regex_split_iterable to containers") {
    std::regex r1(R"(\s+)");
    std::string s = "    Hello, world! How are you?    ";
    auto splitter = lz::regex_split(s, r1);

    SUBCASE("To vector") {
        auto vec = splitter | lz::to<std::vector>();
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(lz::equal(vec, expected));
    }

    SUBCASE("To list") {
        auto list = splitter | lz::to<std::list>();
        std::list<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(lz::equal(list, expected));
    }
}
