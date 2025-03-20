#include <Lz/c_string.hpp>
#include <Lz/concatenate.hpp>
#include <Lz/filter.hpp>
#include <Lz/map.hpp>
#include <Lz/zip_longest.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Zip longest with sentinels") {
    auto cstr = lz::c_string("Hello");
    auto cstr2 = lz::c_string("Hello1");
    auto cstr3 = lz::c_string("S");
    auto longest = lz::zip_longest(cstr, cstr2, cstr3);
    static_assert(!std::is_same<decltype(longest.begin()), decltype(longest.end())>::value, "should be sentinel");
    REQUIRE(lz::distance(longest.begin(), longest.end()) == static_cast<std::ptrdiff_t>(std::strlen("Hello1")));

    auto begin = longest.begin();
    lz::optional<std::reference_wrapper<const char>> ref1(std::get<0>(*begin));
    lz::optional<std::reference_wrapper<const char>> ref2(std::get<1>(*begin));
    lz::optional<std::reference_wrapper<const char>> ref3(std::get<2>(*begin));

    REQUIRE(ref1.value() == 'H');
    REQUIRE(ref2.value() == 'H');
    REQUIRE(ref3.value() == 'S');
    ++begin;

    ref1 = std::get<0>(*begin);
    ref2 = std::get<1>(*begin);
    ref3 = std::get<2>(*begin);

    REQUIRE(ref1.value() == 'e');
    REQUIRE(ref2.value() == 'e');
    REQUIRE(!ref3);

    ++begin;
    ref1 = std::get<0>(*begin);
    ref2 = std::get<1>(*begin);
    ref3 = std::get<2>(*begin);

    REQUIRE(ref1.value() == 'l');
    REQUIRE(ref2.value() == 'l');
    REQUIRE(!ref3);

    ++begin;
    ref1 = std::get<0>(*begin);
    ref2 = std::get<1>(*begin);
    ref3 = std::get<2>(*begin);

    REQUIRE(ref1.value() == 'l');
    REQUIRE(ref2.value() == 'l');
    REQUIRE(!ref3);

    ++begin;
    ref1 = std::get<0>(*begin);
    ref2 = std::get<1>(*begin);
    ref3 = std::get<2>(*begin);

    REQUIRE(ref1.value() == 'o');
    REQUIRE(ref2.value() == 'o');
    REQUIRE(!ref3);

    ++begin;
    ref1 = std::get<0>(*begin);
    ref2 = std::get<1>(*begin);
    ref3 = std::get<2>(*begin);

    REQUIRE(!ref1);
    REQUIRE(ref2.value() == '1');
    REQUIRE(!ref3);

    ++begin;
    ref1 = std::get<0>(*begin);
    ref2 = std::get<1>(*begin);
    ref3 = std::get<2>(*begin);

    REQUIRE(!ref1);
    REQUIRE(!ref2);
    REQUIRE(!ref3);
}

