#include <Lz/iter_tools.hpp>
#include <Lz/zip.hpp>
#include <c_string/c_string_forward_decl.hpp>
#include <catch2/catch.hpp>
#include <map>

#ifdef LZ_HAS_CXX_11
#include <Lz/common.hpp>
#endif

TEST_CASE("Lines") {
    const lz::string_view expected[] = { "hello world", "this is a message", "testing" };
    lz::lines_iterable_sv<> actual = lz::lines(lz::string_view("hello world\nthis is a message\ntesting"));
    REQUIRE(lz::equal(actual, expected));

    auto actual2 = lz::lines(lz::c_string("hello world\nthis is a message\ntesting"));
    REQUIRE(lz::equal(actual2, expected));
    actual2 = lz::c_string("hello world\nthis is a message\ntesting") | lz::lines;
    REQUIRE(lz::equal(actual2, expected));

    std::string to_split = "hello world\nthis is a message\ntesting";
    lz::lines_iterable<std::string> actual3 = lz::lines(to_split);
    REQUIRE(lz::equal(actual3, expected));
    actual3 = to_split | lz::lines;
    REQUIRE(lz::equal(actual3, expected));
}

TEST_CASE("As") {
    SECTION("Without sentinel") {
        const lz::basic_string_view<char> actual = "hello world";
        const lz::basic_string_view<unsigned char> expected = reinterpret_cast<const unsigned char*>("hello world");
#ifdef LZ_HAS_CXX_11
        REQUIRE(lz::equal(actual | lz::as<unsigned char>{}, expected));
        REQUIRE(lz::equal(lz::as<unsigned char>{}(actual), expected));
#else
        REQUIRE(lz::equal(lz::as<unsigned char>(actual), expected));
        REQUIRE(lz::equal(actual | lz::as<unsigned char>, expected));
#endif

    }

    SECTION("With sentinel") {
        const auto actual = lz::c_string("hello world");
        const auto expected = lz::c_string(reinterpret_cast<const unsigned char*>("hello world"));
#ifdef LZ_HAS_CXX_11
        REQUIRE(lz::equal(lz::as<unsigned char>{}(actual), expected));
        REQUIRE(lz::equal(actual | lz::as<unsigned char>{}, expected));
#else
        REQUIRE(lz::equal(lz::as<unsigned char>(actual), expected));
        REQUIRE(lz::equal(actual | lz::as<unsigned char>, expected));
#endif
    }
}

TEST_CASE("Zip with") {
    SECTION("Without sentinel") {
        const std::vector<int> v1 = { 1, 2, 3, 4, 5 };
        const std::vector<int> v2 = { 6, 7, 8, 9, 10 };
        const std::vector<int> v3 = { 11, 12, 13, 14, 15 };
        constexpr int expected[] = { 1 + 6 + 11, 2 + 7 + 12, 3 + 8 + 13, 4 + 9 + 14, 5 + 10 + 15 };
        using v = const std::vector<int>;
        std::function<int(int, int, int)> fn = [](int a, int b, int c) {
            return a + b + c;
        };
        lz::zip_with_iterable<decltype(fn), v, v, v> actual = lz::zip_with(std::move(fn), v1, v2, v3);
        REQUIRE(lz::equal(actual, expected));
    }

    SECTION("With sentinel") {
        const auto v1 = lz::c_string("hello");
        const auto v2 = lz::c_string("world");
        const auto v3 = lz::c_string("12345");
        constexpr int expected[] = { 'h' + 'w' + '1', 'e' + 'o' + '2', 'l' + 'r' + '3', 'l' + 'l' + '4', 'o' + 'd' + '5' };
        auto actual = lz::zip_with([](int a, int b, int c) { return a + b + c; }, v1, v2, v3);
        REQUIRE(lz::equal(actual, expected));
    }
}

TEST_CASE("Unzip with") {
    const std::vector<std::tuple<int, int>> zipped = { std::make_tuple(1, 6), std::make_tuple(2, 7), std::make_tuple(3, 8),
                                                       std::make_tuple(4, 9), std::make_tuple(5, 10) };
    std::function<int(int, int)> unzip_fn = [](int a, int b) {
        return a + b;
    };
    lz::unzip_with_iterable<decltype(zipped), std::function<int(int, int)>> actual = zipped | lz::unzip_with(std::move(unzip_fn));
    const std::vector<int> expected = { 7, 9, 11, 13, 15 };
    REQUIRE(lz::equal(actual, expected));
}

