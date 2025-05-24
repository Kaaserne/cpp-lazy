#define LZ_STANDALONE

#include <Lz/algorithm.hpp>
#include <Lz/detail/compiler_checks.hpp>

#if !defined(LZ_HAS_STRING_VIEW)

#include <Lz/string_view.hpp>

#include <catch2/catch.hpp>

TEST_CASE("String view basic functionality") {
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

    SECTION("Empty") {
        lz::string_view empty_view;
        REQUIRE(empty_view.empty());
        REQUIRE(empty_view.size() == 0);
        REQUIRE(empty_view.data() == nullptr);
    }
}
#endif