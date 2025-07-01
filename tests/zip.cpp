#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <Lz/zip.hpp>
#include <c_string/c_string_forward_decl.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <test_procs.hpp>
#include <unordered_map>

TEST_CASE("Zip with sentinels") {
    auto cstr = lz::c_string("Hello");
    auto cstr2 = lz::c_string("World!!");
    lz::zip_iterable<decltype(cstr), decltype(cstr2)> zip = lz::zip(cstr, cstr2);
    std::vector<std::tuple<char, char>> expected = { std::make_tuple('H', 'W'), std::make_tuple('e', 'o'),
                                                     std::make_tuple('l', 'r'), std::make_tuple('l', 'l'),
                                                     std::make_tuple('o', 'd') };
    REQUIRE(lz::equal(zip, expected));
    static_assert(!std::is_same<decltype(zip.begin()), decltype(zip.end())>::value, "Should be sentinel-like");

    SECTION("Operator=") {
        auto it = zip.begin();
        REQUIRE(it == zip.begin());
        using sentinel = decltype(zip.end());
        it = sentinel{ cstr.end(), cstr2.end() };
        REQUIRE(it == zip.end());

        std::vector<int> ints = { 1, 2, 3, 4 };
        auto zipper = lz::zip(cstr, ints);
        using sentinel2 = decltype(zipper.end());
        auto it2 = zipper.begin();
        REQUIRE(it2 == zipper.begin());
        it2 = sentinel2{ cstr.end(), ints.end() };
        REQUIRE(it2 == zipper.end());
    }
}

TEST_CASE("zip_iterable changing and creating elements") {
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<float> b = { 1.f, 2.f, 3.f, 4.f };

    SECTION("Unequal lengths") {
        std::vector<int> ints = { 1, 2, 3, 4, 5 };
        std::vector<double> floats = { 1.2, 3.3 };

        auto zipper = ints | lz::zip(floats);
        static_assert(std::is_same<typename decltype(zipper.begin())::reference, std::tuple<int&, double&>>::value,
                      "should be tuple ref");
        std::array<std::tuple<int, Approx>, 2> expected = { std::make_tuple(2, Approx{ 3.3f }),
                                                            std::make_tuple(1, Approx{ 1.2f }) };
        REQUIRE(lz::equal(lz::reverse(zipper), expected));
        REQUIRE(lz::equal(zipper, expected | lz::reverse));
    }
}

TEST_CASE("Empty or one element zip") {
    SECTION("Empty") {
        std::vector<int> empty;
        auto empty2 = lz::c_string("");
        auto zipper = lz::zip(empty, empty2);
        static_assert(std::is_same<decltype(zipper.end()), std::tuple<decltype(empty)::iterator, lz::default_sentinel>>::value,
                      "should be sentinel like");
        REQUIRE(lz::empty(zipper));
        REQUIRE(!lz::has_many(zipper));
        REQUIRE(!lz::has_one(zipper));
    }

    SECTION("One element 1") {
        std::vector<int> one = { 1 };
        std::vector<int> empty;
        auto zipper = lz::zip(one, empty);
        REQUIRE(lz::empty(zipper));
        REQUIRE(!lz::has_many(zipper));
        REQUIRE(!lz::has_one(zipper));
    }

    SECTION("One element 2") {
        std::vector<int> empty;
        std::vector<int> one = { 1 };
        auto zipper = lz::zip(empty, one);
        REQUIRE(lz::empty(zipper));
        REQUIRE(!lz::has_many(zipper));
        REQUIRE(!lz::has_one(zipper));
    }

    SECTION("One element 3") {
        std::vector<int> one = { 1 };
        std::vector<int> one2 = { 1 };
        auto zipper = lz::zip(one, one2);
        REQUIRE(!lz::empty(zipper));
        REQUIRE(!lz::has_many(zipper));
        REQUIRE(lz::has_one(zipper));
    }
}

