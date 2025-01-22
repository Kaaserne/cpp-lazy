#include <Lz/c_string.hpp>
#include <Lz/split.hpp>
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <list>

TEST_CASE("Splitter permutations") {
    SECTION("Starting with one delimiter ending with none") {
        const std::string to_split = " Hello world test 123";
        auto splitter =
            lz::split(to_split, " ").transform_as<std::vector<std::string>>([](const auto& v) { return v.to_string(); });
        std::vector<std::string> expected = { "", "Hello", "world", "test", "123" };
        REQUIRE(splitter == expected);
    }

    SECTION("Starting with two delimiters ending with none") {
        const std::string to_split = "  Hello world test 123";
        auto splitter =
            lz::split(to_split, " ").transform_as<std::vector<std::string>>([](const auto& v) { return v.to_string(); });
        std::vector<std::string> expected = { "", "", "Hello", "world", "test", "123" };
        REQUIRE(splitter == expected);
    }

    SECTION("Ending with one delimiter starting with none") {
        const std::string to_split = "Hello world test 123 ";
        auto splitter =
            lz::split(to_split, " ").transform_as<std::vector<std::string>>([](const auto& v) { return v.to_string(); });
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(splitter == expected);
    }

    SECTION("Ending with two delimiters starting with none") {
        const std::string to_split = "Hello world test 123  ";
        auto splitter =
            lz::split(to_split, " ").transform_as<std::vector<std::string>>([](const auto& v) { return v.to_string(); });
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "", "" };
        REQUIRE(splitter == expected);
    }

    SECTION("Starting and ending with one delimiter") {
        const std::string to_split = " Hello world test 123 ";
        auto splitter =
            lz::split(to_split, " ").transform_as<std::vector<std::string>>([](const auto& v) { return v.to_string(); });
        std::vector<std::string> expected = { "", "Hello", "world", "test", "123", "" };
        REQUIRE(splitter == expected);
    }

    SECTION("Starting and ending with two delimiters") {
        const std::string to_split = "  Hello world test 123  ";
        auto splitter =
            lz::split(to_split, " ").transform_as<std::vector<std::string>>([](const auto& v) { return v.to_string(); });
        std::vector<std::string> expected = { "", "", "Hello", "world", "test", "123", "", "" };
        REQUIRE(splitter == expected);
    }

    SECTION("No delimiters at all") {
        const std::string to_split = "Hello world test 123";
        auto splitter =
            lz::split(to_split, " ").transform_as<std::vector<std::string>>([](const auto& v) { return v.to_string(); });
        std::vector<std::string> expected = { "Hello", "world", "test", "123" };
        REQUIRE(splitter == expected);
    }

    SECTION("Multiple delimiters in the middle") {
        const std::string to_split = "Hello  world  test  123";
        auto splitter =
            lz::split(to_split, " ").transform_as<std::vector<std::string>>([](const auto& v) { return v.to_string(); });
        std::vector<std::string> expected = { "Hello", "", "world", "", "test", "", "123" };
        REQUIRE(splitter == expected);
    }
}

TEST_CASE("Splitter changing and creating elements", "[Splitter][Basic functionality]") {
    const std::string to_split = "Hello  world  test  123  ";
    const char* delimiter = "  ";
    auto splitter = lz::split(to_split, delimiter);

    auto it = splitter.begin();

    SECTION("Should split on delimiter") {
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(static_cast<std::size_t>(splitter.distance()) == expected.size());
        for (std::size_t i = 0; i < expected.size(); i++, ++it) {
            auto string = lz::to_string_view(*it);
            REQUIRE(string == expected[i]);
        }
        REQUIRE(it == splitter.end());
    }

    SECTION("Should be std::string") {
        REQUIRE(std::is_same<decltype(*it), lz::basic_iterable<decltype(to_split.begin())>>::value);
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
    auto splitter = lz::split(to_split, " ");
    auto it = splitter.begin();

    REQUIRE(it->empty());

    SECTION("Operator++") {
        REQUIRE(it->empty());
        ++it;
        REQUIRE(lz::to_string_view(*it) == "Hello");
        ++it;
        REQUIRE(lz::to_string_view(*it) == "world");
        ++it;
        REQUIRE(lz::to_string_view(*it) == "test");
        ++it;
        REQUIRE(lz::to_string_view(*it) == "123");
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
    auto splitter = lz::split(to_split, " ");
    using View = typename decltype(splitter.begin())::value_type;

    SECTION("To array") {
        auto actual = splitter.transform_as<std::array<std::string, 5>>([](const View& v) { return v.to_string(); });
        std::array<std::string, actual.size()> expected = { "Hello", "world", "test", "123", "" };

        REQUIRE(actual == expected);
    }

    SECTION("To vector") {
        std::vector<std::string> actual =
            splitter.transform_as<std::vector<std::string>>([](const View& v) { return v.to_string(); });
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };

        REQUIRE(actual == expected);
    }

    SECTION("To other container using to<>()") {
        std::list<std::string> actual =
            splitter.transform_as<std::list<std::string>>([](const View& v) { return v.to_string(); });
        std::list<std::string> expected = { "Hello", "world", "test", "123", "" };

        REQUIRE(actual == expected);
    }

    SECTION("To map") {
        std::map<std::string, std::string> actual =
            splitter.to_map([](const View& v) { return std::make_pair(v.to_string(), v.to_string()); });
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
        std::unordered_map<std::string, std::string> actual =
            splitter.to_unordered_map([](const View& v) { return std::make_pair(v.to_string(), v.to_string()); });
        std::unordered_map<std::string, std::string> expected = { std::make_pair(std::string("Hello"), std::string("Hello")),
                                                                  std::make_pair(std::string("world"), std::string("world")),
                                                                  std::make_pair(std::string("test"), std::string("test")),
                                                                  std::make_pair(std::string("123"), std::string("123")),
                                                                  std::make_pair(std::string(""), std::string("")) };

        REQUIRE(actual == expected);
    }
}
