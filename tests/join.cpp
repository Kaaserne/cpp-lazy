#include <Lz/join.hpp>
#include <Lz/map.hpp>
#include <Lz/c_string.hpp>
#include <catch2/catch.hpp>
#include <sstream>


TEST_CASE("Join with sentinels") {
    auto c_string = lz::c_string("Hello, World!");
    auto join = lz::join(c_string, ", ");
    REQUIRE(join.to_string() == "H, e, l, l, o, ,,  , W, o, r, l, d, !");
    static_assert(!std::is_same<decltype(join.begin()), decltype(join.end())>::value, "Should be sentinel");
}

TEST_CASE("Join should convert to string", "[Join][Basic functionality]") {
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    std::vector<std::string> s = { "h", "e", "l", "l", "o" };
    auto join_int = lz::join(v, ", ");
    static_assert(std::is_same<decltype(join_int.begin()), decltype(join_int.end())>::value, "Should not be sentinel");
    auto join_str = lz::join(s, ", ");

    REQUIRE(join_int.to_string() == "1, 2, 3, 4, 5");
    REQUIRE(join_str.to_string() == "h, e, l, l, o");

    SECTION("String streams") {
        std::ostringstream ss;
        ss << join_int;
        REQUIRE(ss.str() == "1, 2, 3, 4, 5");
    }

    SECTION("Should convert to string") {
        REQUIRE(*join_int.begin() == "1");
        REQUIRE(*join_str.begin() == "h");
    }

    SECTION("Type checking") {
        static_assert(std::is_same<decltype(*join_str.begin()), std::string&>::value, "String container should be std::string&");
        static_assert(std::is_same<decltype(*join_int.begin()), std::string>::value, "Int container should be std::string");
    }

    SECTION("Should be correct size") {
        REQUIRE(std::distance(join_int.begin(), join_int.end()) == 9);
        REQUIRE(std::distance(join_str.begin(), join_str.end()) == 9);
    }

    SECTION("Immediate to string") {
        REQUIRE(lz::str_join(v) == "12345");
    }
}

TEST_CASE("Empty or one element join") {
    SECTION("Empty") {
        std::vector<int> v;
        auto join = lz::join(v, ", ");
        REQUIRE(lz::empty(join));
        REQUIRE(!lz::has_many(join));
        REQUIRE(!lz::has_one(join));
    }

    SECTION("One element") {
        std::vector<int> v = { 1 };
        auto join = lz::join(v, ", ");
        REQUIRE(!lz::empty(join));
        REQUIRE(!lz::has_many(join));
        REQUIRE(lz::has_one(join));
    }
}

TEST_CASE("Join binary operations", "[Join][Binary ops]") {
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    std::vector<std::string> s = { "h", "e", "l", "l", "o" };
    auto join_int = lz::join(v, ", ");
    auto join_str = lz::join(s, ", ");

    auto join_int_iter = join_int.begin();
    auto join_str_iter = join_str.begin();

    REQUIRE(*join_int_iter == "1");
    REQUIRE(*join_str_iter == "h");

    SECTION("Operator++") {
        ++join_int_iter;
        REQUIRE(*join_int_iter == ", ");

        ++join_str_iter;
        REQUIRE(*join_str_iter == ", ");
    }

    SECTION("Operator--") {
        ++join_int_iter, ++join_str_iter;

        REQUIRE(*--join_int_iter == "1");
        auto join_int_end = join_int.end();
        REQUIRE(*--join_int_end == "5");
        REQUIRE(*--join_int_end == ", ");

        --join_str_iter;
        REQUIRE(*join_str_iter == "h");
    }

    SECTION("Operator== & operator!=") {
        REQUIRE(join_int_iter != join_int.end());
        REQUIRE(join_str_iter != join_str.end());

        join_int_iter = join_int.end();
        join_str_iter = join_str.end();

        REQUIRE(join_int_iter == join_int.end());
        REQUIRE(join_str_iter == join_str.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        REQUIRE(*(join_int_iter + 2) == "2");
        REQUIRE(*(join_str_iter + 2) == "e");

        REQUIRE(*(join_int_iter + 3) == ", ");
        REQUIRE(*(join_str_iter + 3) == ", ");
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        join_int_iter = join_int.end();
        join_str_iter = join_str.end();

        REQUIRE(*(join_int_iter - 1) == "5");
        REQUIRE(*(join_str_iter - 1) == "o");

        REQUIRE(*(join_int_iter - 2) == ", ");
        REQUIRE(*(join_str_iter - 2) == ", ");

        join_int_iter = join_int.begin();
        REQUIRE(*(join_int_iter - -3) == ", ");
        REQUIRE(*(join_int_iter - -2) == "2");

        join_int_iter = join_int.end();
        REQUIRE(*(join_int_iter + -3) == "4");
        REQUIRE(*(join_int_iter + -4) == ", ");
    }

    SECTION("Operator-(Iterator)") {
        REQUIRE(std::distance(join_int_iter, join_int.end()) == 9);
        REQUIRE(std::distance(join_str_iter, join_str.end()) == 9);

        REQUIRE(join_int.end() - join_int_iter == 9);
        REQUIRE(join_str.end() - join_str_iter == 9);
    }

    SECTION("Operator[]()") {
        REQUIRE(join_int_iter[2] == "2");
        REQUIRE(join_int_iter[1] == ", ");

        REQUIRE(join_str_iter[2] == "e");
        REQUIRE(join_str_iter[3] == ", ");
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto join_int_dist = std::distance(join_int_iter, join_int.end());
        auto join_int_end = join_int.end();
        REQUIRE(join_int_iter < join_int_end);
        REQUIRE(join_int_iter + join_int_dist - 1 > join_int_end - join_int_dist);
        REQUIRE(join_int_iter + join_int_dist - 1 <= join_int_end);
        REQUIRE(join_int_iter + join_int_dist - 1 >= join_int_end - 1);

        auto join_str_dist = std::distance(join_str_iter, join_str.end());
        auto join_str_end = join_str.end();
        REQUIRE(join_str_iter < join_str_end);
        REQUIRE(join_str_iter + join_str_dist - 1 > join_str_end - join_str_dist);
        REQUIRE(join_str_iter + join_str_dist - 1 <= join_str_end);
        REQUIRE(join_str_iter + join_str_dist - 1 >= join_str_end - 1);
    }

    SECTION("String join double format") {
        std::array<double, 4> vec = { 1.1, 2.2, 3.3, 4.4 };
        auto doubles = lz::str_join(vec, ", ", "{:.2f}");
        REQUIRE(doubles == "1.10, 2.20, 3.30, 4.40");
    }
}
