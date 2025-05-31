#include <Lz/algorithm.hpp>
#include <Lz/c_string.hpp>
#include <Lz/cartesian_product.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <catch2/catch.hpp>
#include <cstddef>
#include <list>
#include <map>
#include <test_procs.hpp>
#include <unordered_map>

TEST_CASE("Reference and compile test with operator|") {
    std::vector<int> vec1 = { 1 };
    std::vector<int> vec2 = { 1 };
    const std::vector<int> vec3 = { 1 };
    std::vector<int> vec4 = { 1 };
    auto cartesian = lz::cartesian_product(vec1, vec2, vec3, vec4);
    REQUIRE(cartesian.size() == 1);
    REQUIRE(!vec1.empty());
    REQUIRE(!vec2.empty());
    REQUIRE(!vec3.empty());
    REQUIRE(!vec4.empty());
    REQUIRE(&std::get<0>(*cartesian.begin()) == &vec1[0]);
    REQUIRE(&std::get<1>(*cartesian.begin()) == &vec2[0]);
    REQUIRE(&std::get<2>(*cartesian.begin()) == &vec3[0]);
    REQUIRE(&std::get<3>(*cartesian.begin()) == &vec4[0]);

    auto tmp = vec1 | cartesian;
    REQUIRE(&std::get<0>(*tmp.begin()) == &vec1[0]);
    REQUIRE(&std::get<1>(*tmp.begin()) == &vec1[0]);
    REQUIRE(&std::get<2>(*tmp.begin()) == &vec2[0]);
    REQUIRE(&std::get<3>(*tmp.begin()) == &vec3[0]);
    REQUIRE(&std::get<4>(*tmp.begin()) == &vec4[0]);

    cartesian = vec1 | lz::cartesian_product(vec2, vec3, vec4);
    REQUIRE(!vec1.empty());
    REQUIRE(!vec2.empty());
    REQUIRE(!vec3.empty());
    REQUIRE(!vec4.empty());
    REQUIRE(&std::get<0>(*cartesian.begin()) == &vec1[0]);
    REQUIRE(&std::get<1>(*cartesian.begin()) == &vec2[0]);
    REQUIRE(&std::get<2>(*cartesian.begin()) == &vec3[0]);
    REQUIRE(&std::get<3>(*cartesian.begin()) == &vec4[0]);
}

TEST_CASE("Is sentinel") {
    const char* str = "Hello,";
    const char* str2 = " World!";
    auto cstr1 = lz::c_string(str);
    auto cstr2 = lz::c_string(str2);
    auto cart = cstr1 | lz::cartesian_product(cstr2);

    SECTION("Should be sentinel") {
        static_assert(!std::is_same<decltype(cart.begin()), decltype(cart.end())>::value, "Should not be the same");
    }

    SECTION("Should be correct length") {
        REQUIRE(static_cast<std::size_t>(lz::distance(cart.begin(), cart.end())) == std::strlen(str) * std::strlen(str2));
    }

    SECTION("Should make permutation") {
        REQUIRE(*cart.begin() == std::make_tuple('H', ' '));
    }

    SECTION("Operator=") {
        auto begin = cart.begin();
        begin = cart.end();
        REQUIRE(begin == cart.end());
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

TEST_CASE("Cartesian product changing and creating elements") {
    std::vector<int> vec = { 1, 2, 3 };
    std::vector<char> chars = { 'a', 'b', 'c' };
    auto cartesian = lz::cartesian_product(vec, chars);
    REQUIRE(cartesian.size() == vec.size() * chars.size());
    REQUIRE(cartesian.size() == static_cast<std::size_t>(cartesian.end() - cartesian.begin()));

    SECTION("Should not be sentinel") {
        static_assert(std::is_same<decltype(cartesian.begin()), decltype(cartesian.end())>::value, "Should be the same");
    }

    SECTION("Should be by reference") {
        auto& elm_vec = std::get<0>(*cartesian.begin());
        auto& elm_chars = std::get<1>(*cartesian.begin());

        REQUIRE(&elm_vec == &vec[0]);
        REQUIRE(&elm_chars == &chars[0]);
    }

    SECTION("Should make combinations") {
        REQUIRE(*cartesian.begin() == std::make_tuple(1, 'a'));
        REQUIRE(*++cartesian.begin() == std::make_tuple(1, 'b'));
    }

    SECTION("Should be correct length") {
        REQUIRE(std::distance(cartesian.begin(), cartesian.end()) == static_cast<std::ptrdiff_t>(vec.size() * chars.size()));
    }

    SECTION("Different sizes") {
        std::vector<int> vec2 = { 1, 2 };
        std::vector<int> vec3 = { 1, 2, 3, 4 };
        auto cart = lz::cartesian_product(vec, vec2, vec3);
        REQUIRE(cart.size() == vec.size() * vec2.size() * vec3.size());
        REQUIRE(std::distance(cart.begin(), cart.end()) == static_cast<std::ptrdiff_t>(cart.size()));
        auto actual = cart | lz::to<std::vector>();
        std::vector<std::tuple<int, int, int>> expected = {
            std::make_tuple(1, 1, 1), std::make_tuple(1, 1, 2), std::make_tuple(1, 1, 3), std::make_tuple(1, 1, 4),
            std::make_tuple(1, 2, 1), std::make_tuple(1, 2, 2), std::make_tuple(1, 2, 3), std::make_tuple(1, 2, 4),
            std::make_tuple(2, 1, 1), std::make_tuple(2, 1, 2), std::make_tuple(2, 1, 3), std::make_tuple(2, 1, 4),
            std::make_tuple(2, 2, 1), std::make_tuple(2, 2, 2), std::make_tuple(2, 2, 3), std::make_tuple(2, 2, 4),
            std::make_tuple(3, 1, 1), std::make_tuple(3, 1, 2), std::make_tuple(3, 1, 3), std::make_tuple(3, 1, 4),
            std::make_tuple(3, 2, 1), std::make_tuple(3, 2, 2), std::make_tuple(3, 2, 3), std::make_tuple(3, 2, 4),
        };

        REQUIRE(actual == expected);
    }
}

TEST_CASE("Cartesian product binary operations") {
    std::vector<int> vec = { 1, 2 };
    std::vector<char> chars = { 'a', 'b', 'c' };
    std::vector<char> chars2 = { 'a', 'b' };
    auto cartesian = lz::cartesian_product(vec, chars, chars2);
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