TEST_CASE("zip_longest_iterable changing and creating elements") {
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7 };
    std::list<char> v2 = { 'a', 'b', 'c' };
    std::vector<char> v3 = { 'a', 'b', 'c', 'd' };

    auto bidi = lz::zip_longest(v, v2, v3);
    auto ra = lz::zip_longest(v, v3);
    static_assert(lz::detail::is_fwd<decltype(bidi.begin())>::value, "");
    static_assert(lz::detail::is_ra<decltype(ra.begin())>::value, "");
    REQUIRE(ra.size() == v.size());
    REQUIRE(bidi.size() == v.size());

    SECTION("Unequal lengths") {
        REQUIRE(std::distance(bidi.begin(), bidi.end()) == 7);
        REQUIRE(std::distance(ra.begin(), ra.end()) == 7);
    }

    SECTION("Should zip longest") {
        std::size_t counter = 0;
        for (auto&& tup : bidi) {
            auto&& a = std::get<0>(tup);
            auto&& b = std::get<1>(tup);
            auto&& c = std::get<2>(tup);

            REQUIRE(*a == v[counter]);

            if (counter > 2) {
                REQUIRE(!b);
            }
            else {
                REQUIRE(b);
                REQUIRE(*b == *std::next(v2.begin(), static_cast<std::ptrdiff_t>(counter)));
            }
            if (counter > 3) {
                REQUIRE(!c);
            }
            else {
                REQUIRE(c);
                REQUIRE(*c == v3[counter]);
            }
            counter++;
        }

        counter = 0;

        for (auto&& tup : ra) {
            auto&& a = std::get<0>(tup);
            auto&& b = std::get<1>(tup);
            REQUIRE(*a == v[counter]);
            if (counter > 3) {
                REQUIRE(!b);
            }
            else {
                REQUIRE(b);
                REQUIRE(*b == v3[counter]);
            }
            counter++;
        }
    }

    SECTION("Should be by ref") {
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
    std::vector<float> b = { 1.f, 2.f, 3.f, 4.f, 5.f };
    std::array<short, 2> c = { 1, 2 };

    auto zipper = a | lz::zip_longest(b, c);
    auto begin = zipper.begin();

    REQUIRE(*std::get<0>(*begin) == 1);
    REQUIRE(*std::get<1>(*begin) == 1.f);
    REQUIRE(*std::get<2>(*begin) == 1);

    SECTION("Operator++") {
        ++begin;
        REQUIRE(*std::get<0>(*begin) == 2);
        REQUIRE(*std::get<1>(*begin) == 2.f);
        REQUIRE(*std::get<2>(*begin) == 2);
        ++begin;
        REQUIRE(*std::get<0>(*begin) == 3);
        REQUIRE(*std::get<1>(*begin) == 3.f);
        REQUIRE(!std::get<2>(*begin));
    }

    SECTION("Operator--") {
        auto end = zipper.end();
        --end;
        REQUIRE(!std::get<0>(*end).has_value());
        REQUIRE(std::get<1>(*end).value() == 5.f);
        REQUIRE(!std::get<2>(*end).has_value());
        --end;
        REQUIRE(std::get<0>(*end).value() == 4);
        REQUIRE(std::get<1>(*end).value() == 4.f);
        REQUIRE(!std::get<2>(*end).has_value());
        --end;
        REQUIRE(std::get<0>(*end).value() == 3);
        REQUIRE(std::get<1>(*end).value() == 3.f);
        REQUIRE(!std::get<2>(*end).has_value());
        --end;
        REQUIRE(std::get<0>(*end).value() == 2);
        REQUIRE(std::get<1>(*end).value() == 2.f);
        REQUIRE(std::get<2>(*end).value() == 2);
        --end;
        REQUIRE(std::get<0>(*end).value() == 1);
        REQUIRE(std::get<1>(*end).value() == 1.f);
        REQUIRE(std::get<2>(*end).value() == 1);
        REQUIRE(end == zipper.begin());
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(begin != zipper.end());
        begin = zipper.end();
        REQUIRE(begin == zipper.end());
    }

    SECTION("Operator+(int) offset, tests += as well") {
        REQUIRE(std::get<0>(*(begin + 0)).value() == 1);
        REQUIRE(std::get<1>(*(begin + 0)).value() == 1.f);
        REQUIRE(std::get<2>(*(begin + 0)).value() == 1);

        REQUIRE(std::get<0>(*(begin + 1)).value() == 2);
        REQUIRE(std::get<1>(*(begin + 1)).value() == 2.f);
        REQUIRE(std::get<2>(*(begin + 1)).value() == 2);

        REQUIRE(std::get<0>(*(begin + 2)).value() == 3);
        REQUIRE(std::get<1>(*(begin + 2)).value() == 3.f);
        REQUIRE(!std::get<2>(*(begin + 2)).has_value());

        REQUIRE(std::get<0>(*(begin + 3)).value() == 4);
        REQUIRE(std::get<1>(*(begin + 3)).value() == 4.f);
        REQUIRE(!std::get<2>(*(begin + 3)).has_value());

        REQUIRE(!std::get<0>(*(begin + 4)).has_value());
        REQUIRE(std::get<1>(*(begin + 4)).value() == 5.f);
        REQUIRE(!std::get<2>(*(begin + 4)).has_value());
    }

    SECTION("Operator-(int) offset, tests -= as well") {
        auto end = zipper.end();
        REQUIRE(!std::get<0>(*(end - 1)).has_value());
        REQUIRE(std::get<1>(*(end - 1)).value() == 5.f);
        REQUIRE(!std::get<2>(*(end - 1)).has_value());

        REQUIRE(std::get<0>(*(end - 2)).value() == 4);
        REQUIRE(std::get<1>(*(end - 2)).value() == 4.f);
        REQUIRE(!std::get<2>(*(end - 2)).has_value());

        REQUIRE(std::get<0>(*(end - 3)).value() == 3);
        REQUIRE(std::get<1>(*(end - 3)).value() == 3.f);
        REQUIRE(!std::get<2>(*(end - 3)).has_value());

        REQUIRE(std::get<0>(*(end - 4)).value() == 2);
        REQUIRE(std::get<1>(*(end - 4)).value() == 2.f);
        REQUIRE(std::get<2>(*(end - 4)).value() == 2);

        REQUIRE(std::get<0>(*(end - 5)).value() == 1);
        REQUIRE(std::get<1>(*(end - 5)).value() == 1.f);
        REQUIRE(std::get<2>(*(end - 5)).value() == 1);
    }

    SECTION("Operator-(Iterator)") {
        REQUIRE(zipper.end() - begin == static_cast<std::ptrdiff_t>(b.size()));
        ++begin;
        REQUIRE(zipper.end() - begin == static_cast<std::ptrdiff_t>(b.size() - 1));
        ++begin;
        REQUIRE(zipper.end() - begin == static_cast<std::ptrdiff_t>(b.size() - 2));
        ++begin;
        REQUIRE(zipper.end() - begin == static_cast<std::ptrdiff_t>(b.size() - 3));
        ++begin;
        REQUIRE(zipper.end() - begin == static_cast<std::ptrdiff_t>(b.size() - 4));
        ++begin;
        REQUIRE(zipper.end() - begin == static_cast<std::ptrdiff_t>(b.size() - 5));
        REQUIRE(begin == zipper.end());

        begin = zipper.begin();
        auto end = zipper.end();
        --end;
        REQUIRE(end - begin == static_cast<std::ptrdiff_t>(b.size() - 1));
        --end;
        REQUIRE(end - begin == static_cast<std::ptrdiff_t>(b.size() - 2));
        --end;
        REQUIRE(end - begin == static_cast<std::ptrdiff_t>(b.size() - 3));
        --end;
        REQUIRE(end - begin == static_cast<std::ptrdiff_t>(b.size() - 4));
        --end;
        REQUIRE(end - begin == static_cast<std::ptrdiff_t>(b.size() - 5));
        REQUIRE(end == begin);

        end = zipper.end();
        begin = zipper.begin();
        REQUIRE(begin - end == -static_cast<std::ptrdiff_t>(5));
        REQUIRE((begin + 1) - end == -static_cast<std::ptrdiff_t>(4));
        REQUIRE((begin + 2) - end == -static_cast<std::ptrdiff_t>(3));
        REQUIRE((begin + 3) - end == -static_cast<std::ptrdiff_t>(2));
        REQUIRE((begin + 4) - end == -static_cast<std::ptrdiff_t>(1));
        REQUIRE((begin + 5) - end == 0);

        for (std::size_t i = 0; i < lz::size(zipper) / 2; ++i) {
            INFO("With i = " << i);
            REQUIRE((end - i) - (begin + i) == static_cast<std::ptrdiff_t>(lz::size(zipper) - 2 * i));
            REQUIRE((begin + i) - (end - i) == -static_cast<std::ptrdiff_t>(lz::size(zipper) - 2 * i));
        }
    }
}

