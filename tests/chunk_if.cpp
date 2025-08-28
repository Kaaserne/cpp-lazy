#include <Lz/c_string.hpp>
#include <Lz/chunk_if.hpp>
#include <Lz/common.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

TEST_CASE("Chunk if custom value type") {
    auto str = lz::c_string(";hello;world;");
    std::function<bool(char)> func = [](char c) noexcept {
        return c == ';';
    };
#ifdef LZ_HAS_CXX_11
    lz::t_chunk_if_iterable<std::vector<char>, decltype(str), decltype(func)> chunked =
        str | lz::t_chunk_if<std::vector<char>>{}(std::move(func));
#else
    lz::t_chunk_if_iterable<std::vector<char>, decltype(str), decltype(func)> chunked =
        str | lz::t_chunk_if<std::vector<char>>(std::move(func));
#endif
    std::vector<std::vector<char>> expected = { {}, { 'h', 'e', 'l', 'l', 'o' }, { 'w', 'o', 'r', 'l', 'd' }, {} };
    REQUIRE(lz::equal(chunked, expected));
}

TEST_CASE("chunk if operator=(default_sentinel)") {
    auto fun = [](int a) {
        return a % 2 == 0;
    };

    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
    auto chunked = lz::chunk_if(vec, fun);
    using value_type = lz::val_iterable_t<decltype(chunked)>;
    auto common = lz::common(chunked);
    std::vector<std::vector<int>> expected = { { 1 }, { 3 }, { 5 }, {} };
    REQUIRE(lz::equal(common, expected, [](value_type a, const std::vector<int>& b) { return lz::equal(a, b); }));

    vec = { 1, 2, 3, 4, 5 };
    chunked = lz::chunk_if(vec, fun);
    common = lz::common(chunked);
    expected = { { 1 }, { 3 }, { 5 } };
    REQUIRE(lz::equal(common, expected, [](value_type a, const auto& b) { return lz::equal(a, b); }));
}

TEST_CASE("Chunk if with sentinels") {
    auto cstr = lz::c_string("hello world; this is a message;;");

    std::function<bool(char)> predicate = [](char c) noexcept {
        return c == ';';
    };
    lz::chunk_if_iterable<decltype(cstr), decltype(predicate)> chunked = lz::chunk_if(cstr, std::move(predicate));

    std::vector<std::vector<char>> expected = { { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd' },
                                                { ' ', 't', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 'm', 'e', 's', 's', 'a',
                                                  'g', 'e' },
                                                {},
                                                {} };
    REQUIRE(lz::equal(chunked, expected,
                      [](const decltype(chunked)::value_type a, const std::vector<char>& b) { return lz::equal(a, b); }));
}

TEST_CASE("Non string literal test") {
    std::function<bool(int)> is_even = [](int i) noexcept {
        return i % 2 == 0;
    };
    std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
    auto chunked = lz::chunk_if(arr, is_even);
    std::vector<std::vector<int>> expected = { { 1 }, { 3 }, { 5 } };
    REQUIRE(lz::equal(chunked, expected,
                      [](const decltype(chunked)::value_type& a, const std::vector<int>& b) { return lz::equal(a, b); }));
}

TEST_CASE("Empty or one element chunk_if") {
    SUBCASE("Empty") {
        std::string s;
        auto chunked = lz::chunk_if(s, [](char c) { return c == ';'; });
        REQUIRE(lz::empty(chunked));
        REQUIRE_FALSE(lz::has_one(chunked));
        REQUIRE_FALSE(lz::has_many(chunked));
    }

    SUBCASE("One element") {
        std::string s = ";";
        auto chunked = s | lz::chunk_if([](char c) { return c == ';'; });
        REQUIRE_FALSE(lz::empty(chunked));
        REQUIRE_FALSE(lz::has_one(chunked));
        REQUIRE(lz::has_many(chunked));
    }

    SUBCASE("One element that does not satisfy predicate") {
        std::string s = "h";
        auto chunked = lz::chunk_if(s, [](char c) { return c == ';'; });
        REQUIRE_FALSE(lz::empty(chunked));
        REQUIRE(lz::has_one(chunked));
        REQUIRE_FALSE(lz::has_many(chunked));
    }
}

TEST_CASE("chunk_if variations") {
    SUBCASE("Ending and starting with delimiter") {
        std::string s = ";hello world;; this is a message;;; testing;;";
        auto chunked = lz::sv_chunk_if(s, [](const char c) { return c == ';'; });
        auto vec = chunked | lz::to<std::vector<lz::string_view>>();
        std::vector<lz::string_view> expected = { "", "hello world", "", " this is a message", "", "", " testing", "", "" };
        REQUIRE(vec == expected);
    }

    SUBCASE("Ending with two one delimiter") {
        std::string s = "hello world; this is a message;";
        auto chunked = lz::sv_chunk_if(s, [](const char c) { return c == ';'; });
        auto vec = chunked | lz::to<std::vector>();
        std::vector<std::string> expected = { "hello world", " this is a message", "" };
        REQUIRE(lz::equal(vec, expected));
    }
}

TEST_CASE("chunk_if binary operations") {
    std::string s = ";hello world;; this is a message;;; testing;;";
    auto chunked = lz::sv_chunk_if(s, [](const char c) { return c == ';'; });
    static_assert(!std::is_same<decltype(chunked.begin()), decltype(chunked.end())>::value, "Must be sentinel");
    REQUIRE(*chunked.begin() == "");

    SUBCASE("Operator++") {
        std::vector<lz::string_view> expected = { "", "hello world", "", " this is a message", "", "", " testing", "", "" };
        REQUIRE(lz::equal(chunked, expected, [](lz::string_view a, lz::string_view b) { return lz::equal(a, b); }));
    }
}

TEST_CASE("chunk_if to containers") {
    std::string s = "hello world; this is a message;;";
    auto chunked = lz::sv_chunk_if(s, [](const char c) { return c == ';'; });

    SUBCASE("To array") {
        REQUIRE(lz::distance(chunked) == 4);
        auto arr = chunked | lz::to<std::array<lz::string_view, 4>>();
        REQUIRE(arr == decltype(arr){ lz::string_view{ "hello world" }, lz::string_view{ " this is a message" },
                                      lz::string_view{}, lz::string_view{} });
    }

    SUBCASE("To vector") {
        auto vec = chunked | lz::to<std::vector>();
        REQUIRE(vec == decltype(vec){ lz::string_view{ "hello world" }, lz::string_view{ " this is a message" },
                                      lz::string_view{}, lz::string_view{} });
    }

    SUBCASE("To other container") {
        auto list = chunked | lz::to<std::list>();
        REQUIRE(list == decltype(list){ lz::string_view{ "hello world" }, lz::string_view{ " this is a message" },
                                        lz::string_view{}, lz::string_view{} });
    }
}
