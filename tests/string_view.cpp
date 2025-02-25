#define LZ_STANDALONE

#include <Lz/detail/compiler_checks.hpp>

#if !defined(LZ_HAS_STRING_VIEW)

#include <Lz/string_view.hpp>

#include <catch2/catch.hpp>

TEST_CASE("String view basic functionality", "[String view][Basic functionality]") {
    const std::string str = "Hello world";
    lz::string_view view(str.data(), str.size());

    SECTION("Should have correct size") {
        REQUIRE(view.size() == str.size());
    }

    SECTION("Should have correct data") {
        REQUIRE(view.data() == str.data());
    }

    SECTION("Should have correct length") {
        REQUIRE(view.length() == str.size());
    }

    SECTION("Should have correct begin") {
        REQUIRE(view.begin() == str.data());
    }

    SECTION("Should have correct end") {
        REQUIRE(view.end() == str.data() + str.size());
    }

    SECTION("Should convert to string") {
        REQUIRE(view.to_std_string() == str);
    }

    SECTION("Use static_cast") {
        REQUIRE(static_cast<std::string>(view) == str);
    }

    SECTION("Prefix and suffix removal") {
        view = lz::string_view("Hello world");
        view.remove_prefix(6);
        REQUIRE(view == "world");
        view = lz::string_view("Hello world");
        view.remove_suffix(5);
        REQUIRE(view == "Hello ");
    }

    SECTION("Contains") {
        REQUIRE(view.contains("Hello"));
    }

    SECTION("Find") {
        REQUIRE(view.find("world") == 6);
        REQUIRE(view.find("Hello") == 0);
        REQUIRE(view.find("zzz") == lz::string_view::npos);
    }

    SECTION("Operator==") {
        REQUIRE(view == str);
    }

    SECTION("Operator!=") {
        view = lz::string_view("Hello");
        REQUIRE(view != "Hello world!");
    }

    SECTION("Should have correct end") {
        REQUIRE(view.end() == str.data() + str.size());
    }

    SECTION("Should have correct operator[]") {
        REQUIRE(view[0] == str[0]);
        REQUIRE(view[1] == str[1]);
        REQUIRE(view[2] == str[2]);
        REQUIRE(view[3] == str[3]);
        REQUIRE(view[4] == str[4]);
        REQUIRE(view[5] == str[5]);
        REQUIRE(view[6] == str[6]);
        REQUIRE(view[7] == str[7]);
        REQUIRE(view[8] == str[8]);
        REQUIRE(view[9] == str[9]);
    }
}	
#endif