TEST_CASE("Empty and one element zip longest") {
    SECTION("Empty") {
        std::vector<int> a;
        std::vector<float> b;
        auto zipper = lz::zip_longest(a, b);
        REQUIRE(lz::empty(zipper));
        REQUIRE(!lz::has_one(zipper));
        REQUIRE(!lz::has_many(zipper));
    }

    SECTION("One element 1") {
        std::vector<int> a = { 1 };
        std::vector<float> b;
        auto zipper = lz::zip_longest(a, b);
        REQUIRE(!lz::empty(zipper));
        REQUIRE(lz::has_one(zipper));
        REQUIRE(!lz::has_many(zipper));
    }

    SECTION("One element 2") {
        std::vector<int> a;
        std::vector<float> b = { 1.f };
        auto zipper = lz::zip_longest(a, b);
        REQUIRE(!lz::empty(zipper));
        REQUIRE(lz::has_one(zipper));
        REQUIRE(!lz::has_many(zipper));
    }
}

TEST_CASE("Zip longest iterable to container") {
    std::array<int, 4> a = { 1, 2, 3, 4 };
    std::list<float> b = { 1.f, 2.f, 3.f, 4.f, 5.f };
    std::vector<char> c = { 'a', 'b', 'c', 'd', 'f', 'g' };
    auto zipper = lz::zip_longest(a, b, c);

    SECTION("To array") {
        using optional_tuple = std::tuple<lz::optional<int>, lz::optional<float>, lz::optional<char>>;
        auto to_arr = zipper | lz::to<std::array<optional_tuple, 6>>();
        std::array<optional_tuple, 6> expected = {
            std::make_tuple(1, 1.f, 'a'), std::make_tuple(2, 2.f, 'b'),           std::make_tuple(3, 3.f, 'c'),
            std::make_tuple(4, 4.f, 'd'), std::make_tuple(lz::nullopt, 5.f, 'f'), std::make_tuple(lz::nullopt, lz::nullopt, 'g')
        };
        REQUIRE(std::equal(to_arr.begin(), to_arr.end(), expected.begin()));
    }

    SECTION("To vector") {
        using optional_tuple = std::tuple<lz::optional<int>, lz::optional<float>, lz::optional<char>>;
        auto to_vec = zipper | lz::to<std::vector<optional_tuple>>();
        std::vector<optional_tuple> expected = {
            std::make_tuple(1, 1.f, 'a'), std::make_tuple(2, 2.f, 'b'),           std::make_tuple(3, 3.f, 'c'),
            std::make_tuple(4, 4.f, 'd'), std::make_tuple(lz::nullopt, 5.f, 'f'), std::make_tuple(lz::nullopt, lz::nullopt, 'g')
        };
        REQUIRE(std::equal(to_vec.begin(), to_vec.end(), expected.begin()));
    }

    SECTION("To other container using to<>()") {
        using optional_tuple = std::tuple<lz::optional<int>, lz::optional<float>, lz::optional<char>>;
        auto to_list = zipper | lz::to<std::list<optional_tuple>>();
        std::list<optional_tuple> expected = {
            std::make_tuple(1, 1.f, 'a'), std::make_tuple(2, 2.f, 'b'),           std::make_tuple(3, 3.f, 'c'),
            std::make_tuple(4, 4.f, 'd'), std::make_tuple(lz::nullopt, 5.f, 'f'), std::make_tuple(lz::nullopt, lz::nullopt, 'g')
        };
        REQUIRE(std::equal(to_list.begin(), to_list.end(), expected.begin()));
    }

    SECTION("To map") {
        using tuple_ref = lz::ref_t<decltype(zipper.begin())>;
        using map_type = std::map<char, lz::optional<float>>;

        auto to_map = zipper |
                      lz::map([](const tuple_ref& tup) { return std::make_pair(std::get<2>(tup).value(), std::get<1>(tup)); }) |
                      lz::to<map_type>();
        map_type expected = { { 'a', 1.f }, { 'b', 2.f }, { 'c', 3.f }, { 'd', 4.f }, { 'f', 5.f }, { 'g', lz::nullopt } };
        REQUIRE(to_map == expected);
    }

    SECTION("To unordered map") {
        using tuple_ref = lz::ref_t<decltype(zipper.begin())>;
        using map_type = std::unordered_map<char, lz::optional<float>>;

        auto to_unordered =
            zipper | lz::map([](const tuple_ref& tup) { return std::make_pair(std::get<2>(tup).value(), std::get<1>(tup)); }) |
            lz::to<map_type>();
        map_type expected = { { 'a', 1.f }, { 'b', 2.f }, { 'c', 3.f }, { 'd', 4.f }, { 'f', 5.f }, { 'g', lz::nullopt } };
        REQUIRE(to_unordered == expected);
    }
}
