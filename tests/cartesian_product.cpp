#include <Lz/algorithm.hpp>
#include <Lz/cartesian_product.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <cpp-lazy-ut-helper/repeat.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <cstddef>
#include <doctest/doctest.h>
#include <list>
#include <map>
#include <vector>
#include <unordered_map>

TEST_CASE("with sentinel") {
    const char* str = "abc";
    const char* str2 = "def";
    auto cstr1 = lz::c_string(str);
    auto cstr2 = lz::c_string(str2);
    auto cart = cstr1 | lz::cartesian_product(cstr2);

    static_assert(!std::is_same<decltype(cart.begin()), decltype(cart.end())>::value, "Should not be the same");
    REQUIRE(static_cast<std::size_t>(lz::distance(cart)) == std::strlen(str) * std::strlen(str2));

    auto expected = { std::make_tuple('a', 'd'), std::make_tuple('a', 'e'), std::make_tuple('a', 'f'),
                      std::make_tuple('b', 'd'), std::make_tuple('b', 'e'), std::make_tuple('b', 'f'),
                      std::make_tuple('c', 'd'), std::make_tuple('c', 'e'), std::make_tuple('c', 'f') };
    REQUIRE(lz::equal(cart, expected));

    SUBCASE("Operator=") {
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
    SUBCASE("Empty") {
        std::vector<int> vec;
        lz::cartesian_product_iterable<std::vector<int>, std::vector<int>> cart = lz::cartesian_product(vec, vec);
        REQUIRE(lz::empty(cart));
        REQUIRE_FALSE(lz::has_one(cart));
        REQUIRE_FALSE(lz::has_many(cart));
        REQUIRE(cart.size() == 0);
    }

    SUBCASE("One element") {
        std::vector<int> vec = { 1 };
        auto cart = lz::cartesian_product(vec, vec);
        REQUIRE_FALSE(lz::empty(cart));
        REQUIRE(lz::has_one(cart));
        REQUIRE_FALSE(lz::has_many(cart));
        REQUIRE(cart.size() == 1);
    }

    SUBCASE("One element and zero elements combined") {
        std::vector<int> vec = { 1 };
        std::vector<int> vec2;
        auto cart = lz::cartesian_product(vec, vec2);
        REQUIRE(lz::empty(cart));
        REQUIRE_FALSE(lz::has_one(cart));
        REQUIRE_FALSE(lz::has_many(cart));
        REQUIRE(cart.size() == 0);

        cart = lz::cartesian_product(vec2, vec);
        REQUIRE(lz::empty(cart));
        REQUIRE_FALSE(lz::has_one(cart));
        REQUIRE_FALSE(lz::has_many(cart));
        REQUIRE(cart.size() == 0);
    }

    SUBCASE("One element and zero elements combined with sentinels") {
        auto cstr = lz::c_string("H");
        auto cstr2 = lz::c_string("");
        auto cart = lz::cartesian_product(cstr, cstr2);
        REQUIRE(lz::empty(cart));
        REQUIRE_FALSE(lz::has_one(cart));
        REQUIRE_FALSE(lz::has_many(cart));

        cart = lz::cartesian_product(cstr2, cstr);
        REQUIRE(lz::empty(cart));
        REQUIRE_FALSE(lz::has_one(cart));
        REQUIRE_FALSE(lz::has_many(cart));
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

    SUBCASE("Operator++") {
        REQUIRE(lz::equal(cartesian, expected));
    }

    SUBCASE("Operator--") {
        REQUIRE(lz::equal(lz::reverse(cartesian), lz::reverse(expected)));
    }

    SUBCASE("Operator+") {
        test_procs::test_operator_plus(cartesian, expected);
    }

    SUBCASE("Operator-") {
        test_procs::test_operator_minus(cartesian);
    }

    SUBCASE("Operator-(default_sentinel_t)") {
        auto repeat1 = lz::repeat(1, 3);
        auto repeat2 = lz::repeat(2, 4);
        auto cartesian2 = repeat1 | lz::cartesian_product(repeat2);
        REQUIRE(lz::size(cartesian2) == 12);
        test_procs::test_operator_minus(cartesian2);
    }
}

TEST_CASE("CartesianProduct to containers") {
    std::vector<int> vec = { 1, 2, 3 };
    std::vector<char> chars = { 'a', 'b', 'c' };
    std::vector<char> chars2 = { 'a', 'b' };
    auto cartesian = lz::cartesian_product(vec, chars, chars2);

    SUBCASE("To array") {
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

    SUBCASE("To vector reversed") {
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

    SUBCASE("To list") {
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

    SUBCASE("To vector") {
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

    SUBCASE("To map") {
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

    SUBCASE("To unordered map") {
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
