#include <Lz/c_string.hpp>
#include <Lz/drop.hpp>
#include <Lz/map.hpp>
#include <Lz/slice.hpp>
#include <Lz/take.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Take with sentinels") {
    const char* str = "Hello, world!";
    auto c_string = lz::c_string(str);
    auto take = c_string | lz::take(5);
    static_assert(!std::is_same<decltype(take.begin()), decltype(take.end())>::value, "Should be sentinel");
    auto expected = lz::c_string("Hello");
    REQUIRE(lz::equal(take, expected));
    auto vec = take | lz::to<std::vector<char>>();
    REQUIRE(lz::equal(vec, expected));

    SECTION("Operator=") {
        auto it = take.begin();
        REQUIRE(it == take.begin());
        it = take.end();
        REQUIRE(it == take.end());
    }
}

TEST_CASE("Take changing and creating elements") {
    std::array<int, 10> array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take = lz::take(array, 4);
    REQUIRE(take.size() == 4);
    REQUIRE(*take.begin() == 1);
    REQUIRE(*std::next(take.begin()) == 2);
    REQUIRE(*std::prev(take.end()) == 4);
    *take.begin() = 0;
    REQUIRE(array[0] == 0);
}

TEST_CASE("Take binary operations where n is smaller than size") {
    std::array<int, 10> array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take = lz::take(array, 4);
    REQUIRE(take.size() == 4);

    SECTION("Operator++") {
        auto begin = take.begin();
        ++begin;
        REQUIRE(*begin == 2);
    }

    SECTION("Operator--") {
        auto end = take.end();
        --end;
        REQUIRE(*end == 4);
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(take.begin() != take.end());
        REQUIRE_FALSE(take.begin() == take.end());
        auto it = take.begin();
        it = take.end();
        REQUIRE(it == take.end());
    }

    SECTION("Operator+") {
        auto begin = take.begin();
        auto end = take.end();

        std::vector<int> expected = { 1, 2, 3, 4 };
        for (std::size_t i = 0; i < lz::size(take) - 1; ++i) {
            REQUIRE(*(begin + i) == *(expected.begin() + i));
        }
        REQUIRE(begin + lz::size(take) == take.end());
        for (std::size_t i = 1; i <= lz::size(take); ++i) {
            REQUIRE(*(end - i) == *(expected.end() - i));
        }
        REQUIRE(end - lz::size(take) == take.begin());

        std::advance(begin, lz::size(take));
        std::advance(end, -static_cast<std::ptrdiff_t>(lz::size(take)));

        for (std::size_t i = 0; i < lz::size(take) - 1; ++i) {
            REQUIRE(*(end + i) == *(expected.begin() + i));
        }
        REQUIRE(end + lz::size(take) == take.end());
        for (std::size_t i = 1; i <= lz::size(take); ++i) {
            REQUIRE(*(begin - i) == *(expected.end() - i));
        }
        REQUIRE(begin - lz::size(take) == take.begin());
    }

    SECTION("Operator-") {
        auto begin = take.begin();
        auto end = take.end();
        for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(lz::size(take)); ++i) {
            INFO("With i = " << i);
            REQUIRE((end - i) - begin == static_cast<std::ptrdiff_t>(lz::size(take) - i));
            REQUIRE(end - (begin + i) == static_cast<std::ptrdiff_t>(lz::size(take) - i));
            REQUIRE((begin + i) - end == -static_cast<std::ptrdiff_t>(lz::size(take) - i));
            REQUIRE(begin - (end - i) == -static_cast<std::ptrdiff_t>(lz::size(take) - i));
        }

        for (std::size_t i = 0; i < lz::size(take); ++i) {
            INFO("With i = " << i);
            REQUIRE((end - i) - (begin + i) == static_cast<std::ptrdiff_t>(lz::size(take) - 2 * i));
            REQUIRE((begin + i) - (end - i) == -static_cast<std::ptrdiff_t>(lz::size(take) - 2 * i));
        }
    }
}

