#include <Lz/algorithm/empty.hpp>
#include <Lz/algorithm/equal.hpp>
#include <Lz/algorithm/has_many.hpp>
#include <Lz/algorithm/has_one.hpp>
#include <Lz/map.hpp>
#include <Lz/procs/to.hpp>
#include <Lz/split.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

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
    SUBCASE("No delimiters at end and begin") {
        const std::string to_split = "Hello world test 123";
        lz::sv_multiple_split_iterable<const std::string> splitter = lz::sv_split(to_split, " ");
        auto vec = splitter | lz::to<std::vector>();

        std::vector<lz::string_view> expected = { "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SUBCASE("Starting with one delimiter ending with none") {
        const std::string to_split = " Hello world test 123";
        auto splitter = to_split | lz::s_split(" ");
        std::vector<std::string> expected = { "", "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SUBCASE("Starting with two delimiters ending with none") {
        const std::string to_split = "  Hello world test 123";
        auto splitter = to_split | lz::sv_split(" ");
        std::vector<std::string> expected = { "", "", "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SUBCASE("Ending with one delimiter starting with none") {
        const std::string to_split = "Hello world test 123 ";
        auto splitter = lz::sv_split(to_split, " ");

        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SUBCASE("Ending with two delimiters starting with none") {
        const std::string to_split = "Hello world test 123  ";
        auto splitter = to_split | lz::sv_split(" ");
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "", "" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SUBCASE("Starting and ending with one delimiter") {
        const std::string to_split = " Hello world test 123 ";
        using value_type = lz::basic_iterable<decltype(to_split.begin())>;
        auto splitter = lz::split(to_split, " ") | lz::map([](const value_type& vt) { return vt | lz::to<std::string>(); }) |
                        lz::to<std::vector>();
        std::vector<std::string> expected = { "", "Hello", "world", "test", "123", "" };
        REQUIRE(splitter == expected);
    }

    SUBCASE("Starting and ending with two delimiters") {
        const std::string to_split = "  Hello world test 123  ";
        auto splitter = lz::sv_split(to_split, " ");
        std::vector<std::string> expected = { "", "", "Hello", "world", "test", "123", "", "" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SUBCASE("No delimiters at all") {
        const std::string to_split = "Hello world test 123";
        auto splitter = lz::sv_split(to_split, " ");
        std::vector<std::string> expected = { "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SUBCASE("Multiple delimiters in the middle") {
        const std::string to_split = "Hello  world  test  123";
        auto splitter = lz::sv_split(to_split, " ");
        std::vector<std::string> expected = { "Hello", "", "world", "", "test", "", "123" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SUBCASE("Operator=(default_sentinel_t)") {
        SUBCASE("not ending with delim") {
            const std::string s = "hello, world! This is a test.";

            SUBCASE("single") {
                auto splitted = lz::sv_split(s, ' ');
                auto common = make_sentinel_assign_op_tester(splitted);
                auto expected = { "hello,", "world!", "This", "is", "a", "test." };
                REQUIRE(lz::equal(common, expected));
            }

            SUBCASE("multiple") {
                auto splitted2 = lz::sv_split(s, ", ");
                auto common2 = make_sentinel_assign_op_tester(splitted2);
                auto expected2 = { "hello", "world! This is a test." };
                REQUIRE(lz::equal(common2, expected2));
            }
        }

        SUBCASE("ending with delim") {
            SUBCASE("single") {
                const std::string s = "hello, world! This is a test. ";
                auto splitted = lz::sv_split(s, ' ');
                auto common = make_sentinel_assign_op_tester(splitted);
                auto expected = { "hello,", "world!", "This", "is", "a", "test.", "" };
                REQUIRE(lz::equal(common, expected));
            }

            SUBCASE("multiple") {
                const std::string s = "hello, world! This is a test, ";
                auto splitted2 = lz::sv_split(s, ", ");
                auto common2 = make_sentinel_assign_op_tester(splitted2);
                auto expected = { "hello", "world! This is a test", "" };
                REQUIRE(lz::equal(common2, expected));
            }
        }
    }
}

TEST_CASE("Splitter changing and creating elements") {
    const std::string to_split = "Hello  world  test  123  ";
    const char* delimiter = "  ";
    auto splitter = lz::sv_split(to_split, delimiter);

    SUBCASE("Should split on delimiter") {
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SUBCASE("Should be correct value type") {
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
    SUBCASE("Empty") {
        std::string to_split;
        using it = std::string::iterator;
        using iterable = lz::split_iterable<lz::basic_iterable<it>, std::string, lz::copied_sv>;
        iterable splitter = lz::split(to_split, " ");

        REQUIRE(lz::empty(splitter));
        REQUIRE_FALSE(lz::has_many(splitter));
        REQUIRE_FALSE(lz::has_one(splitter));
    }

    SUBCASE("One element") {
        std::string to_split = "Hello";
        auto splitter = lz::split(to_split, " ");
        REQUIRE_FALSE(lz::empty(splitter));
        REQUIRE_FALSE(lz::has_many(splitter));
        REQUIRE(lz::has_one(splitter));
    }

    SUBCASE("One element with delimiter") {
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

    SUBCASE("Operator++") {
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

    SUBCASE("Operator== & Operator!=") {
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
    auto cmp = [](const std::string& a, const lz::string_view& b) {
        return a == std::string{ b.data(), b.size() };
    };

    SUBCASE("To array") {
        std::array<std::string, 5> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(expected, splitter, cmp));
    }

    SUBCASE("To vector") {
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(expected, splitter, cmp));
    }

    SUBCASE("To other container using to<>()") {
        std::list<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(expected, splitter, cmp));
    }

    SUBCASE("To map") {
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

    SUBCASE("To unordered map") {
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
    SUBCASE("No delimiters at end and begin") {
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

    SUBCASE("Starting with one delimiter ending with none") {
        const std::string to_split = " Hello world test 123";
        lz::s_single_split_iterable<const std::string> splitter = lz::s_split(to_split, ' ');
        std::vector<std::string> expected = { "", "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SUBCASE("Starting with two delimiters ending with none") {
        const std::string to_split = "  Hello world test 123";
        auto splitter = lz::sv_split(to_split, ' ');
        std::vector<std::string> expected = { "", "", "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SUBCASE("Ending with one delimiter starting with none") {
        const std::string to_split = "Hello world test 123 ";
        auto splitter = lz::sv_split(to_split, ' ');
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SUBCASE("Ending with two delimiters starting with none") {
        const std::string to_split = "Hello world test 123  ";
        auto splitter = to_split | lz::sv_split(' ');
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "", "" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SUBCASE("Starting and ending with one delimiter") {
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

    SUBCASE("Starting and ending with two delimiters") {
        const std::string to_split = "  Hello world test 123  ";
        auto splitter = lz::sv_split(to_split, ' ');
        std::vector<std::string> expected = { "", "", "Hello", "world", "test", "123", "", "" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SUBCASE("No delimiters at all") {
        const std::string to_split = "Hello world test 123";
        auto splitter = lz::sv_split(to_split, ' ');
        std::vector<std::string> expected = { "Hello", "world", "test", "123" };
        REQUIRE(lz::equal(splitter, expected));

        auto it = splitter.begin();
        REQUIRE(it == splitter.begin());
        it = splitter.end();
        REQUIRE(it == splitter.end());
    }

    SUBCASE("Multiple delimiters in the middle") {
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

    SUBCASE("Should split on delimiter") {
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(splitter, expected));
    }

    SUBCASE("Should be correct value type") {
        static_assert(std::is_same<decltype(*lz::sv_split(to_split, ' ').begin()), lz::string_view>::value,
                      "should be the same");
        static_assert(std::is_same<decltype(*lz::s_split(to_split, ' ').begin()), std::string>::value, "should be the same");
        static_assert(
            std::is_same<decltype(*lz::split(to_split, ' ').begin()), lz::basic_iterable<decltype(to_split.begin())>>::value,
            "should be the same");
    }
}

TEST_CASE("Empty or one element string one element splitter") {
    SUBCASE("Empty") {
        std::string to_split;
        auto splitter = lz::split(to_split, ' ');
        REQUIRE(lz::empty(splitter));
        REQUIRE_FALSE(lz::has_many(splitter));
        REQUIRE_FALSE(lz::has_one(splitter));
    }

    SUBCASE("One element") {
        std::string to_split = "Hello";
        auto splitter = lz::split(to_split, ' ');
        REQUIRE_FALSE(lz::empty(splitter));
        REQUIRE_FALSE(lz::has_many(splitter));
        REQUIRE(lz::has_one(splitter));
    }

    SUBCASE("One element with delimiter") {
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

    SUBCASE("Operator++") {
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

    SUBCASE("Operator== & Operator!=") {
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
    auto cmp = [](const std::string& a, const lz::string_view& b) {
        return a == std::string{ b.data(), b.size() };
    };

    SUBCASE("To array") {
        std::array<std::string, 5> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(expected, splitter, cmp));
    }

    SUBCASE("To vector") {
        std::vector<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(expected, splitter, cmp));
    }

    SUBCASE("To other container using to<>()") {
        std::list<std::string> expected = { "Hello", "world", "test", "123", "" };
        REQUIRE(lz::equal(expected, splitter, cmp));
    }

    SUBCASE("To map") {
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

    SUBCASE("To unordered map") {
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
