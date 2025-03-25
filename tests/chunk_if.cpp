#include <Lz/basic_iterable.hpp>
#include <Lz/c_string.hpp>
#include <Lz/chunk_if.hpp>
#include <Lz/iter_tools.hpp>
#include <catch2/catch.hpp>
#include <iostream>
#include <list>

TEST_CASE("Chunk if custom value type") {
    auto str = lz::c_string(";hello;world;");
#ifdef LZ_HAS_CXX_11
    auto chunked = str | lz::t_chunk_if<std::vector<char>>{}([](char c) { return c == ';'; });
#else
    auto chunked = str | lz::t_chunk_if<std::vector<char>>([](char c) { return c == ';'; });
#endif
    auto it = chunked.begin();
    REQUIRE(*it == std::vector<char>{});
    ++it;
    REQUIRE(*it == std::vector<char>{ 'h', 'e', 'l', 'l', 'o' });
    ++it;
    REQUIRE(*it == std::vector<char>{ 'w', 'o', 'r', 'l', 'd' });
    ++it;
    REQUIRE(*it == std::vector<char>{});
    ++it;
    REQUIRE(it == chunked.end());
}

TEST_CASE("Chunk if with sentinels") {
    auto cstr = lz::c_string("hello world; this is a message;;");
    auto chunked = lz::chunk_if(cstr, [](char c) { return c == ';'; });
    auto it = chunked.begin();
    std::vector<std::string> expected = { "hello world", " this is a message", "", "" };
    for (const auto& str : expected) {
        REQUIRE(lz::equal(*it, str));
        ++it;
    }
    REQUIRE(lz::distance(chunked.begin(), chunked.end()) == 4);
    REQUIRE(it == chunked.end());
}

TEST_CASE("Non string literal test") {
    std::function<bool(int)> is_even = [](int i) {
        return i % 2 == 0;
    };
    std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
    auto chunked = lz::chunk_if(arr, is_even);

    std::vector<int> expected = { 1 };
    auto it = chunked.begin();
    REQUIRE(lz::equal(*it, expected));
    ++it;
    expected = { 3 };
    REQUIRE(lz::equal(*it, expected));
    ++it;
    expected = { 5 };
    REQUIRE(lz::equal(*it, expected));
    ++it;
    REQUIRE(it == chunked.end());

    arr = { 0, 1, 2, 3, 4 };
    chunked = lz::chunk_if(arr, is_even);
    it = chunked.begin();
    expected = {};
    REQUIRE(lz::equal(*it, expected));
    ++it;
    expected = { 1 };
    REQUIRE(lz::equal(*it, expected));
    ++it;
    expected = { 3 };
    REQUIRE(lz::equal(*it, expected));
    ++it;
    expected = {};
    ++it;
    REQUIRE(it == chunked.end());
}

TEST_CASE("Empty or one element chunk_if") {
    SECTION("Empty") {
        std::string s;
        auto chunked = lz::chunk_if(s, [](char c) { return c == ';'; });
        REQUIRE(lz::empty(chunked));
        REQUIRE(!lz::has_one(chunked));
        REQUIRE(!lz::has_many(chunked));
    }

    SECTION("One element") {
        std::string s = ";";
        auto chunked = s | lz::chunk_if([](char c) { return c == ';'; });
        REQUIRE(!lz::empty(chunked));
        REQUIRE(!lz::has_one(chunked));
        REQUIRE(lz::has_many(chunked));
    }

    SECTION("One element that does not satisfy predicate") {
        std::string s = "h";
        auto chunked = lz::chunk_if(s, [](char c) { return c == ';'; });
        REQUIRE(!lz::empty(chunked));
        REQUIRE(lz::has_one(chunked));
        REQUIRE(!lz::has_many(chunked));
    }
}

TEST_CASE("chunk_if changing and creating elements") {
    std::string s = "hello world; this is a message;;";
    auto chunked = s | lz::sv_chunk_if([](const char c) { return c == ';'; });
    static_assert(!std::is_same<decltype(chunked.begin()), decltype(chunked.end())>::value, "Must be sentinel");

    SECTION("Length should be correct") {
        REQUIRE(lz::distance(chunked.begin(), chunked.end()) == 4);
    }

    SECTION("Should be by reference") {
        auto it = chunked.begin();
        REQUIRE(&*it->begin() == &s[0]);
    }

    SECTION("Should chunk correct length") {
        auto it = chunked.begin();
        REQUIRE(*it == "hello world");
        ++it;
        REQUIRE(*it == " this is a message");
    }
}

