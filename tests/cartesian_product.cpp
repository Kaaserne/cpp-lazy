#include <Lz/algorithm.hpp>
#include <Lz/cartesian_product.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <c_string/c_string_forward_decl.hpp>
#include <catch2/catch.hpp>
#include <cstddef>
#include <list>
#include <map>
#include <test_procs.hpp>
#include <unordered_map>

TEST_CASE("with sentinel") {
    const char* str = "abc";
    const char* str2 = "def";
    auto cstr1 = lz::c_string(str);
    auto cstr2 = lz::c_string(str2);
    auto cart = cstr1 | lz::cartesian_product(cstr2);

    static_assert(!std::is_same<decltype(cart.begin()), decltype(cart.end())>::value, "Should not be the same");
    REQUIRE(static_cast<std::size_t>(lz::distance(cart.begin(), cart.end())) == std::strlen(str) * std::strlen(str2));

    auto expected = { std::make_tuple('a', 'd'), std::make_tuple('a', 'e'), std::make_tuple('a', 'f'),
                      std::make_tuple('b', 'd'), std::make_tuple('b', 'e'), std::make_tuple('b', 'f'),
                      std::make_tuple('c', 'd'), std::make_tuple('c', 'e'), std::make_tuple('c', 'f') };
    REQUIRE(lz::equal(cart, expected));

    SECTION("Operator=") {
        auto it = cart.begin();
        REQUIRE(it == cart.begin());
        REQUIRE(it != cart.end());
        REQUIRE(cart.end() != it);
        REQUIRE(cart.begin() == it);

        it = cart.end();
        REQUIRE(it == cart.end());
        REQUIRE(it != cart.begin());
        REQUIRE(cart.end() == it);
        REQUIRE(cart.begin() != it);
    }
}

TEST_CASE("Empty or one element cartesian product") {
    SECTION("Empty") {
        std::vector<int> vec;
        lz::cartesian_product_iterable<std::vector<int>, std::vector<int>> cart = lz::cartesian_product(vec, vec);
        REQUIRE(lz::empty(cart));
        REQUIRE(!lz::has_one(cart));
        REQUIRE(!lz::has_many(cart));
        REQUIRE(cart.size() == 0);
    }

    SECTION("One element") {
        std::vector<int> vec = { 1 };
        auto cart = lz::cartesian_product(vec, vec);
        REQUIRE(!lz::empty(cart));
        REQUIRE(lz::has_one(cart));
        REQUIRE(!lz::has_many(cart));
        REQUIRE(cart.size() == 1);
    }

    SECTION("One element and zero elements combined") {
        std::vector<int> vec = { 1 };
        std::vector<int> vec2;
        auto cart = lz::cartesian_product(vec, vec2);
        REQUIRE(lz::empty(cart));
        REQUIRE(!lz::has_one(cart));
        REQUIRE(!lz::has_many(cart));
        REQUIRE(cart.size() == 0);

        cart = lz::cartesian_product(vec2, vec);
        REQUIRE(lz::empty(cart));
        REQUIRE(!lz::has_one(cart));
        REQUIRE(!lz::has_many(cart));
        REQUIRE(cart.size() == 0);
    }

    SECTION("One element and zero elements combined with sentinels") {
        auto cstr = lz::c_string("H");
        auto cstr2 = lz::c_string("");
        auto cart = lz::cartesian_product(cstr, cstr2);
        REQUIRE(lz::empty(cart));
        REQUIRE(!lz::has_one(cart));
        REQUIRE(!lz::has_many(cart));

        cart = lz::cartesian_product(cstr2, cstr);
        REQUIRE(lz::empty(cart));
        REQUIRE(!lz::has_one(cart));
        REQUIRE(!lz::has_many(cart));
    }
}

