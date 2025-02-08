#include <Lz/c_string.hpp>
#include <Lz/map.hpp>
#include <Lz/split.hpp>
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Splitter permutations") {
    SECTION("Starting with one delimiter ending with none") {
        auto cstr = lz::c_string("");
        const std::string to_split = " Hello world test 123";
        auto splitter = lz::s_split(to_split, " ");
        std::vector<std::string> expected = { "", "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SECTION("Starting with two delimiters ending with none") {
        const std::string to_split = "  Hello world test 123";
        auto splitter = lz::sv_split(to_split, " ");
        std::vector<std::string> expected = { "", "", "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SECTION("Ending with one delimiter starting with none") {
        const std::string to_split = "Hello world test 123 ";
        auto splitter = lz::sv_split(to_split, " ");
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SECTION("Ending with two delimiters starting with none") {
        const std::string to_split = "Hello world test 123  ";
        auto splitter = to_split | lz::sv_split(" ");
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "", "" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SECTION("Starting and ending with one delimiter") {
        const std::string to_split = " Hello world test 123 ";
        using value_type = lz::basic_iterable<decltype(to_split.begin())>;
        auto splitter = lz::split(to_split, " ") | lz::map([](const value_type& vt) { return vt | lz::to<std::string>(); }) |
                        lz::to<std::vector>();
        std::vector<std::string> expected = { "", "Hello", "world", "test", "123", "" };
        REQUIRE(splitter == expected);
    }

    SECTION("Starting and ending with two delimiters") {
        const std::string to_split = "  Hello world test 123  ";
        auto splitter = lz::sv_split(to_split, " ");
        std::vector<std::string> expected = { "", "", "Hello", "world", "test", "123", "", "" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SECTION("No delimiters at all") {
        const std::string to_split = "Hello world test 123";
        auto splitter = lz::sv_split(to_split, " ");
        std::vector<std::string> expected = { "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SECTION("Multiple delimiters in the middle") {
        const std::string to_split = "Hello  world  test  123";
        auto splitter = lz::sv_split(to_split, " ");
        std::vector<std::string> expected = { "Hello", "", "world", "", "test", "", "123" };
        REQUIRE(lz::equal(splitter, expected));
    }
}

TEST_CASE("Splitter changing and creating elements", "[Splitter][Basic functionality]") {
    const std::string to_split = "Hello  world  test  123  ";
    const char* delimiter = "  ";
    auto splitter = lz::sv_split(to_split, delimiter);

    auto it = splitter.begin();

    SECTION("Should split on delimiter") {
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SECTION("Should be correct value type") {
        static_assert(std::is_same<decltype(*lz::sv_split(to_split, delimiter).begin()), lz::string_view>::value,
                      "should be the same");
        static_assert(std::is_same<decltype(*lz::s_split(to_split, delimiter).begin()), std::string>::value,
                      "should be the same");
        static_assert(std::is_same<decltype(*lz::split(to_split, delimiter).begin()),
                                   lz::basic_iterable<decltype(to_split.begin())>>::value,
                      "should be the same");
    }
}

TEST_CASE("Empty or one element string splitter") {
    SECTION("Empty") {
        std::string to_split;
        auto splitter = lz::split(to_split, " ");
        REQUIRE(lz::empty(splitter));
        REQUIRE(!lz::has_many(splitter));
        REQUIRE(!lz::has_one(splitter));
    }

    SECTION("One element") {
        std::string to_split = "Hello";
        auto splitter = lz::split(to_split, " ");
        REQUIRE(!lz::empty(splitter));
        REQUIRE(!lz::has_many(splitter));
        REQUIRE(lz::has_one(splitter));
    }

    SECTION("One element with delimiter") {
        std::string to_split = "Hello ";
        auto splitter = lz::split(to_split, " ");
        REQUIRE(!lz::empty(splitter));
        REQUIRE(lz::has_many(splitter));
        REQUIRE(!lz::has_one(splitter));
    }
}

TEST_CASE("Splitter binary operations", "[Splitter][Binary ops]") {
    std::string to_split = " Hello world test 123 ";
    auto splitter = to_split | lz::sv_split(" ");
    auto it = splitter.begin();

    REQUIRE(it->empty());

    SECTION("Operator++") {
        REQUIRE(it->empty());
        ++it;
        REQUIRE(*it == "Hello");
        ++it;
        REQUIRE(*it == "world");
        ++it;
        REQUIRE(*it == "test");
        ++it;
        REQUIRE(*it == "123");
        ++it;
        REQUIRE(it->empty());
        REQUIRE(it != splitter.end());
        ++it;
        REQUIRE(it == splitter.end());
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(it != splitter.end());
        while (it != splitter.end()) {
            ++it;
        }
        REQUIRE(it == splitter.end());
    }
}

TEST_CASE("Splitter to containers", "[Splitter][To container]") {
    std::string to_split = "Hello world test 123 ";
    auto splitter = lz::sv_split(to_split, " ");
    using View = typename decltype(splitter.begin())::value_type;

    SECTION("To array") {
        std::array<std::string, 5> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(expected, splitter));
    }

    SECTION("To vector") {
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(expected, splitter));
    }

    SECTION("To other container using to<>()") {
        std::list<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(expected, splitter));
    }

    SECTION("To map") {
        auto actual = splitter |
                      lz::map([](const lz::string_view v) { return std::make_pair(v.to_std_string(), v.to_std_string()); }) |
                      lz::to<std::map<std::string, std::string>>();

        std::map<std::string, std::string> expected = {
            std::make_pair(std::string("Hello"), std::string("Hello")),
            std::make_pair(std::string("world"), std::string("world")),
            std::make_pair(std::string("test"), std::string("test")),
            std::make_pair(std::string("123"), std::string("123")),
            std::make_pair(std::string(""), std::string("")),
        };

        REQUIRE(actual == expected);
    }

    SECTION("To unordered map") {
        auto actual = splitter |
                      lz::map([](const lz::string_view v) { return std::make_pair(v.to_std_string(), v.to_std_string()); }) |
                      lz::to<std::unordered_map<std::string, std::string>>();
        std::unordered_map<std::string, std::string> expected = { std::make_pair(std::string("Hello"), std::string("Hello")),
                                                                  std::make_pair(std::string("world"), std::string("world")),
                                                                  std::make_pair(std::string("test"), std::string("test")),
                                                                  std::make_pair(std::string("123"), std::string("123")),
                                                                  std::make_pair(std::string(""), std::string("")) };

        REQUIRE(actual == expected);
    }
}
