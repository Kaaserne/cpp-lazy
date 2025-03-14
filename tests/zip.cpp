#include <Lz/c_string.hpp>
#include <Lz/reverse.hpp>
#include <Lz/map.hpp>
#include <Lz/zip.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Zip with sentinels") {
    auto cstr = lz::c_string("Hello");
    auto cstr2 = lz::c_string("World!!");
    auto zip = lz::zip(cstr, cstr2);
    std::vector<std::tuple<char, char>> expected = { std::make_tuple('H', 'W'), std::make_tuple('e', 'o'),
                                                     std::make_tuple('l', 'r'), std::make_tuple('l', 'l'),
                                                     std::make_tuple('o', 'd') };
    REQUIRE(lz::equal(zip, expected));
    static_assert(!std::is_same<decltype(zip.begin()), decltype(zip.end())>::value, "Should be sentinel-like");
}

TEST_CASE("zip_iterable changing and creating elements") {
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<float> b = { 1.f, 2.f, 3.f, 4.f };
    std::array<short, 4> c = { 1, 2, 3, 4 };

    SECTION("Unequal lengths") {
        std::vector<int> ints = { 1, 2, 3, 4, 5 };
        std::vector<double> floats = { 1.2, 3.3 };

        auto zipper = ints | lz::zip(floats);
        auto end = zipper.end();

        REQUIRE(*--end == std::make_tuple(2, 3.3));
        REQUIRE(*--end == std::make_tuple(1, 1.2));
        REQUIRE(end == zipper.begin());
        REQUIRE(std::distance(zipper.begin(), zipper.end()) == 2);
        REQUIRE(floats.size() == zipper.size());

        std::array<std::tuple<int, Approx>, 2> expected = { std::make_tuple(2, Approx{ 3.3f }),
                                                            std::make_tuple(1, Approx{ 1.2f }) };
        auto reversed = lz::reverse(zipper);
        REQUIRE(lz::equal(reversed, expected));

        auto begin = zipper.begin();
        REQUIRE(*begin == std::make_tuple(1, 1.2));
        REQUIRE(*(begin + 1) == std::make_tuple(2, 3.3));
        REQUIRE(begin + 2 == zipper.end());

        end = zipper.end();
        REQUIRE(*(end - 1) == std::make_tuple(2, 3.3));
        REQUIRE(*(end - 2) == std::make_tuple(1, 1.2));
        REQUIRE(end - 2 == zipper.begin());

        REQUIRE(end - begin == 2);
        REQUIRE((end - 1) - begin == 1);
        REQUIRE((end - 2) - begin == 0);

        REQUIRE(end - (begin + 1) == 1);
        REQUIRE(end - (begin + 2) == 0);

        REQUIRE(begin - end == -2);
        REQUIRE((begin + 1) - end == -1);
        REQUIRE((begin + 2) - end == 0);

        REQUIRE(begin - (end - 1) == -1);
        REQUIRE(begin - (end - 2) == 0);
    }

    SECTION("Should compile") {
        std::vector<int> v = {1, 2, 3, 4, 5};
        std::list<int> v2 = {1, 2, 3};
        auto zipper = lz::zip(v, v2);
        REQUIRE(std::distance(zipper.begin(), zipper.end()) == 3);
    }

    SECTION("Should zip") {
        std::size_t i = 0;
        for (const auto& tup : lz::zip(a, b, c)) {
            REQUIRE(tup == std::make_tuple(a[i], Approx(b[i]), c[i]));
            ++i;
        }
    }

    SECTION("Should stop at smallest container") {
        std::vector<int> smallest = { 1, 2 };
        std::size_t counter = 0;

        for (const auto& tup : lz::zip(a, b, smallest)) {
            (void)tup;
            counter++;
        }

        REQUIRE(counter == smallest.size());
    }

    SECTION("Should be by ref") {
        std::size_t i = 0;
        for (auto tup : lz::zip(a, b, c)) {
            auto& a_element = std::get<0>(tup);
            auto& b_element = std::get<1>(tup);
            auto& c_element = std::get<2>(tup);

            REQUIRE(&a_element == &a[i]);
            REQUIRE(&b_element == &b[i]);
            REQUIRE(&c_element == &c[i]);

            ++i;
        }
    }
}

