#include <Lz/concatenate.hpp>
#include <Lz/filter.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <Lz/zip_longest.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <cpp-lazy-ut-helper/repeat.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <doctest/doctest.h>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

TEST_CASE("Zip longest with sentinels") {
    auto cstr = lz::c_string("Hello");
    auto cstr2 = lz::c_string("Hello1");
    auto cstr3 = lz::c_string("S");
    lz::zip_longest_iterable<decltype(cstr), decltype(cstr2), decltype(cstr3)> longest = lz::zip_longest(cstr, cstr2, cstr3);
    static_assert(!std::is_same<decltype(longest.begin()), decltype(longest.end())>::value, "should be sentinel");
    REQUIRE(lz::distance(longest) == static_cast<std::ptrdiff_t>(std::strlen("Hello1")));

    using tup = std::tuple<lz::optional<char>, lz::optional<char>, lz::optional<char>>;
    std::vector<tup> expected = { std::make_tuple(lz::optional<char>('H'), lz::optional<char>('H'), lz::optional<char>('S')),
                                  std::make_tuple(lz::optional<char>('e'), lz::optional<char>('e'), lz::nullopt),
                                  std::make_tuple(lz::optional<char>('l'), lz::optional<char>('l'), lz::nullopt),
                                  std::make_tuple(lz::optional<char>('l'), lz::optional<char>('l'), lz::nullopt),
                                  std::make_tuple(lz::optional<char>('o'), lz::optional<char>('o'), lz::nullopt),
                                  std::make_tuple(lz::nullopt, lz::optional<char>('1'), lz::nullopt) };

    REQUIRE(lz::equal(longest, expected));

    SUBCASE("Operator=") {
        auto it = longest.begin();
        REQUIRE(it == longest.begin());
        it = longest.end();
        REQUIRE(it == longest.end());
    }
}

TEST_CASE("zip_longest_iterable changing and creating elements") {
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7 };
    const std::list<char> v2 = { 'a', 'b', 'c' };
    std::vector<char> v3 = { 'a', 'b', 'c', 'd' };

    auto bidi = lz::zip_longest(v, v2, v3);
    auto ra = lz::zip_longest(v, v3);
    static_assert(lz::detail::is_bidi<decltype(bidi.begin())>::value, "");
    static_assert(lz::detail::is_ra<decltype(ra.begin())>::value, "");
    REQUIRE(ra.size() == v.size());
    REQUIRE(bidi.size() == v.size());

    SUBCASE("Unequal lengths") {
        REQUIRE(std::distance(bidi.begin(), bidi.end()) == 7);
        REQUIRE(std::distance(ra.begin(), ra.end()) == 7);
        REQUIRE(bidi.size() == 7);
        REQUIRE(ra.size() == 7);
    }

    SUBCASE("Should be by ref") {
        auto begin = ra.begin();
        std::reference_wrapper<int> ref_wrapper = std::get<0>(*begin).value();
        ref_wrapper.get() = 2000;
        REQUIRE(v[0] == ref_wrapper.get());
    }
}

TEST_CASE("Zip longest bidi and not sized") {
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7 };
    auto filter = v | lz::filter([](int i) { return i % 2 == 0; });
    auto zip = lz::zip_longest(v, filter);
    static_assert(lz::detail::is_bidi<decltype(zip.begin())>::value, "Should not be bidi");
    static_assert(!lz::detail::sized<decltype(zip.begin())>::value, "Should not be sized");

    std::tuple<lz::optional<int>, lz::optional<int>> expected[] = { { 1, 2 },           { 2, 4 },           { 3, 6 },
                                                                    { 4, lz::nullopt }, { 5, lz::nullopt }, { 6, lz::nullopt },
                                                                    { 7, lz::nullopt } };
    REQUIRE(lz::equal(zip, expected));
}

TEST_CASE("zip_longest_iterable binary operations") {
    std::vector<int> a = { 1, 2, 3, 4 };
    std::vector<unsigned> b = { 1, 2, 3, 4, 5 };
    std::array<short, 2> c = { 1, 2 };
    auto zipper = a | lz::zip_longest(b, c);

    using tuple = std::tuple<lz::optional<int>, lz::optional<unsigned>, lz::optional<short>>;
    auto expected = std::vector<tuple>{
        { 1, 1, short{ 1 } }, { 2, 2, short{ 2 } }, { 3, 3, lz::nullopt }, { 4, 4, lz::nullopt }, { lz::nullopt, 5, lz::nullopt }
    };

    SUBCASE("Operator++") {
        REQUIRE(lz::equal(zipper, expected));
    }

    SUBCASE("Operator--") {
        REQUIRE(lz::equal(zipper | lz::reverse, expected | lz::reverse));
    }

    SUBCASE("Operator== & Operator!=") {
        auto begin = zipper.begin();
        REQUIRE(begin != zipper.end());
        begin = zipper.end();
        REQUIRE(begin == zipper.end());
    }

    SUBCASE("Operator+") {
        test_procs::test_operator_plus(zipper, expected);
    }

    SUBCASE("Operator-(Iterator)") {
        test_procs::test_operator_minus(zipper);
    }

    SUBCASE("Operator-(default_sentinel)") {
        auto first = lz::repeat(1, 5), second = lz::repeat(1, 4);
        auto l = lz::zip_longest(first, second);
        REQUIRE(lz::size(l) == 5);
        test_procs::test_operator_minus(l);
    }
}

