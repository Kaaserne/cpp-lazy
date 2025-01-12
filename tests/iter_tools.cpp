#include <Lz/iter_tools.hpp>
#include <catch2/catch.hpp>

#ifdef LZ_HAS_CXX_11
#include <Lz/common.hpp>
#endif

TEST_CASE("Lines") {
    const lz::string_view expected[] = { "hello world", "this is a message", "testing" };
    auto actual = lz::lines(lz::c_string("hello world\nthis is a message\ntesting"));
    CHECK(actual.distance() == 3);

    auto iterable = lz::zip(actual, expected);
    using value_type = lz::val_iterable_t<decltype(iterable)>;

    iterable.for_each([](const value_type& act_exp) { CHECK(lz::equal(std::get<0>(act_exp), std::get<1>(act_exp))); });
}

TEST_CASE("Unlines") {
    const auto expected = { lz::c_string("hello world"), lz::c_string("\n"), lz::c_string("this is a message"),
                            lz::c_string("\n"), lz::c_string("testing") };
    std::vector<std::string> to_unline = { "hello world", "this is a message", "testing" };
    auto actual = lz::unlines(to_unline);

    for (const auto& act_exp : lz::zip(actual, expected)) {
        CHECK(lz::equal(std::get<0>(act_exp), std::get<1>(act_exp)));
    }
}

TEST_CASE("Reverse") {
    const lz::string_view actual = "hello world";
    const lz::string_view expected = "dlrow olleh";
    CHECK(lz::equal(lz::reverse(actual), expected));
}

TEST_CASE("As") {
    SECTION("Without sentinel") {
        const lz::basic_string_view<char> actual = "hello world";
        const lz::basic_string_view<unsigned char> expected = reinterpret_cast<const unsigned char*>("hello world");
        CHECK(lz::equal(lz::as<unsigned char>(actual), expected));
    }

    SECTION("With sentinel") {
        const auto actual = lz::c_string("hello world");
        const auto expected = lz::c_string(reinterpret_cast<const unsigned char*>("hello world"));
        CHECK(lz::equal(lz::as<unsigned char>(actual), expected));
    }
}

TEST_CASE("Zip with") {
    SECTION("Without sentinel") {
        const std::vector<int> v1 = { 1, 2, 3, 4, 5 };
        const std::vector<int> v2 = { 6, 7, 8, 9, 10 };
        const std::vector<int> v3 = { 11, 12, 13, 14, 15 };
        constexpr int expected[] = { 1 + 6 + 11, 2 + 7 + 12, 3 + 8 + 13, 4 + 9 + 14, 5 + 10 + 15 };
        auto actual = lz::zip_with([](int a, int b, int c) { return a + b + c; }, v1, v2, v3);
        CHECK(lz::equal(actual, expected));
    }

    SECTION("With sentinel") {
        const auto v1 = lz::c_string("hello");
        const auto v2 = lz::c_string("world");
        const auto v3 = lz::c_string("12345");
        constexpr int expected[] = { 'h' + 'w' + '1', 'e' + 'o' + '2', 'l' + 'r' + '3', 'l' + 'l' + '4', 'o' + 'd' + '5' };
        auto actual = lz::zip_with([](int a, int b, int c) { return a + b + c; }, v1, v2, v3);
        CHECK(lz::equal(actual, expected));
    }
}

TEST_CASE("Pairwise") {
    SECTION("With sentinel") {
        const auto actual = lz::c_string("hello");
        const std::vector<std::tuple<char, char>> expected = { std::make_tuple('h', 'e'), std::make_tuple('e', 'l'),
                                                               std::make_tuple('l', 'l'), std::make_tuple('l', 'o') };
        auto actual_pairwise = lz::pairwise(actual);
        CHECK(lz::equal(actual_pairwise, expected));
    }

    SECTION("Without sentinels") {
        const std::vector<int> actual = { 1, 2, 3, 4, 5 };
        const std::vector<std::tuple<int, int>> expected = { std::make_tuple(1, 2), std::make_tuple(2, 3), std::make_tuple(3, 4),
                                                             std::make_tuple(4, 5) };
        auto actual_pairwise = lz::pairwise(actual);
        CHECK(lz::equal(actual_pairwise, expected));
    }

    SECTION("With sentinels, three adjacent elements") {
        const auto actual = lz::c_string("hello");
        const std::vector<std::tuple<char, char, char>> expected = { std::make_tuple('h', 'e', 'l'),
                                                                     std::make_tuple('e', 'l', 'l'),
                                                                     std::make_tuple('l', 'l', 'o') };
        auto actual_pairwise = lz::pairwise_n<3>(actual);
        CHECK(lz::equal(actual_pairwise, expected));
    }
}

TEST_CASE("Keys & values") {
    std::map<int, std::string> m = { { 1, "hello" }, { 2, "world" }, { 3, "!" } };
    const std::vector<int> expectedKeys = { 1, 2, 3 };
    const std::vector<std::string> expectedValues = { "hello", "world", "!" };
    auto keys = lz::keys(m);
    auto values = lz::values(m);
    CHECK(lz::equal(keys, expectedKeys));
    CHECK(lz::equal(values, expectedValues));
}

TEST_CASE("Filtermap") {
    SECTION("With sentinels") {
        const auto actual = lz::c_string("hello world");
        const auto expected = lz::c_string("eoo");
        auto actual_filter_map = lz::filter_map(actual, [](char c) { return c == 'o' || c == 'e'; }, [](char c) { return c; });
        CHECK(lz::equal(actual_filter_map, expected));
    }

    SECTION("Without sentinels") {
        const std::vector<int> actual = { 1, 2, 3, 4, 5 };
        const std::vector<int> expected = { 2, 4 };
        auto actual_filter_map = lz::filter_map(actual, [](int i) { return i % 2 == 0; }, [](int i) { return i; });
        CHECK(lz::equal(actual_filter_map, expected));
    }
}

TEST_CASE("Trim variants") {
    SECTION("Drop back while") {
        const std::vector<int> actual = { 1, 2, 3, 4, 5 };
        const std::vector<int> expected = { 1, 2, 3 };
        auto actual_trim_back = lz::drop_back_while(actual, [](int i) { return i > 3; });
        CHECK(lz::equal(actual_trim_back, expected));
    }

    SECTION("Trim without sentinels") {
        const std::vector<int> actual = { 1, 2, 3, 4, 5 };
        const std::vector<int> expected = { 3, 4 };
        auto actual_trim = lz::trim(actual, [](int i) { return i < 3; }, [](int i) { return i > 4; });
        CHECK(lz::equal(actual_trim, expected));
    }

    SECTION("Trim string without sentinels") {
        const std::string actual = "   hello world   ";
        const std::string expected = "hello world";
        auto actual_trim = lz::trim_string(actual);
        CHECK(lz::equal(actual_trim, expected));
    }
}