TEST_CASE("Empty or one element zip") {
    SECTION("Empty") {
        std::vector<int> empty;
        std::vector<int> empty2;
        auto zipper = lz::zip(empty, empty2);
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

    auto zipper = lz::zip(a, b, c);
    auto begin = zipper.begin();

    SECTION("Operator++") {
        REQUIRE(begin == zipper.begin());
        ++begin;
        REQUIRE(*begin == std::make_tuple(*std::next(a.begin()), Approx{ *std::next(b.begin()) }, *std::next(c.begin())));
        REQUIRE(begin == std::next(zipper.begin()));
    }

    SECTION("Operator--") {
        ++begin;
        --begin;
        REQUIRE(*begin == std::make_tuple(*a.begin(), Approx(*b.begin()), *c.begin()));
        REQUIRE(begin == zipper.begin());
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(begin != zipper.end());
        begin = zipper.end();
        REQUIRE(begin == zipper.end());
    }

    SECTION("Operator+(int)") {
        for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(zipper.size()); i++) {
            REQUIRE(*(begin + i) ==
                    std::make_tuple(*std::next(a.begin(), i), *std::next(b.begin(), i), *std::next(c.begin(), i)));
        }

        auto end = zipper.end();
        for (std::ptrdiff_t i = 1; i < static_cast<std::ptrdiff_t>(zipper.size()); i++) {
            REQUIRE(*(end - i) == std::make_tuple(*std::prev(a.end(), i), *std::prev(b.end(), i), *std::prev(c.end(), i)));
        }

        while (begin != end) {
            ++begin;
        }
        for (std::ptrdiff_t i = 1; i < static_cast<std::ptrdiff_t>(zipper.size()); i++) {
            REQUIRE(*(begin - i) == std::make_tuple(*std::prev(a.end(), i), *std::prev(b.end(), i), *std::prev(c.end(), i)));
        }

        begin = zipper.begin();
        while (end != begin) {
            --end;
        }
        for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(zipper.size()); i++) {
            REQUIRE(*(end + i) == std::make_tuple(*std::next(a.begin(), i), *std::next(b.begin(), i), *std::next(c.begin(), i)));
        }
    }

    SECTION("Operator-(Iterator)") {
        REQUIRE((zipper.end() - zipper.begin()) == 4);
        REQUIRE(zipper.begin() - zipper.end() == -4);

        std::array<short, 3> shortest = { 1, 2, 3 };
        auto zip = lz::zip(c, shortest);
        auto beg = zip.begin();
        auto e = zip.end();

        for (std::ptrdiff_t i = zip.size(); i > 0; i--) {
            REQUIRE((e - beg) == i);
            REQUIRE((beg - e) == -i);
            --e;
        }

        e = zip.end();
        for (std::ptrdiff_t i = zip.size(); i > 0; i--) {
            REQUIRE((beg - e) == -i);
            REQUIRE((e - beg) == i);
            ++beg;
        }
    }
}

TEST_CASE("zip_iterable to containers") {
    constexpr std::size_t size = 4;
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<float> b = { 1.f, 2.f, 3.f, 4.f };
    std::array<short, size> c = { 1, 2, 3, 4 };

    SECTION("To array") {
        auto array = lz::zip(a, b, c) | lz::to<std::array<std::tuple<int, float, short>, size>>();

        for (std::size_t i = 0; i < array.size(); i++) {
            auto& a_element = std::get<0>(array[i]);
            auto& b_element = std::get<1>(array[i]);
            auto& c_element = std::get<2>(array[i]);

            REQUIRE(a_element == a[i]);
            REQUIRE(b_element == b[i]);
            REQUIRE(c_element == c[i]);
        }
    }

    SECTION("To vector") {
        auto vector = lz::zip(a, b, c) | lz::to<std::vector>();

        for (std::size_t i = 0; i < vector.size(); i++) {
            auto& a_element = std::get<0>(vector[i]);
            auto& b_element = std::get<1>(vector[i]);
            auto& c_element = std::get<2>(vector[i]);

            REQUIRE(a_element == a[i]);
            REQUIRE(b_element == b[i]);
            REQUIRE(c_element == c[i]);
        }
    }

    SECTION("To other container using to<>()") {
        auto list = lz::zip(a, b, c) | lz::to<std::list<std::tuple<int, float, short>>>();
        auto list_iter = list.begin();

        for (std::size_t i = 0; i < list.size(); i++) {
            auto& a_element = std::get<0>(*list_iter);
            auto& b_element = std::get<1>(*list_iter);
            auto& c_element = std::get<2>(*list_iter);

            REQUIRE(a_element == a[i]);
            REQUIRE(b_element == b[i]);
            REQUIRE(c_element == c[i]);

            ++list_iter;
        }
    }

    SECTION("To map") {
        using Tuple = std::tuple<int, float, short>;
        auto actual = lz::zip(a, b, c) | lz::map([](const Tuple& tup) { return std::make_pair(std::get<0>(tup), tup); }) |
                      lz::to<std::map<int, Tuple>>();
        std::map<int, std::tuple<int, float, short>> expected = {
            std::make_pair(1, std::make_tuple(1, 1.f, static_cast<short>(1))),
            std::make_pair(2, std::make_tuple(2, 2.f, static_cast<short>(2))),
            std::make_pair(3, std::make_tuple(3, 3.f, static_cast<short>(3))),
            std::make_pair(4, std::make_tuple(4, 4.f, static_cast<short>(4)))
        };

        REQUIRE(actual == expected);
    }

    SECTION("To map") {
        using Tuple = std::tuple<int, float, short>;
        auto actual = lz::zip(a, b, c) | lz::map([](const Tuple& tup) { return std::make_pair(std::get<0>(tup), tup); }) |
                      lz::to<std::unordered_map<int, Tuple>>();

        std::unordered_map<int, std::tuple<int, float, short>> expected = {
            std::make_pair(1, std::make_tuple(1, 1.f, static_cast<short>(1))),
            std::make_pair(2, std::make_tuple(2, 2.f, static_cast<short>(2))),
            std::make_pair(3, std::make_tuple(3, 3.f, static_cast<short>(3))),
            std::make_pair(4, std::make_tuple(4, 4.f, static_cast<short>(4)))

        };

        REQUIRE(actual == expected);
    }
}