TEST_CASE("Pairwise") {
    SECTION("With sentinel") {
        const auto actual = lz::c_string("hello");
        const std::vector<std::tuple<char, char>> expected = { std::make_tuple('h', 'e'), std::make_tuple('e', 'l'),
                                                               std::make_tuple('l', 'l'), std::make_tuple('l', 'o') };
        auto actual_pairwise = lz::pairwise(actual);
        REQUIRE(lz::equal(actual_pairwise, expected));
        static_assert(!lz::sized<decltype(actual_pairwise)>::value, "Pairwise should not be sized");
        actual_pairwise = actual | lz::pairwise;
        static_assert(!lz::sized<decltype(actual_pairwise)>::value, "Pairwise should not be sized");
        REQUIRE(lz::equal(actual_pairwise, expected));
    }

    SECTION("Without sentinels") {
        const std::vector<int> actual = { 1, 2, 3, 4, 5 };
        const std::vector<std::tuple<int, int>> expected = { std::make_tuple(1, 2), std::make_tuple(2, 3), std::make_tuple(3, 4),
                                                             std::make_tuple(4, 5) };
        auto actual_pairwise = lz::pairwise(actual);
        REQUIRE(lz::equal(actual_pairwise, expected));
        REQUIRE(lz::size(actual_pairwise) == 4);
        static_assert(lz::sized<decltype(actual_pairwise)>::value, "Pairwise should be sized");
        actual_pairwise = actual | lz::pairwise;
        REQUIRE(lz::equal(actual_pairwise, expected));
        REQUIRE(lz::size(actual_pairwise) == 4);
        static_assert(lz::sized<decltype(actual_pairwise)>::value, "Pairwise should be sized");
    }

    SECTION("With sentinels, three adjacent elements") {
        const auto actual = lz::c_string("hello");
        const std::vector<std::tuple<char, char, char>> expected = { std::make_tuple('h', 'e', 'l'),
                                                                     std::make_tuple('e', 'l', 'l'),
                                                                     std::make_tuple('l', 'l', 'o') };
#ifdef LZ_HAS_CXX_11
        auto actual_pairwise = lz::pairwise_n<3>{}(actual);
        REQUIRE(lz::equal(actual_pairwise, expected));
        actual_pairwise = actual | lz::pairwise_n<3>{};
        REQUIRE(lz::equal(actual_pairwise, expected));
#else
        auto actual_pairwise = lz::pairwise_n<3>(actual);
        REQUIRE(lz::equal(actual_pairwise, expected));
        actual_pairwise = actual | lz::pairwise_n<3>;
        REQUIRE(lz::equal(actual_pairwise, expected));
#endif
    }

    SECTION("Without sentinels, three adjacent elements") {
        const std::vector<int> actual = { 1, 2, 3, 4, 5 };
        const std::vector<std::tuple<int, int, int>> expected = { std::make_tuple(1, 2, 3), std::make_tuple(2, 3, 4),
                                                                  std::make_tuple(3, 4, 5) };
#ifdef LZ_HAS_CXX_11
        auto actual_pairwise = lz::pairwise_n<3>{}(actual);
        REQUIRE(lz::equal(actual_pairwise, expected));
        REQUIRE(lz::size(actual_pairwise) == 3);
        actual_pairwise = actual | lz::pairwise_n<3>{};
        REQUIRE(lz::equal(actual_pairwise, expected));
        REQUIRE(lz::size(actual_pairwise) == 3);
#else
        auto actual_pairwise = lz::pairwise_n<3>(actual);
        REQUIRE(lz::equal(actual_pairwise, expected));
        REQUIRE(lz::size(actual_pairwise) == 3);
        actual_pairwise = actual | lz::pairwise_n<3>;
        REQUIRE(lz::equal(actual_pairwise, expected));
        REQUIRE(lz::size(actual_pairwise) == 3);
#endif
    }
}

TEST_CASE("Keys & values") {
    std::map<int, std::string> m = { { 1, "hello" }, { 2, "world" }, { 3, "!" } };
    const std::vector<int> expected_keys = { 1, 2, 3 };
    const std::vector<std::string> expected_values = { "hello", "world", "!" };

    SECTION("Key & value") {
        lz::keys_iterable<decltype(m)> keys = lz::keys(m);
        lz::values_iterable<decltype(m)> values = lz::values(m);
        REQUIRE(lz::equal(keys, expected_keys));
        REQUIRE(lz::equal(values, expected_values));
        keys = m | lz::keys;
        values = m | lz::values;
        REQUIRE(lz::equal(keys, expected_keys));
        REQUIRE(lz::equal(values, expected_values));
    }

    SECTION("Get nth") {
        const std::vector<std::tuple<int, int, int>> three_tuple_vec = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
        const std::vector<int> expected = { 3, 6, 9 };
#ifdef LZ_HAS_CXX_11
        lz::get_nth_iterable<decltype(three_tuple_vec), 2> actual = lz::get_nth<2>{}(three_tuple_vec);
        REQUIRE(lz::equal(actual, expected));
        actual = three_tuple_vec | lz::get_nth<2>{};
        REQUIRE(lz::equal(actual, expected));
#else
        lz::get_nth_iterable<decltype(three_tuple_vec), 2> actual = lz::get_nth<2>(three_tuple_vec);
        REQUIRE(lz::equal(actual, expected));
        actual = three_tuple_vec | lz::get_nth<2>;
        REQUIRE(lz::equal(actual, expected));
#endif
    }

    SECTION("Get nths") {
        const std::vector<std::tuple<int, int, int>> three_tuple_vec = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
        const std::vector<std::tuple<int, int>> expected = { { 1, 3 }, { 4, 6 }, { 7, 9 } };
#ifdef LZ_HAS_CXX_11
        lz::get_nths_iterable<decltype(three_tuple_vec), 0, 2> actual = lz::get_nths<0, 2>{}(three_tuple_vec);
#else
        lz::get_nths_iterable<decltype(three_tuple_vec), 0, 2> actual = lz::get_nths<0, 2>(three_tuple_vec);
#endif
        REQUIRE(lz::equal(actual, expected));
    }
}