TEST_CASE("Cartesian product binary operations") {
    std::vector<int> vec = { 1, 2 };
    std::vector<char> chars = { 'a', 'b', 'c' };
    std::vector<char> chars2 = { 'a', 'b' };
    auto cartesian = vec | lz::cartesian_product(chars, chars2);
    REQUIRE(cartesian.size() == vec.size() * chars.size() * chars2.size());

    static_assert(std::is_same<decltype(cartesian.begin()), decltype(cartesian.end())>::value, "Should be the same");

    std::vector<std::tuple<int, char, char>> expected = {
        std::make_tuple(1, 'a', 'a'), std::make_tuple(1, 'a', 'b'), std::make_tuple(1, 'b', 'a'), std::make_tuple(1, 'b', 'b'),
        std::make_tuple(1, 'c', 'a'), std::make_tuple(1, 'c', 'b'), std::make_tuple(2, 'a', 'a'), std::make_tuple(2, 'a', 'b'),
        std::make_tuple(2, 'b', 'a'), std::make_tuple(2, 'b', 'b'), std::make_tuple(2, 'c', 'a'), std::make_tuple(2, 'c', 'b')
    };

    SECTION("Operator++") {
        REQUIRE(lz::equal(cartesian, expected));
    }

    SECTION("Operator--") {
        REQUIRE(lz::equal(lz::reverse(cartesian), lz::reverse(expected)));
    }

    SECTION("Operator+") {
        test_procs::test_operator_plus(cartesian, expected);
    }

    SECTION("Operator-") {
        test_procs::test_operator_minus(cartesian);
    }
}