TEST_CASE("chunk_if variations") {
    SECTION("Ending and starting with delimiter") {
        std::string s = ";hello world;; this is a message;;; testing;;";
        auto chunked = lz::sv_chunk_if(s, [](const char c) { return c == ';'; });
        auto vec = chunked | lz::to<std::vector<lz::string_view>>();
        std::vector<lz::string_view> expected = { "", "hello world", "", " this is a message", "", "", " testing", "", "" };
        REQUIRE(vec == expected);
    }

    SECTION("Ending with delimiters") {
        std::string s = "hello world; this is a message;;";
        auto chunked = s | lz::s_chunk_if([](const char c) { return c == ';'; });
        auto vec = chunked | lz::to<std::vector>();
        std::vector<std::string> expected = { "hello world", " this is a message", "", "" };
        REQUIRE(vec == expected);
    }

    SECTION("Ending with two one delimiter") {
        std::string s = "hello world; this is a message;";
        auto chunked = lz::sv_chunk_if(s, [](const char c) { return c == ';'; });
        auto vec = chunked | lz::to<std::vector>();
        std::vector<std::string> expected = { "hello world", " this is a message", "" };
        REQUIRE(lz::equal(vec, expected));
    }

    SECTION("No delimiters") {
        std::string s = "hello world this is a message";
        auto chunked = lz::s_chunk_if(s, [](const char c) { return c == ';'; });
        auto vec = chunked | lz::to<std::vector>();
        std::vector<std::string> expected = { "hello world this is a message" };
        REQUIRE(lz::equal(vec, expected));
    }
}

TEST_CASE("chunk_if binary operations") {
    std::string s = ";hello world;; this is a message;;; testing;;";
    auto chunked = lz::sv_chunk_if(s, [](const char c) { return c == ';'; });
    static_assert(!std::is_same<decltype(chunked.begin()), decltype(chunked.end())>::value, "Must be sentinel");
    REQUIRE(*chunked.begin() == "");

    SECTION("Operator++") {
        REQUIRE(lz::distance(chunked.begin(), chunked.end()) == 9);
        auto begin = chunked.begin();
        REQUIRE(*begin == "");
        ++begin;
        REQUIRE(*begin == "hello world");
        ++begin;
        REQUIRE(*begin == "");
        ++begin;
        REQUIRE(*begin == " this is a message");
        ++begin;
        REQUIRE(*begin == "");
        ++begin;
        REQUIRE(*begin == "");
        ++begin;
        REQUIRE(*begin == " testing");
        ++begin;
        REQUIRE(*begin == "");
        ++begin;
        REQUIRE(*begin == "");
        ++begin;
        REQUIRE(begin == chunked.end());
    }

    SECTION("Operator== & operator!=") {
        auto begin = chunked.begin();
        REQUIRE(begin != chunked.end());
        while (begin != chunked.end()) {
            ++begin;
        }
        REQUIRE(begin == chunked.end());
    }

    SECTION("Operator--") {
    }
}

TEST_CASE("chunk_if to containers") {
    std::string s = "hello world; this is a message;;";
    auto chunked = lz::sv_chunk_if(s, [](const char c) { return c == ';'; });
    using Iterator = decltype(*chunked.begin());

    SECTION("To array") {
        REQUIRE(lz::distance(chunked.begin(), chunked.end()) == 4);
        auto arr = chunked | lz::to<std::array<lz::string_view, 4>>();
        lz::transform(chunked, arr.begin(),
                      [](const Iterator& it) { return lz::string_view{ it.begin(), it.size() }; });
        REQUIRE(arr == decltype(arr){ lz::string_view{ "hello world" }, lz::string_view{ " this is a message" },
                                      lz::string_view{}, lz::string_view{} });
    }

    SECTION("To vector") {
        auto vec = chunked | lz::to<std::vector>();
        REQUIRE(vec == decltype(vec){ lz::string_view{ "hello world" }, lz::string_view{ " this is a message" },
                                      lz::string_view{}, lz::string_view{} });
    }

    SECTION("To other container") {
        auto list = chunked | lz::to<std::list>();
        REQUIRE(list == decltype(list){ lz::string_view{ "hello world" }, lz::string_view{ " this is a message" },
                                        lz::string_view{}, lz::string_view{} });
    }
}