TEST_CASE("Empty and one element zip longest") {
    SUBCASE("Empty") {
        std::vector<int> a;
        auto b = lz::c_string("");
        auto zipper = lz::zip_longest(a, b);
        static_assert(std::is_same<decltype(zipper.end()), lz::default_sentinel_t>::value, "should be default sentinel");
        REQUIRE(lz::empty(zipper));
        REQUIRE_FALSE(lz::has_one(zipper));
        REQUIRE_FALSE(lz::has_many(zipper));
    }

    SUBCASE("One element 1") {
        std::vector<int> a = { 1 };
        std::vector<float> b;
        auto zipper = lz::zip_longest(a, b);
        REQUIRE_FALSE(lz::empty(zipper));
        REQUIRE(lz::has_one(zipper));
        REQUIRE_FALSE(lz::has_many(zipper));
    }

    SUBCASE("One element 2") {
        std::vector<int> a;
        std::vector<float> b = { 1 };
        auto zipper = lz::zip_longest(a, b);
        REQUIRE_FALSE(lz::empty(zipper));
        REQUIRE(lz::has_one(zipper));
        REQUIRE_FALSE(lz::has_many(zipper));
    }
}

TEST_CASE("Zip longest iterable to container") {
    std::array<int, 4> a = { 1, 2, 3, 4 };
    std::list<unsigned> b = { 1, 2, 3, 4, 5 };
    std::vector<char> c = { 'a', 'b', 'c', 'd', 'f', 'g' };
    auto zipper = lz::zip_longest(a, b, c);

    SUBCASE("To array") {
        using optional_tuple = std::tuple<lz::optional<int>, lz::optional<unsigned>, lz::optional<char>>;
        auto to_arr = zipper | lz::to<std::array<optional_tuple, 6>>();
        std::array<optional_tuple, 6> expected = {
            std::make_tuple(1, 1, 'a'), std::make_tuple(2, 2, 'b'),           std::make_tuple(3, 3, 'c'),
            std::make_tuple(4, 4, 'd'), std::make_tuple(lz::nullopt, 5, 'f'), std::make_tuple(lz::nullopt, lz::nullopt, 'g')
        };
        REQUIRE(lz::equal(to_arr, expected));
    }

    SUBCASE("To vector") {
        using optional_tuple = std::tuple<lz::optional<int>, lz::optional<unsigned>, lz::optional<char>>;
        auto to_vec = zipper | lz::to<std::vector<optional_tuple>>();
        std::vector<optional_tuple> expected = {
            std::make_tuple(1, 1, 'a'), std::make_tuple(2, 2, 'b'),           std::make_tuple(3, 3, 'c'),
            std::make_tuple(4, 4, 'd'), std::make_tuple(lz::nullopt, 5, 'f'), std::make_tuple(lz::nullopt, lz::nullopt, 'g')
        };
        REQUIRE(lz::equal(to_vec, expected));
    }

    SUBCASE("To other container using to<>()") {
        using optional_tuple = std::tuple<lz::optional<int>, lz::optional<unsigned>, lz::optional<char>>;
        auto to_list = zipper | lz::to<std::list<optional_tuple>>();
        std::list<optional_tuple> expected = {
            std::make_tuple(1, 1, 'a'), std::make_tuple(2, 2, 'b'),           std::make_tuple(3, 3, 'c'),
            std::make_tuple(4, 4, 'd'), std::make_tuple(lz::nullopt, 5, 'f'), std::make_tuple(lz::nullopt, lz::nullopt, 'g')
        };
        REQUIRE(lz::equal(to_list, expected));
    }

    SUBCASE("To map") {
        using tuple_ref = lz::ref_t<decltype(zipper.begin())>;
        using map_type = std::map<char, lz::optional<unsigned>>;

        auto to_map = zipper |
                      lz::map([](const tuple_ref& tup) { return std::make_pair(std::get<2>(tup).value(), std::get<1>(tup)); }) |
                      lz::to<map_type>();
        map_type expected = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'f', 5 }, { 'g', lz::nullopt } };
        REQUIRE(to_map == expected);
    }

    SUBCASE("To unordered map") {
        using tuple_ref = lz::ref_t<decltype(zipper.begin())>;
        using map_type = std::unordered_map<char, lz::optional<unsigned>>;

        auto to_unordered =
            zipper | lz::map([](const tuple_ref& tup) { return std::make_pair(std::get<2>(tup).value(), std::get<1>(tup)); }) |
            lz::to<map_type>();
        map_type expected = { { 'a', 1 }, { 'b', 2 }, { 'c', 3 }, { 'd', 4 }, { 'f', 5 }, { 'g', lz::nullopt } };
        REQUIRE(to_unordered == expected);
    }
}