TEST_CASE("CartesianProduct to containers") {
    std::vector<int> vec = { 1, 2, 3 };
    std::vector<char> chars = { 'a', 'b', 'c' };
    std::vector<char> chars2 = { 'a', 'b' };
    auto cartesian = lz::cartesian_product(vec, chars, chars2);

    SECTION("To array") {
        auto actual = cartesian | lz::to<std::array<std::tuple<int, char, char>, 18>>();
        std::array<std::tuple<int, char, char>, 18> expected = {
            std::make_tuple(1, 'a', 'a'), std::make_tuple(1, 'a', 'b'), std::make_tuple(1, 'b', 'a'),
            std::make_tuple(1, 'b', 'b'), std::make_tuple(1, 'c', 'a'), std::make_tuple(1, 'c', 'b'),
            std::make_tuple(2, 'a', 'a'), std::make_tuple(2, 'a', 'b'), std::make_tuple(2, 'b', 'a'),
            std::make_tuple(2, 'b', 'b'), std::make_tuple(2, 'c', 'a'), std::make_tuple(2, 'c', 'b'),
            std::make_tuple(3, 'a', 'a'), std::make_tuple(3, 'a', 'b'), std::make_tuple(3, 'b', 'a'),
            std::make_tuple(3, 'b', 'b'), std::make_tuple(3, 'c', 'a'), std::make_tuple(3, 'c', 'b')
        };
        REQUIRE(actual == expected);
    }

    SECTION("To vector reversed") {
        auto actual = lz::reverse(cartesian) | lz::to<std::vector>();
        std::vector<std::tuple<int, char, char>> expected = {
            std::make_tuple(3, 'c', 'b'), std::make_tuple(3, 'c', 'a'), std::make_tuple(3, 'b', 'b'),
            std::make_tuple(3, 'b', 'a'), std::make_tuple(3, 'a', 'b'), std::make_tuple(3, 'a', 'a'),
            std::make_tuple(2, 'c', 'b'), std::make_tuple(2, 'c', 'a'), std::make_tuple(2, 'b', 'b'),
            std::make_tuple(2, 'b', 'a'), std::make_tuple(2, 'a', 'b'), std::make_tuple(2, 'a', 'a'),
            std::make_tuple(1, 'c', 'b'), std::make_tuple(1, 'c', 'a'), std::make_tuple(1, 'b', 'b'),
            std::make_tuple(1, 'b', 'a'), std::make_tuple(1, 'a', 'b'), std::make_tuple(1, 'a', 'a')
        };
        REQUIRE(actual == expected);
    }

    SECTION("To list") {
        auto actual = cartesian | lz::to<std::list>();
        std::list<std::tuple<int, char, char>> expected = {
            std::make_tuple(1, 'a', 'a'), std::make_tuple(1, 'a', 'b'), std::make_tuple(1, 'b', 'a'),
            std::make_tuple(1, 'b', 'b'), std::make_tuple(1, 'c', 'a'), std::make_tuple(1, 'c', 'b'),
            std::make_tuple(2, 'a', 'a'), std::make_tuple(2, 'a', 'b'), std::make_tuple(2, 'b', 'a'),
            std::make_tuple(2, 'b', 'b'), std::make_tuple(2, 'c', 'a'), std::make_tuple(2, 'c', 'b'),
            std::make_tuple(3, 'a', 'a'), std::make_tuple(3, 'a', 'b'), std::make_tuple(3, 'b', 'a'),
            std::make_tuple(3, 'b', 'b'), std::make_tuple(3, 'c', 'a'), std::make_tuple(3, 'c', 'b'),
        };
        REQUIRE(actual == expected);
    }

    SECTION("To vector") {
        auto actual = cartesian | lz::to<std::vector>();
        std::vector<std::tuple<int, char, char>> expected = {
            std::make_tuple(1, 'a', 'a'), std::make_tuple(1, 'a', 'b'), std::make_tuple(1, 'b', 'a'),
            std::make_tuple(1, 'b', 'b'), std::make_tuple(1, 'c', 'a'), std::make_tuple(1, 'c', 'b'),
            std::make_tuple(2, 'a', 'a'), std::make_tuple(2, 'a', 'b'), std::make_tuple(2, 'b', 'a'),
            std::make_tuple(2, 'b', 'b'), std::make_tuple(2, 'c', 'a'), std::make_tuple(2, 'c', 'b'),
            std::make_tuple(3, 'a', 'a'), std::make_tuple(3, 'a', 'b'), std::make_tuple(3, 'b', 'a'),
            std::make_tuple(3, 'b', 'b'), std::make_tuple(3, 'c', 'a'), std::make_tuple(3, 'c', 'b'),
        };
        REQUIRE(actual == expected);
    }

    SECTION("To map") {
        using elm_type = std::tuple<int, char, char>;
        auto actual = cartesian | lz::map([](const elm_type& elm) {
                          return std::make_pair(std::get<0>(elm), std::make_pair(std::get<1>(elm), std::get<2>(elm)));
                      }) |
                      lz::to<std::map<int, std::pair<char, char>>>();
        std::map<int, std::pair<char, char>> expected = {
            { 1, { 'a', 'a' } }, { 1, { 'a', 'b' } }, { 1, { 'b', 'a' } }, { 1, { 'b', 'b' } }, { 1, { 'c', 'a' } },
            { 1, { 'c', 'b' } }, { 2, { 'a', 'a' } }, { 2, { 'a', 'b' } }, { 2, { 'b', 'a' } }, { 2, { 'b', 'b' } },
            { 2, { 'c', 'a' } }, { 2, { 'c', 'b' } }, { 3, { 'a', 'a' } }, { 3, { 'a', 'b' } }, { 3, { 'b', 'a' } },
            { 3, { 'b', 'b' } }, { 3, { 'c', 'a' } }, { 3, { 'c', 'b' } }
        };
        REQUIRE(actual == expected);
    }

    SECTION("To unordered map") {
        using elm_type = std::tuple<int, char, char>;
        auto actual = cartesian | lz::map([](const elm_type& elm) {
                          return std::make_pair(std::get<0>(elm), std::make_pair(std::get<1>(elm), std::get<2>(elm)));
                      }) |
                      lz::to<std::unordered_map<int, std::pair<char, char>>>();
        std::unordered_map<int, std::pair<char, char>> expected = {
            { 1, { 'a', 'a' } }, { 1, { 'a', 'b' } }, { 1, { 'b', 'a' } }, { 1, { 'b', 'b' } }, { 1, { 'c', 'a' } },
            { 1, { 'c', 'b' } }, { 2, { 'a', 'a' } }, { 2, { 'a', 'b' } }, { 2, { 'b', 'a' } }, { 2, { 'b', 'b' } },
            { 2, { 'c', 'a' } }, { 2, { 'c', 'b' } }, { 3, { 'a', 'a' } }, { 3, { 'a', 'b' } }, { 3, { 'b', 'a' } },
            { 3, { 'b', 'b' } }, { 3, { 'c', 'a' } }, { 3, { 'c', 'b' } }
        };
        REQUIRE(actual == expected);
    }
}
