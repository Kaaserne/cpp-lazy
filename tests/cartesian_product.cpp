#include <Lz/algorithm.hpp>
#include <Lz/c_string.hpp>
#include <Lz/cartesian_product.hpp>
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
    REQUIRE(!vec1.empty());
    REQUIRE(!vec2.empty());
    REQUIRE(!vec3.empty());
    REQUIRE(!vec4.empty());
    CHECK(&std::get<0>(*cartesian.begin()) == &vec1[0]);
    CHECK(&std::get<1>(*cartesian.begin()) == &vec2[0]);
    CHECK(&std::get<2>(*cartesian.begin()) == &vec3[0]);
    CHECK(&std::get<3>(*cartesian.begin()) == &vec4[0]);

    auto tmp = vec1 | cartesian;
    CHECK(&std::get<0>(*tmp.begin()) == &vec1[0]);
    CHECK(&std::get<1>(*tmp.begin()) == &vec1[0]);
    CHECK(&std::get<2>(*tmp.begin()) == &vec2[0]);
    CHECK(&std::get<3>(*tmp.begin()) == &vec3[0]);
    CHECK(&std::get<4>(*tmp.begin()) == &vec4[0]);

    cartesian = vec1 | lz::cartesian_product(vec2, vec3, vec4);
    REQUIRE(!vec1.empty());
    REQUIRE(!vec2.empty());
    REQUIRE(!vec3.empty());
    REQUIRE(!vec4.empty());
    CHECK(&std::get<0>(*cartesian.begin()) == &vec1[0]);
    CHECK(&std::get<1>(*cartesian.begin()) == &vec2[0]);
    CHECK(&std::get<2>(*cartesian.begin()) == &vec3[0]);
    CHECK(&std::get<3>(*cartesian.begin()) == &vec4[0]);
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
}

TEST_CASE("Cartesian product binary operations", "[CartesianProduct][Binary ops]") {
    std::vector<int> vec = { 1, 2, 3 };
    std::vector<char> chars = { 'a', 'b', 'c' };
    auto cartesian = lz::cartesian_product(vec, chars);

    SECTION("Operator++") {
        auto begin = cartesian.begin();
        ++begin;
        REQUIRE(*begin == std::make_tuple(1, 'b'));
        ++begin, ++begin;
        REQUIRE(*begin == std::make_tuple(2, 'a'));
    }

    SECTION("Operator--") {
        std::vector<int> tmp_vec = { 1, 2, 3 };
        std::vector<char> tmp_chars = { 'a', 'b', 'c' };
        auto cart = lz::cartesian_product(tmp_vec, tmp_chars);
        auto end = cart.end();

        --end;
        REQUIRE(*end == std::make_tuple(3, 'c'));

        --end;
        REQUIRE(*end == std::make_tuple(3, 'b'));

        --end;
        REQUIRE(*end == std::make_tuple(3, 'a'));

        --end;
        REQUIRE(*end == std::make_tuple(2, 'c'));

        --end /*2, 'b'*/, --end /*2, 'a'*/, --end /*1, 'c'*/, --end, /*1, 'b'*/ --end /*1, 'a'*/;
        REQUIRE(end == cart.begin());

        auto beg = cart.begin();
        ++beg;
        --beg;
        REQUIRE(*beg == std::make_tuple(1, 'a'));
    }

    SECTION("Operator== & operator!=") {
        auto it = cartesian.begin();
        REQUIRE(it != cartesian.end());
        it = cartesian.end();
        REQUIRE(it == cartesian.end());
    }

    SECTION("Operator+(int), tests += as well") {
        auto iter = cartesian.begin();
        REQUIRE(*(iter + 1) == std::make_tuple(1, 'b'));
        REQUIRE(*(iter + 2) == std::make_tuple(1, 'c'));
        REQUIRE(*(iter + 3) == std::make_tuple(2, 'a'));
        REQUIRE(*(iter + 6) == std::make_tuple(3, 'a'));
        REQUIRE(*(iter + 8) == std::make_tuple(3, 'c'));
        REQUIRE(iter + 9 == cartesian.end());
    }

    SECTION("Operator-(int), tests -= as well") {
        auto iter = cartesian.begin();
        iter += 8;
        REQUIRE(*(iter - 3) == std::make_tuple(2, 'c'));
        REQUIRE(*(iter - 2) == std::make_tuple(3, 'a'));
        REQUIRE(*(iter - 1) == std::make_tuple(3, 'b'));
        REQUIRE(*(iter - 8) == std::make_tuple(1, 'a'));
        REQUIRE(iter - 8 == cartesian.begin());
    }

    SECTION("Operator<, <, <=, >, >=") {
        auto b = cartesian.begin();
        auto end = cartesian.end();
        auto distance = std::distance(b, end);

        REQUIRE(b < end);
        REQUIRE(b + distance - 1 > end - distance);
        REQUIRE(b + distance - 1 <= end);
        REQUIRE(b + 9 - 1 >= end - 1);
    }
}

