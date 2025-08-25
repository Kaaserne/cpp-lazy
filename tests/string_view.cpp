#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-macros"
#endif

#define LZ_STANDALONE

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <doctest/doctest.h>
#include <pch.hpp>
#include <Lz/string_view.hpp>

#if !defined(LZ_HAS_STRING_VIEW)

#include <Lz/string_view.hpp>
#include <doctest/doctest.h>

TEST_CASE("String view basic functionality") {
    constexpr const char* cstr = "Hello world!";
    constexpr lz::string_view view("Hello world!");

    SUBCASE("Should have correct size") {
        static_assert(view.size() == lz::detail::constexpr_str_len(cstr), "view.size() != cstr size");
    }

    SUBCASE("Should have correct data") {
        REQUIRE(view.data() == cstr);
    }

    SUBCASE("Should have correct length") {
        static_assert(view.length() == lz::detail::constexpr_str_len(cstr), "view.length() != cstr length");
    }

    SUBCASE("Should have correct begin") {
        REQUIRE(view.begin() == cstr);
    }

    SUBCASE("Should have correct end") {
        REQUIRE(view.end() == cstr + lz::detail::constexpr_str_len(cstr));
    }

    SUBCASE("Empty") {
        constexpr lz::string_view empty_view;
        static_assert(empty_view.size() == 0, "empty_view.size() != 0");
        static_assert(empty_view.length() == 0, "empty_view.length() != 0");
        static_assert(empty_view.data() == nullptr, "empty_view.data() != nullptr");
        static_assert(empty_view.begin() == nullptr, "empty_view.begin() != nullptr");
        static_assert(empty_view.end() == nullptr, "empty_view.end() != nullptr");
        static_assert(empty_view.begin() == empty_view.end(), "empty_view.begin() != empty_view.end()");
    }
}
#endif
