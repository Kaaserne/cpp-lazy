#include <Lz/regex_split.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <regex>

TEST_CASE("regex_split_iterable changing and creating elements") {
    std::regex r1(R"(\s+)");

    SECTION("Empty string") {
        std::string s = "";
        lz::regex_split_iterable<std::sregex_token_iterator> splitter = lz::regex_split(s, r1);
        auto actual = splitter | lz::to<std::vector>();
        std::vector<std::string> expected = {};
        REQUIRE(lz::equal(actual, expected));
    }

    SECTION("Starting with delimiter") {
        std::string s = "    Hello, world! How are you?";
        auto splitter = s | lz::regex_split(r1);
        auto actual = splitter | lz::to<std::vector>();
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(lz::equal(actual, expected));
    }

    SECTION("Ending with delimiter") {
        std::string s = "Hello, world! How are you?    ";
        auto splitter = lz::regex_split(s, r1);
        auto actual = splitter | lz::to<std::vector>();
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(lz::equal(actual, expected));
    }

    SECTION("Starting and ending with delimiter") {
        std::string s = "    Hello, world! How are you?    ";
        auto splitter = lz::regex_split(s, r1);
        auto actual = splitter | lz::to<std::vector>();
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(lz::equal(actual, expected));
    }

    SECTION("Operator=") {
        std::string s = "Hello, world! How are you?";
        auto splitter = lz::regex_split(s, r1);
        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = std::regex_token_iterator<std::string::const_iterator>{};
        REQUIRE(it == splitter.end());
    }
}

TEST_CASE("Empty or one element regex split") {
    SECTION("Empty") {
        std::regex r1(R"(\s+)");
        std::string s;
        auto splitter = lz::regex_split(s, r1);
        REQUIRE(lz::empty(splitter));
        REQUIRE(!lz::has_one(splitter));
        REQUIRE(!lz::has_many(splitter));
    }

    SECTION("One element with result") {
        std::regex r1(R"(\s+)");
        std::string s = "Hello ";
        auto splitter = lz::regex_split(s, r1);
        REQUIRE(!lz::empty(splitter));
        REQUIRE(lz::has_one(splitter));
        REQUIRE(!lz::has_many(splitter));
    }

    SECTION("One element without result") {
        std::regex r1(R"(\s+)");
        std::string s = "Hello";
        auto splitter = lz::regex_split(s, r1);
        REQUIRE(!lz::empty(splitter));
        REQUIRE(lz::has_one(splitter));
        REQUIRE(!lz::has_many(splitter));
    }
}

TEST_CASE("regex_split_iterable binary operations") {
    std::regex r1(R"(\s+)");
    std::string s = "    Hello, world! How are you?    ";
    auto splitter = lz::regex_split(s, r1);
    auto begin = splitter.begin();

    SECTION("Operator++") {
        ++begin;
        REQUIRE(lz::distance(begin, splitter.end()) == 4);
    }

    SECTION("Operator== & Operator!=") {
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

    SECTION("To vector") {
        auto vec = splitter | lz::to<std::vector>();
        std::vector<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(lz::equal(vec, expected));
    }

    SECTION("To list") {
        auto list = splitter | lz::to<std::list>();
        std::list<std::string> expected = { "Hello,", "world!", "How", "are", "you?" };
        REQUIRE(lz::equal(list, expected));
    }
}
