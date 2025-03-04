#include <Lz/algorithm.hpp>
#include <Lz/c_string.hpp>
#include <Lz/cartesian_product.hpp>
#include <Lz/iter_tools.hpp>
#include <Lz/map.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
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
}

TEST_CASE("Empty or one element cartesian product") {
    SECTION("Empty") {
        std::vector<int> vec;
        auto cart = lz::cartesian_product(vec, vec);
        REQUIRE(lz::empty(cart));
        REQUIRE(!lz::has_one(cart));
        REQUIRE(!lz::has_many(cart));
    }

    SECTION("One element") {
        std::vector<int> vec = { 1 };
        auto cart = lz::cartesian_product(vec, vec);
        REQUIRE(!lz::empty(cart));
        REQUIRE(lz::has_one(cart));
        REQUIRE(!lz::has_many(cart));
    }
}

TEST_CASE("Cartesian product changing and creating elements", "[CartesianProduct][Basic functionality]") {
    std::vector<int> vec = { 1, 2, 3 };
    std::vector<char> chars = { 'a', 'b', 'c' };
    auto cartesian = lz::cartesian_product(vec, chars);
    REQUIRE(cartesian.size() == vec.size() * chars.size());
    REQUIRE(cartesian.size() == static_cast<std::size_t>(cartesian.end() - cartesian.begin()));

    SECTION("Should not be sentinel") {
        static_assert(std::is_same<decltype(cartesian.begin()), decltype(cartesian.end())>::value, "Should be the same");
    }

    SECTION("Distances") {
        auto iter = cartesian.begin();
        REQUIRE(std::distance(iter, iter) == 0);
        REQUIRE(std::distance(iter, iter + 1) == 1);
        REQUIRE(std::distance(iter, iter + 2) == 2);
        REQUIRE(std::distance(iter, iter + 3) == 3);
        REQUIRE(std::distance(iter, iter + 4) == 4);
        REQUIRE(std::distance(iter, iter + 5) == 5);
        REQUIRE(std::distance(iter, iter + 6) == 6);
        REQUIRE(std::distance(iter, iter + 7) == 7);
        REQUIRE(std::distance(iter, iter + 8) == 8);
        REQUIRE(std::distance(iter, iter + 9) == 9);
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

TEST_CASE("Cartesian product binary operations", "[CartesianProduct][Binary ops]") {
    std::vector<int> vec = { 1, 2 };
    std::vector<char> chars = { 'a', 'b', 'c' };
    std::vector<char> chars2 = { 'a', 'b' };
    auto cartesian = lz::cartesian_product(vec, chars, chars2);

    SECTION("Operator++") {
        auto iter = cartesian.begin();
        REQUIRE(*iter == std::make_tuple(1, 'a', 'a'));
        REQUIRE(*(++iter) == std::make_tuple(1, 'a', 'b'));
        REQUIRE(*(++iter) == std::make_tuple(1, 'b', 'a'));
        REQUIRE(*(++iter) == std::make_tuple(1, 'b', 'b'));
        REQUIRE(*(++iter) == std::make_tuple(1, 'c', 'a'));
        REQUIRE(*(++iter) == std::make_tuple(1, 'c', 'b'));
        REQUIRE(*(++iter) == std::make_tuple(2, 'a', 'a'));
        REQUIRE(*(++iter) == std::make_tuple(2, 'a', 'b'));
        REQUIRE(*(++iter) == std::make_tuple(2, 'b', 'a'));
        REQUIRE(*(++iter) == std::make_tuple(2, 'b', 'b'));
        REQUIRE(*(++iter) == std::make_tuple(2, 'c', 'a'));
        REQUIRE(*(++iter) == std::make_tuple(2, 'c', 'b'));
        REQUIRE(++iter == cartesian.end());
    }

    SECTION("Operator--") {
        auto iter = cartesian.end();
        REQUIRE(*(--iter) == std::make_tuple(2, 'c', 'b'));
        REQUIRE(*(--iter) == std::make_tuple(2, 'c', 'a'));
        REQUIRE(*(--iter) == std::make_tuple(2, 'b', 'b'));
        REQUIRE(*(--iter) == std::make_tuple(2, 'b', 'a'));
        REQUIRE(*(--iter) == std::make_tuple(2, 'a', 'b'));
        REQUIRE(*(--iter) == std::make_tuple(2, 'a', 'a'));
        REQUIRE(*(--iter) == std::make_tuple(1, 'c', 'b'));
        REQUIRE(*(--iter) == std::make_tuple(1, 'c', 'a'));
        REQUIRE(*(--iter) == std::make_tuple(1, 'b', 'b'));
        REQUIRE(*(--iter) == std::make_tuple(1, 'b', 'a'));
        REQUIRE(*(--iter) == std::make_tuple(1, 'a', 'b'));
        REQUIRE(*(--iter) == std::make_tuple(1, 'a', 'a'));
        REQUIRE(iter == cartesian.begin());
    }

    SECTION("Operator+(int), tests += as well") {
        auto iter = cartesian.begin();
        REQUIRE(*iter == std::make_tuple(1, 'a', 'a'));
        REQUIRE(*(iter + 1) == std::make_tuple(1, 'a', 'b'));
        REQUIRE(*(iter + 2) == std::make_tuple(1, 'b', 'a'));
        REQUIRE(*(iter + 3) == std::make_tuple(1, 'b', 'b'));
        REQUIRE(*(iter + 4) == std::make_tuple(1, 'c', 'a'));
        REQUIRE(*(iter + 5) == std::make_tuple(1, 'c', 'b'));
        REQUIRE(*(iter + 6) == std::make_tuple(2, 'a', 'a'));
        REQUIRE(*(iter + 7) == std::make_tuple(2, 'a', 'b'));
        REQUIRE(*(iter + 8) == std::make_tuple(2, 'b', 'a'));
        REQUIRE(*(iter + 9) == std::make_tuple(2, 'b', 'b'));
        REQUIRE(*(iter + 10) == std::make_tuple(2, 'c', 'a'));
        REQUIRE(*(iter + 11) == std::make_tuple(2, 'c', 'b'));
        REQUIRE((iter + 12) == cartesian.end());

        iter = cartesian.end();
        REQUIRE(*(iter - 1) == std::make_tuple(2, 'c', 'b'));
        REQUIRE(*(iter - 2) == std::make_tuple(2, 'c', 'a'));
        REQUIRE(*(iter - 3) == std::make_tuple(2, 'b', 'b'));
        REQUIRE(*(iter - 4) == std::make_tuple(2, 'b', 'a'));
        REQUIRE(*(iter - 5) == std::make_tuple(2, 'a', 'b'));
        REQUIRE(*(iter - 6) == std::make_tuple(2, 'a', 'a'));
        REQUIRE(*(iter - 7) == std::make_tuple(1, 'c', 'b'));
        REQUIRE(*(iter - 8) == std::make_tuple(1, 'c', 'a'));
        REQUIRE(*(iter - 9) == std::make_tuple(1, 'b', 'b'));
        REQUIRE(*(iter - 10) == std::make_tuple(1, 'b', 'a'));
        REQUIRE(*(iter - 11) == std::make_tuple(1, 'a', 'b'));
        REQUIRE(*(iter - 12) == std::make_tuple(1, 'a', 'a'));
        REQUIRE((iter - 12) == cartesian.begin());
    }

    SECTION("Distances") {
        auto iter = cartesian.begin();
        REQUIRE(std::distance(iter, iter) == 0);
        REQUIRE(std::distance(iter, iter + 1) == 1);
        REQUIRE(std::distance(iter, iter + 2) == 2);
        REQUIRE(std::distance(iter, iter + 3) == 3);
        REQUIRE(std::distance(iter, iter + 4) == 4);
        REQUIRE(std::distance(iter, iter + 5) == 5);
        REQUIRE(std::distance(iter, iter + 6) == 6);
        REQUIRE(std::distance(iter, iter + 7) == 7);
        REQUIRE(std::distance(iter, iter + 8) == 8);
        REQUIRE(std::distance(iter, iter + 9) == 9);
        REQUIRE(std::distance(iter, iter + 10) == 10);
        REQUIRE(std::distance(iter, iter + 11) == 11);
        REQUIRE(std::distance(iter, iter + 12) == 12);

        REQUIRE(std::distance(iter + 1, iter) == -1);
        REQUIRE(std::distance(iter + 2, iter) == -2);
        REQUIRE(std::distance(iter + 3, iter) == -3);
        REQUIRE(std::distance(iter + 4, iter) == -4);
        REQUIRE(std::distance(iter + 5, iter) == -5);
        REQUIRE(std::distance(iter + 6, iter) == -6);
        REQUIRE(std::distance(iter + 7, iter) == -7);
        REQUIRE(std::distance(iter + 8, iter) == -8);
        REQUIRE(std::distance(iter + 9, iter) == -9);
        REQUIRE(std::distance(iter + 10, iter) == -10);
        REQUIRE(std::distance(iter + 11, iter) == -11);
        REQUIRE(std::distance(iter + 12, iter) == -12);
    }
}

TEST_CASE("CartesianProduct to containers", "[CartesianProduct][To container]") {
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
            std::make_tuple(3, 'b', 'b'), std::make_tuple(3, 'c', 'a'), std::make_tuple(3, 'c', 'b'),
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
        auto actual = cartesian | lz::map([](const auto& elm) {
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
        auto actual = cartesian | lz::map([](const auto& elm) {
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