TEST_CASE("Take binary operations where n is larger than size") {
    std::array<int, 10> array = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto take = lz::take(array, 20);
    REQUIRE(take.size() == 10);

    SECTION("Operator++") {
        auto begin = take.begin();
        ++begin;
        REQUIRE(*begin == 2);
    }

    SECTION("Operator--") {
        auto end = take.end();
        --end;
        REQUIRE(*end == 10);
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(take.begin() != take.end());
        REQUIRE_FALSE(take.begin() == take.end());
        auto it = take.begin();
        it = take.end();
        REQUIRE(it == take.end());
    }

    SECTION("Operator+") {
        auto begin = take.begin();
        auto end = take.end();

        std::vector<int> expected = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        for (std::size_t i = 0; i < lz::size(take) - 1; ++i) {
            REQUIRE(*(begin + i) == *(expected.begin() + i));
        }
        REQUIRE(begin + lz::size(take) == take.end());
        for (std::size_t i = 1; i <= lz::size(take); ++i) {
            REQUIRE(*(end - i) == *(expected.end() - i));
        }
        REQUIRE(end - lz::size(take) == take.begin());

        std::advance(begin, lz::size(take));
        std::advance(end, -static_cast<std::ptrdiff_t>(lz::size(take)));

        for (std::size_t i = 0; i < lz::size(take) - 1; ++i) {
            REQUIRE(*(end + i) == *(expected.begin() + i));
        }
        REQUIRE(end + lz::size(take) == take.end());
        for (std::size_t i = 1; i <= lz::size(take); ++i) {
            REQUIRE(*(begin - i) == *(expected.end() - i));
        }
        REQUIRE(begin - lz::size(take) == take.begin());
    }
}

TEST_CASE("Empty or one element take") {
    SECTION("Empty") {
        std::vector<int> vec;
        auto take = lz::take(vec, 0);
        REQUIRE(take.size() == 0);
        REQUIRE(lz::empty(take));
        REQUIRE(!lz::has_one(take));
        REQUIRE(!lz::has_many(take));
    }

    SECTION("One element") {
        std::vector<int> vec = { 1 };
        auto take = lz::take(vec, 1);
        REQUIRE(take.size() == 1);
        REQUIRE(!lz::empty(take));
        REQUIRE(lz::has_one(take));
        REQUIRE(!lz::has_many(take));
    }
}

TEST_CASE("Take to containers") {
    constexpr std::size_t size = 8;
    std::array<int, size> array = { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto take = lz::take(array, 4);
    REQUIRE(take.size() == 4);

    SECTION("To array") {
        auto arr = take | lz::to<std::array<int, 4>>();
        REQUIRE(arr.size() == 4);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
        REQUIRE(arr[3] == 4);
    }

    SECTION("To vector") {
        auto vec = take | lz::to<std::vector>(std::allocator<int>());
        REQUIRE(std::equal(vec.begin(), vec.end(), take.begin()));
    }

    SECTION("To other container using to<>()") {
        auto lst = take | lz::to<std::list<int>>(std::allocator<int>());
        REQUIRE(std::equal(lst.begin(), lst.end(), take.begin()));
    }

    SECTION("To map") {
        auto map_obj = take | lz::map([](int i) { return std::make_pair(i, i); });
        auto map = lz::to<std::map<int, int>>(map_obj);
        REQUIRE(map.size() == 4);
        std::map<int, int> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
        REQUIRE(map == expected);
    }

    SECTION("To unordered map") {
        auto map = take | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        REQUIRE(map.size() == 4);
        std::unordered_map<int, int> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
        REQUIRE(map == expected);
    }
}

TEST_CASE("Drop & slice") {
    SECTION("Drop iterator") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto drop = vec | lz::drop(4);
        REQUIRE(drop.size() == 4);
        std::vector<int> expected = { 5, 6, 7, 8 };
        REQUIRE(lz::equal(drop, expected));
    }

    SECTION("Drop where n is larger than size / 2, sized & random access") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto drop = vec | lz::drop(6);
        REQUIRE(drop.size() == 2);
        std::vector<int> expected = { 7, 8 };
        REQUIRE(lz::equal(drop, expected));
    }

    SECTION("Drop where n is larger than size / 2, sized & bidirectional") {
        std::list<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto drop = vec | lz::drop(6);
        REQUIRE(drop.size() == 2);
        std::vector<int> expected = { 7, 8 };
        REQUIRE(lz::equal(drop, expected));
    }

    SECTION("Drop where n is larger than size / 2, not sized & forward") {
        auto cstr = lz::c_string("Hello, world!");
        auto drop = cstr | lz::drop(7);
        std::vector<char> expected = { 'w', 'o', 'r', 'l', 'd', '!' };
        REQUIRE(lz::equal(drop, expected));
    }

    SECTION("Slice iterable") {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
        auto slice = vec | lz::slice(2, 6);
        REQUIRE(slice.size() == 4);
        std::vector<int> expected = { 3, 4, 5, 6 };
        auto result = lz::to<std::vector>(slice, std::allocator<int>());
        REQUIRE(std::equal(slice.begin(), slice.end(), expected.begin()));
    }
}