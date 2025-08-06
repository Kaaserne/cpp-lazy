#define LZ_STANDALONE

#include <Lz/split.hpp>
#include <Lz/stream.hpp>
#include <doctest/doctest.h>
#include <iostream>
#include <vector>

TEST_CASE("Formatting and compile tests") {
    SUBCASE("Compile test") {
        std::string to_split = "Hello, World!";
        auto split = lz::sv_split(to_split, ", ");
        static_assert(std::is_same<decltype(*split.begin()), lz::string_view>::value, "Should be string_view");
    }

    SUBCASE("Format test non empty") {
        std::vector<int> vec = { 1, 2, 3, 4 };
        REQUIRE((vec | lz::format) == "1, 2, 3, 4");
        REQUIRE(lz::format(vec) == "1, 2, 3, 4");

        REQUIRE((vec | lz::format(",")) == "1,2,3,4");
        REQUIRE(lz::format(vec, ",") == "1,2,3,4");

        std::streambuf* old_cout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());

        lz::format(vec, std::cout);
        REQUIRE(oss.str() == "1, 2, 3, 4");
        oss.str("");

        lz::format(vec, std::cout, ",");
        REQUIRE(oss.str() == "1,2,3,4");
        oss.str("");

        vec | lz::format(std::cout);
        REQUIRE(oss.str() == "1, 2, 3, 4");
        oss.str("");

        vec | lz::format(std::cout, ",");
        REQUIRE(oss.str() == "1,2,3,4");
        oss.str("");

        std::cout.rdbuf(old_cout);

#if defined(LZ_HAS_FORMAT)

        REQUIRE((vec | lz::format(", ", "{}")) == "1, 2, 3, 4");
        REQUIRE(lz::format(vec, ", ", "{}") == "1, 2, 3, 4");

        vec | lz::format(std::cout, ", ", "{:02d}");
        REQUIRE(oss.str() == "01, 02, 03, 04");
        oss.str("");

#endif
    }

    SUBCASE("Format empty") {
        std::vector<int> vec = {};
        REQUIRE((vec | lz::format) == "");
        REQUIRE(lz::format(vec) == "");

        REQUIRE((vec | lz::format(",")) == "");
        REQUIRE(lz::format(vec, ",") == "");

#if defined(LZ_HAS_FORMAT)

        REQUIRE((vec | lz::format(", ", "{}")) == "");
        REQUIRE(lz::format(vec, ", ", "{}") == "");

#endif
    }

    SUBCASE("Format one") {
        std::vector<int> vec = { 1 };
        REQUIRE((vec | lz::format) == "1");
        REQUIRE(lz::format(vec) == "1");

        REQUIRE((vec | lz::format(",")) == "1");
        REQUIRE(lz::format(vec, ",") == "1");

#if defined(LZ_HAS_FORMAT)

        REQUIRE((vec | lz::format(", ", "{}")) == "1");
        REQUIRE(lz::format(vec, ", ", "{}") == "1");

#endif
    }

    SUBCASE("Ostream test non empty") {
        std::array<int, 4> arr = { 1, 2, 3, 4 };
        lz::basic_iterable<std::array<int, 4>::iterator> iterable(arr);
        std::ostringstream oss;
        oss << iterable;
        REQUIRE(oss.str() == "1, 2, 3, 4");
    }

    SUBCASE("Ostream test empty") {
        std::array<int, 0> arr = {};
        lz::basic_iterable<std::array<int, 0>::iterator> iterable(arr);
        std::ostringstream oss;
        oss << iterable;
        REQUIRE(oss.str() == "");
    }

    SUBCASE("Ostream test one element") {
        std::array<int, 1> arr = { 1 };
        lz::basic_iterable<std::array<int, 1>::iterator> iterable(arr);
        std::ostringstream oss;
        oss << iterable;
        REQUIRE(oss.str() == "1");
    }

    SUBCASE("std::cout test non empty") {
        std::streambuf* old_cout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());

        std::array<int, 4> arr = { 1, 2, 3, 4 };
        lz::basic_iterable<std::array<int, 4>::iterator> iterable(arr);
        std::cout << iterable;
        REQUIRE(oss.str() == "1, 2, 3, 4");

        std::cout.rdbuf(old_cout);
    }

    SUBCASE("std::cout test empty") {
        std::streambuf* old_cout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());

        std::array<int, 0> arr = {};
        lz::basic_iterable<std::array<int, 0>::iterator> iterable(arr);
        std::cout << iterable;
        REQUIRE(oss.str() == "");

        std::cout.rdbuf(old_cout);
    }

    SUBCASE("std::cout test one element") {
        std::cout.flush();
        std::streambuf* old_cout = std::cout.rdbuf();
        std::ostringstream oss;
        std::cout.rdbuf(oss.rdbuf());

        std::array<int, 1> arr = { 1 };
        lz::basic_iterable<std::array<int, 1>::iterator> iterable(arr);
        std::cout << iterable;
        REQUIRE(oss.str() == "1");

        std::cout.rdbuf(old_cout);
    }
}