TEST_CASE("zip_iterable binary operations") {
    constexpr std::size_t size = 4;
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<float> b = { 1.f, 2.f, 3.f, 4.f };
    std::array<short, size> c = { 1, 2, 3, 4 };

    std::vector<std::tuple<int, Approx, short>> expected = { std::make_tuple(1, Approx{ 1.f }, short(1)),
                                                             std::make_tuple(2, Approx{ 2.f }, short(2)),
                                                             std::make_tuple(3, Approx{ 3.f }, short(3)),
                                                             std::make_tuple(4, Approx{ 4.f }, short(4)) };

    auto zipper = lz::zip(a, b, c);
    auto begin = zipper.begin();

    SECTION("Operator++") {
        REQUIRE(lz::equal(zipper, expected));
    }

    SECTION("Operator--") {
        REQUIRE(lz::equal(zipper | lz::reverse, expected | lz::reverse));
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(begin != zipper.end());
        REQUIRE(zipper.end() != begin);
        begin = zipper.end();
        REQUIRE(begin == zipper.end());
        REQUIRE(zipper.end() == begin);
    }

    SECTION("Operator+(int)") {
        test_procs::test_operator_plus(zipper, expected);
    }

    SECTION("Operator-(Iterator)") {
        test_procs::test_operator_minus(zipper);
    }
}

TEST_CASE("zip_iterable to containers") {
    constexpr std::size_t size = 4;
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<float> b = { 1.f, 2.f, 3.f, 4.f };
    std::array<short, size> c = { 1, 2, 3, 4 };

    using tup = std::tuple<int, float, short>;

    SECTION("To array") {
        auto array = lz::zip(a, b, c) | lz::to<std::array<tup, size>>();
        std::array<tup, size> expected = { std::make_tuple(1, 1.f, static_cast<short>(1)),
                                           std::make_tuple(2, 2.f, static_cast<short>(2)),
                                           std::make_tuple(3, 3.f, static_cast<short>(3)),
                                           std::make_tuple(4, 4.f, static_cast<short>(4)) };
        REQUIRE(array == expected);
    }

    SECTION("To vector") {
        auto vector = lz::zip(a, b, c) | lz::to<std::vector>();
        std::vector<tup> expected = { std::make_tuple(1, 1.f, static_cast<short>(1)),
                                      std::make_tuple(2, 2.f, static_cast<short>(2)),
                                      std::make_tuple(3, 3.f, static_cast<short>(3)),
                                      std::make_tuple(4, 4.f, static_cast<short>(4)) };
        REQUIRE(vector == expected);
    }

    SECTION("To other container using to<>()") {
        auto list = lz::zip(a, b, c) | lz::to<std::list<tup>>();
        std::list<tup> expected = { std::make_tuple(1, 1.f, static_cast<short>(1)),
                                    std::make_tuple(2, 2.f, static_cast<short>(2)),
                                    std::make_tuple(3, 3.f, static_cast<short>(3)),
                                    std::make_tuple(4, 4.f, static_cast<short>(4)) };
        REQUIRE(list == expected);
    }

    SECTION("To map") {
        auto actual = lz::zip(a, b, c) | lz::map([](const tup& t) { return std::make_pair(std::get<0>(t), t); }) |
                      lz::to<std::map<int, tup>>();

        std::map<int, tup> expected = { std::make_pair(1, std::make_tuple(1, 1.f, static_cast<short>(1))),
                                        std::make_pair(2, std::make_tuple(2, 2.f, static_cast<short>(2))),
                                        std::make_pair(3, std::make_tuple(3, 3.f, static_cast<short>(3))),
                                        std::make_pair(4, std::make_tuple(4, 4.f, static_cast<short>(4))) };

        REQUIRE(actual == expected);
    }

    SECTION("To map") {
        auto actual = lz::zip(a, b, c) | lz::map([](const tup& t) { return std::make_pair(std::get<0>(t), t); }) |
                      lz::to<std::unordered_map<int, tup>>();

        std::unordered_map<int, tup> expected = { std::make_pair(1, std::make_tuple(1, 1.f, static_cast<short>(1))),
                                                  std::make_pair(2, std::make_tuple(2, 2.f, static_cast<short>(2))),
                                                  std::make_pair(3, std::make_tuple(3, 3.f, static_cast<short>(3))),
                                                  std::make_pair(4, std::make_tuple(4, 4.f, static_cast<short>(4)))

        };

        REQUIRE(actual == expected);
    }
}