TEST_CASE("CartesianProduct to containers", "[CartesianProduct][To container]") {
    std::vector<int> vec = { 1, 2, 3 };
    std::vector<char> chars = { 'a', 'b', 'c' };
    auto cartesian = lz::cartesian_product(vec, chars);

    SECTION("To array") {
        constexpr std::size_t size = 9;
        auto result = cartesian | lz::to<std::array<std::tuple<int, char>, size>>();

        std::array<std::tuple<int, char>, size> expected = {
            std::make_tuple(1, 'a'), std::make_tuple(1, 'b'), std::make_tuple(1, 'c'),

            std::make_tuple(2, 'a'), std::make_tuple(2, 'b'), std::make_tuple(2, 'c'),

            std::make_tuple(3, 'a'), std::make_tuple(3, 'b'), std::make_tuple(3, 'c'),
        };
        REQUIRE(result == expected);
    }

    SECTION("To vector") {
        std::vector<std::tuple<int, char>> result = cartesian | lz::to<std::vector>();
        std::vector<std::tuple<int, char>> expected = {
            std::make_tuple(1, 'a'), std::make_tuple(1, 'b'), std::make_tuple(1, 'c'),

            std::make_tuple(2, 'a'), std::make_tuple(2, 'b'), std::make_tuple(2, 'c'),

            std::make_tuple(3, 'a'), std::make_tuple(3, 'b'), std::make_tuple(3, 'c'),
        };
        REQUIRE(result == expected);
    }

    SECTION("To other container using to<>()") {
        auto result = cartesian | lz::to<std::list<std::tuple<int, char>>>();
        std::list<std::tuple<int, char>> expected = {
            std::make_tuple(1, 'a'), std::make_tuple(1, 'b'), std::make_tuple(1, 'c'),

            std::make_tuple(2, 'a'), std::make_tuple(2, 'b'), std::make_tuple(2, 'c'),

            std::make_tuple(3, 'a'), std::make_tuple(3, 'b'), std::make_tuple(3, 'c'),
        };
        REQUIRE(result == expected);
    }

    // TODO add to map

    SECTION("To map") {
        auto result = cartesian | lz::map([](lz::ref_iterable_t<decltype(cartesian)> v) {
                          return std::make_pair(std::get<0>(v) + std::get<1>(v), v);
                      }) |
                      lz::to<std::map<int, std::tuple<int, char>>>();
        decltype(result) expected = {
            { 1 + 'a', std::make_tuple(1, 'a') }, { 1 + 'b', std::make_tuple(1, 'b') }, { 1 + 'c', std::make_tuple(1, 'c') },

            { 2 + 'a', std::make_tuple(2, 'a') }, { 2 + 'b', std::make_tuple(2, 'b') }, { 2 + 'c', std::make_tuple(2, 'c') },

            { 3 + 'a', std::make_tuple(3, 'a') }, { 3 + 'b', std::make_tuple(3, 'b') }, { 3 + 'c', std::make_tuple(3, 'c') }
        };
        REQUIRE(result == expected);
    }

    SECTION("To unordered map") {
        auto result = cartesian | lz::map([](const typename decltype(cartesian)::iterator::value_type& v) {
                          return std::make_pair(std::get<0>(v) + std::get<1>(v), v);
                      }) |
                      lz::to<std::unordered_map<int, std::tuple<int, char>>>();
        decltype(result) expected = {
            { 1 + 'a', std::make_tuple(1, 'a') }, { 1 + 'b', std::make_tuple(1, 'b') }, { 1 + 'c', std::make_tuple(1, 'c') },

            { 2 + 'a', std::make_tuple(2, 'a') }, { 2 + 'b', std::make_tuple(2, 'b') }, { 2 + 'c', std::make_tuple(2, 'c') },

            { 3 + 'a', std::make_tuple(3, 'a') }, { 3 + 'b', std::make_tuple(3, 'b') }, { 3 + 'c', std::make_tuple(3, 'c') }
        };
        REQUIRE(result == expected);
    }
}