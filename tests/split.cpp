#include <Lz/map.hpp>
#include <Lz/split.hpp>
#include <catch2/catch.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Split with custom container") {
    std::string to_split = "Hello world test 123";
#ifdef LZ_HAS_CXX_11
    auto splitter = to_split | lz::t_split<std::vector<char>>{}(" ");
#else
    auto splitter = to_split | lz::t_split<std::vector<char>>(" ");
#endif
    std::vector<std::vector<char>> expected = {
        { 'H', 'e', 'l', 'l', 'o' }, { 'w', 'o', 'r', 'l', 'd' }, { 't', 'e', 's', 't' }, { '1', '2', '3' }
    };
    REQUIRE(lz::equal(splitter, expected));
}

TEST_CASE("Splitter permutations") {
    SECTION("No delimiters at end and begin") {
        const std::string to_split = "Hello world test 123";
        lz::sv_multiple_split_iterable<const std::string> splitter = lz::sv_split(to_split, " ");
        auto vec = splitter | lz::to<std::vector>();

        std::vector<lz::string_view> expected = { "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("Starting with one delimiter ending with none") {
        const std::string to_split = " Hello world test 123";
        auto splitter = to_split | lz::s_split(" ");
        std::vector<std::string> expected = { "", "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("Starting with two delimiters ending with none") {
        const std::string to_split = "  Hello world test 123";
        auto splitter = to_split | lz::sv_split(" ");
        std::vector<std::string> expected = { "", "", "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("Ending with one delimiter starting with none") {
        const std::string to_split = "Hello world test 123 ";
        auto splitter = lz::sv_split(to_split, " ");

        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("Ending with two delimiters starting with none") {
        const std::string to_split = "Hello world test 123  ";
        auto splitter = to_split | lz::sv_split(" ");
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "", "" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("Starting and ending with one delimiter") {
        const std::string to_split = " Hello world test 123 ";
        using value_type = lz::basic_iterable<decltype(to_split.begin())>;
        auto splitter = lz::split(to_split, " ") | lz::map([](const value_type& vt) { return vt | lz::to<std::string>(); }) |
                        lz::to<std::vector>();
        std::vector<std::string> expected = { "", "Hello", "world", "test", "123", "" };
        REQUIRE(splitter == expected);

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("Starting and ending with two delimiters") {
        const std::string to_split = "  Hello world test 123  ";
        auto splitter = lz::sv_split(to_split, " ");
        std::vector<std::string> expected = { "", "", "Hello", "world", "test", "123", "", "" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("No delimiters at all") {
        const std::string to_split = "Hello world test 123";
        auto splitter = lz::sv_split(to_split, " ");
        std::vector<std::string> expected = { "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("Multiple delimiters in the middle") {
        const std::string to_split = "Hello  world  test  123";
        auto splitter = lz::sv_split(to_split, " ");
        std::vector<std::string> expected = { "Hello", "", "world", "", "test", "", "123" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }
}

TEST_CASE("Splitter changing and creating elements") {
    const std::string to_split = "Hello  world  test  123  ";
    const char* delimiter = "  ";
    auto splitter = lz::sv_split(to_split, delimiter);

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
        using it = std::string::iterator;
        using iterable = lz::split_iterable<lz::basic_iterable<it>, std::string, lz::copied_sv>;
        iterable splitter = lz::split(to_split, " ");

        REQUIRE(lz::empty(splitter));
        REQUIRE_FALSE(lz::has_many(splitter));
        REQUIRE_FALSE(lz::has_one(splitter));
    }

    SECTION("One element") {
        std::string to_split = "Hello";
        auto splitter = lz::split(to_split, " ");
        REQUIRE_FALSE(lz::empty(splitter));
        REQUIRE_FALSE(lz::has_many(splitter));
        REQUIRE(lz::has_one(splitter));
    }

    SECTION("One element with delimiter") {
        std::string to_split = "Hello ";
        auto splitter = lz::split(to_split, " ");
        REQUIRE_FALSE(lz::empty(splitter));
        REQUIRE(lz::has_many(splitter));
        REQUIRE_FALSE(lz::has_one(splitter));
    }
}

TEST_CASE("Splitter binary operations") {
    std::string to_split = " Hello world test 123 ";
    lz::sv_multiple_split_iterable<std::string> splitter = to_split | lz::sv_split(" ");
    auto it = splitter.begin();

    REQUIRE(*it == "");

    SECTION("Operator++") {
        REQUIRE(*it == "");
        ++it;
        REQUIRE(*it == "Hello");
        ++it;
        REQUIRE(*it == "world");
        ++it;
        REQUIRE(*it == "test");
        ++it;
        REQUIRE(*it == "123");
        ++it;
        REQUIRE(*it == "");
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

TEST_CASE("Splitter to containers") {
    std::string to_split = "Hello world test 123 ";
    auto splitter = lz::sv_split(to_split, " ");

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
        auto actual = splitter | lz::map([](const lz::string_view v) {
                          return std::make_pair(std::string{ v.data(), v.size() }, std::string{ v.data(), v.size() });
                      }) |
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
        auto actual = splitter | lz::map([](const lz::string_view v) {
                          return std::make_pair(std::string{ v.data(), v.size() }, std::string{ v.data(), v.size() });
                      }) |
                      lz::to<std::unordered_map<std::string, std::string>>();
        std::unordered_map<std::string, std::string> expected = { std::make_pair(std::string("Hello"), std::string("Hello")),
                                                                  std::make_pair(std::string("world"), std::string("world")),
                                                                  std::make_pair(std::string("test"), std::string("test")),
                                                                  std::make_pair(std::string("123"), std::string("123")),
                                                                  std::make_pair(std::string(""), std::string("")) };

        REQUIRE(actual == expected);
    }
}

// ---------------------- One element splitters ----------------------

TEST_CASE("One element splitter with custom container") {
    std::string to_split = "Hello world test 123";
#ifdef LZ_HAS_CXX_11
    lz::split_iterable<std::vector<char>, std::string, char> splitter = to_split | lz::t_split<std::vector<char>>{}(' ');
#else
    lz::split_iterable<std::vector<char>, std::string, char> splitter = lz::t_split<std::vector<char>>(to_split, ' ');
#endif
    std::vector<std::vector<char>> expected = {
        { 'H', 'e', 'l', 'l', 'o' }, { 'w', 'o', 'r', 'l', 'd' }, { 't', 'e', 's', 't' }, { '1', '2', '3' }
    };
    REQUIRE(lz::equal(splitter, expected));
}

TEST_CASE("One element splitter permutations") {
    SECTION("No delimiters at end and begin") {
        const std::string to_split = "Hello world test 123";
        lz::sv_single_split_iterable<const std::string> splitter = lz::sv_split(to_split, ' ');
        auto vec = splitter | lz::to<std::vector>();

        std::vector<lz::string_view> expected = { "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("Starting with one delimiter ending with none") {
        const std::string to_split = " Hello world test 123";
        lz::s_single_split_iterable<const std::string> splitter = lz::s_split(to_split, ' ');
        std::vector<std::string> expected = { "", "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("Starting with two delimiters ending with none") {
        const std::string to_split = "  Hello world test 123";
        auto splitter = lz::sv_split(to_split, ' ');
        std::vector<std::string> expected = { "", "", "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("Ending with one delimiter starting with none") {
        const std::string to_split = "Hello world test 123 ";
        auto splitter = lz::sv_split(to_split, ' ');
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("Ending with two delimiters starting with none") {
        const std::string to_split = "Hello world test 123  ";
        auto splitter = to_split | lz::sv_split(' ');
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "", "" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("Starting and ending with one delimiter") {
        const std::string to_split = " Hello world test 123 ";
        using value_type = lz::basic_iterable<decltype(to_split.begin())>;
        auto splitter = lz::split(to_split, ' ') | lz::map([](const value_type& vt) { return vt | lz::to<std::string>(); }) |
                        lz::to<std::vector>();
        std::vector<std::string> expected = { "", "Hello", "world", "test", "123", "" };
        REQUIRE(splitter == expected);

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("Starting and ending with two delimiters") {
        const std::string to_split = "  Hello world test 123  ";
        auto splitter = lz::sv_split(to_split, ' ');
        std::vector<std::string> expected = { "", "", "Hello", "world", "test", "123", "", "" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("No delimiters at all") {
        const std::string to_split = "Hello world test 123";
        auto splitter = lz::sv_split(to_split, ' ');
        std::vector<std::string> expected = { "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SECTION("Multiple delimiters in the middle") {
        const std::string to_split = "Hello  world  test  123";
        auto splitter = lz::sv_split(to_split, ' ');
        std::vector<std::string> expected = { "Hello", "", "world", "", "test", "", "123" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }
}

TEST_CASE("One element splitter changing and creating elements") {
    const std::string to_split = "Hello world test 123 ";
    auto splitter = lz::sv_split(to_split, ' ');

    SECTION("Should split on delimiter") {
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SECTION("Should be correct value type") {
        static_assert(std::is_same<decltype(*lz::sv_split(to_split, ' ').begin()), lz::string_view>::value,
                      "should be the same");
        static_assert(std::is_same<decltype(*lz::s_split(to_split, ' ').begin()), std::string>::value, "should be the same");
        static_assert(
            std::is_same<decltype(*lz::split(to_split, ' ').begin()), lz::basic_iterable<decltype(to_split.begin())>>::value,
            "should be the same");
    }
}

TEST_CASE("Empty or one element string one element splitter") {
    SECTION("Empty") {
        std::string to_split;
        auto splitter = lz::split(to_split, ' ');
        REQUIRE(lz::empty(splitter));
        REQUIRE_FALSE(lz::has_many(splitter));
        REQUIRE_FALSE(lz::has_one(splitter));
    }

    SECTION("One element") {
        std::string to_split = "Hello";
        auto splitter = lz::split(to_split, ' ');
        REQUIRE_FALSE(lz::empty(splitter));
        REQUIRE_FALSE(lz::has_many(splitter));
        REQUIRE(lz::has_one(splitter));
    }

    SECTION("One element with delimiter") {
        std::string to_split = "Hello ";
        auto splitter = lz::split(to_split, ' ');
        REQUIRE_FALSE(lz::empty(splitter));
        REQUIRE(lz::has_many(splitter));
        REQUIRE_FALSE(lz::has_one(splitter));
    }
}

TEST_CASE("One element splitter binary operations") {
    std::string to_split = " Hello world test 123 ";
    auto splitter = to_split | lz::sv_split(' ');
    auto it = splitter.begin();

    REQUIRE(*it == "");

    SECTION("Operator++") {
        REQUIRE(*it == "");
        ++it;
        REQUIRE(*it == "Hello");
        ++it;
        REQUIRE(*it == "world");
        ++it;
        REQUIRE(*it == "test");
        ++it;
        REQUIRE(*it == "123");
        ++it;
        REQUIRE(*it == "");
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

TEST_CASE("One element splitter to containers") {
    std::string to_split = "Hello world test 123 ";
    auto splitter = lz::sv_split(to_split, ' ');

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
        auto actual = splitter | lz::map([](const lz::string_view v) {
                          return std::make_pair(std::string{ v.data(), v.size() }, std::string{ v.data(), v.size() });
                      }) |
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
        auto actual = splitter | lz::map([](const lz::string_view v) {
                          return std::make_pair(std::string{ v.data(), v.size() }, std::string{ v.data(), v.size() });
                      }) |
                      lz::to<std::unordered_map<std::string, std::string>>();
        std::unordered_map<std::string, std::string> expected = { std::make_pair(std::string("Hello"), std::string("Hello")),
                                                                  std::make_pair(std::string("world"), std::string("world")),
                                                                  std::make_pair(std::string("test"), std::string("test")),
                                                                  std::make_pair(std::string("123"), std::string("123")),
                                                                  std::make_pair(std::string(""), std::string("")) };

        REQUIRE(actual == expected);
    }
}