TEST_CASE("Filtermap") {
    SECTION("With sentinels") {
        std::function<bool(char)> filter_fun = [](char c) {
            return c == 'o' || c == 'e';
        };
        std::function<char(char)> map_fun = [](char c) {
            return c;
        };

        const auto actual = lz::c_string("hello world");
        const auto expected = lz::c_string("eoo");
        lz::filter_map_iterable<decltype(actual), decltype(filter_fun), decltype(map_fun)> actual_filter_map =
            lz::filter_map(actual, filter_fun, map_fun);
        REQUIRE(lz::equal(actual_filter_map, expected));
        actual_filter_map = actual | lz::filter_map(std::move(filter_fun), std::move(map_fun));
        REQUIRE(lz::equal(actual_filter_map, expected));
    }

    SECTION("Without sentinels") {
        std::function<bool(int)> filter_fun = [](int i) {
            return i % 2 == 0;
        };
        std::function<int(int)> map_fun = [](int i) {
            return i;
        };

        const std::vector<int> actual = { 1, 2, 3, 4, 5 };
        const std::vector<int> expected = { 2, 4 };
        auto actual_filter_map = lz::filter_map(actual, filter_fun, map_fun);
        REQUIRE(lz::equal(actual_filter_map, expected));
        actual_filter_map = actual | lz::filter_map(std::move(filter_fun), std::move(map_fun));
        REQUIRE(lz::equal(actual_filter_map, expected));
    }
}

TEST_CASE("Select") {
    std::array<int, 6> to_select = { 1, 2, 3, 4, 5, 6 };
    const std::array<bool, to_select.size()> selector = { true, false, true, false, true, false };
    const std::vector<int> expected = { 1, 3, 5 };
    lz::select_iterable<decltype(to_select), decltype(selector)> actual = lz::select(to_select, selector);
    REQUIRE(lz::equal(actual, expected));
    actual = to_select | lz::select(selector);
    REQUIRE(lz::equal(actual, expected));
}

TEST_CASE("Trim variants") {
    SECTION("Drop back while") {
        std::function<bool(int)> pred = [](int i) {
            return i > 3;
        };
        const std::vector<int> actual = { 1, 2, 3, 4, 5 };
        const std::vector<int> expected = { 1, 2, 3 };
        lz::drop_back_iterable<decltype(actual), decltype(pred)> actual_trim_back = lz::drop_back_while(actual, pred);
        REQUIRE(lz::equal(actual_trim_back, expected));
        actual_trim_back = actual | lz::drop_back_while(std::move(pred));
        REQUIRE(lz::equal(actual_trim_back, expected));
    }

    SECTION("Trim vec") {
        const std::vector<int> actual = { 1, 2, 3, 4, 5 };
        const std::vector<int> expected = { 3, 4 };
        std::function<bool(int)> first_pred = [](int i) {
            return i < 3;
        };
        std::function<bool(int)> last_pred = [](int i) {
            return i > 4;
        };
        lz::trim_iterable<decltype(actual), decltype(first_pred), decltype(last_pred)> actual_trim =
            lz::trim(actual, std::move(first_pred), std::move(last_pred));
        REQUIRE(lz::equal(actual_trim, expected));
    }

    SECTION("Trim string") {
        const std::string actual = "   hello world   ";
        const std::string expected = "hello world";
        lz::trim_string_iterable<const std::string> actual_trim = lz::trim(actual);
        REQUIRE(lz::equal(actual_trim, expected));
        actual_trim = actual | lz::trim;
        REQUIRE(lz::equal(actual_trim, expected));

        const lz::string_view actual_view = "   hello world   ";
        const lz::string_view expected_view = "hello world";
        auto actual_trim_view = lz::trim(actual_view);
        REQUIRE(lz::equal(actual_trim_view, expected_view));
        actual_trim_view = actual_view | lz::trim;
        REQUIRE(lz::equal(actual_trim_view, expected_view));
    }
}