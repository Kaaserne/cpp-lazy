#include <Lz/c_string.hpp>
#include <Lz/filter.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <Lz/rotate.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("rotate_iterable with sentinels") {
    auto c_str = lz::c_string("Hello, World!");
    auto rotated = lz::rotate(c_str, 7);
    static_assert(!std::is_same<decltype(rotated.begin()), decltype(rotated.end())>::value, "Should be sentinel");
    REQUIRE((rotated | lz::to<std::string>()) == "World!Hello, ");
}

TEST_CASE("rotate_iterable basic functionality") {
    std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
    auto rotate = lz::rotate(arr, 2);
    REQUIRE(rotate.size() == arr.size());

    SECTION("Should be correct length") {
        auto beg = rotate.begin();
        REQUIRE(std::distance(beg, rotate.end()) == static_cast<std::ptrdiff_t>(arr.size()));
        ++beg, ++beg;
        REQUIRE(std::distance(beg, rotate.end()) == 3);
        ++beg;
        REQUIRE(std::distance(beg, rotate.end()) == 2);
    }

    SECTION("With bidirectional iterator") {
        std::list<int> lst = { 1, 2, 3, 4, 5, 6 };
        auto rotator = lz::rotate(lst, 2);
        REQUIRE(std::distance(rotator.begin(), rotator.end()) == static_cast<std::ptrdiff_t>(lst.size()));
        REQUIRE(rotator.size() == lst.size());
    }
}

TEST_CASE("rotate_iterable binary operations") {
    std::array<int, 4> arr = { 1, 2, 3, 4 };
    auto rotate = lz::rotate(arr, 3);

    auto begin = rotate.begin();
    auto end = rotate.end();

    REQUIRE(*begin == 4);
    REQUIRE(*end == 4);

    SECTION("Operator++") {
        ++begin;
        REQUIRE(*begin == 1);
        ++end;
        REQUIRE(*end == 1);
    }

    SECTION("Operator--") {
        REQUIRE(begin != end);
        --end;
        REQUIRE(*end == 3);
        --end;
        REQUIRE(*end == 2);
        --end;
        REQUIRE(*end == 1);
        --end;
        REQUIRE(*end == 4);
        REQUIRE(begin == end);
    }

    SECTION("Operator== & Operator!=") {
        REQUIRE(begin != end);
        begin = end;
        REQUIRE(begin == end);
    }

    SECTION("Various ++ and -- operators with begin and end") {
        std::array<int, 5> container = { 1, 2, 3, 4, 5 };
        auto rotator = lz::rotate(container, 3);

        auto uneven_begin = rotator.begin();
        auto uneven_end = rotator.end();

        ++uneven_begin;
        REQUIRE(*uneven_begin == 5);
        --uneven_begin;
        REQUIRE(*uneven_begin == 4);
        ++uneven_begin;

        ++uneven_begin;
        REQUIRE(*uneven_begin == 1);

        --uneven_end;
        REQUIRE(*uneven_end == 3);

        --uneven_end;
        REQUIRE(*uneven_end == 2);

        --uneven_end;
        REQUIRE(*uneven_end == 1);

        --uneven_end;
        REQUIRE(*uneven_end == 5);
    }

    SECTION("Operator+= and Operator-=") {
        begin += 0;
        REQUIRE(*begin == 4);
        begin -= 0;
        REQUIRE(*begin == 4);
        begin += 2;
        REQUIRE(*begin == 2);
        begin += 1;
        REQUIRE(*begin == 3);
        begin -= 1;
        REQUIRE(*begin == 2);
        begin -= 2;
        REQUIRE(*begin == 4);
        begin += 4;
        REQUIRE(begin == end);

        end -= 1;
        REQUIRE(*end == 3);
        end -= 0;
        REQUIRE(*end == 3);
        end += 0;
        REQUIRE(*end == 3);
        end -= 3;
        REQUIRE(*end == 4);
        end += 4;
        REQUIRE(begin == end);
    }

    SECTION("Operator-") {
        REQUIRE(end - begin == 4);
        REQUIRE(begin - end == -4);
        ++begin;
        REQUIRE(end - begin == 3);
        REQUIRE(begin - end == -3);
        ++begin;
        REQUIRE(end - begin == 2);
        REQUIRE(begin - end == -2);
        ++begin;
        REQUIRE(end - begin == 1);
        REQUIRE(begin - end == -1);
        ++begin;
        REQUIRE(end - begin == 0);
        REQUIRE(begin - end == 0);
    }
}

TEST_CASE("Empty or one element rotate") {
    SECTION("Empty") {
        std::vector<int> vec{};
        auto rotate = lz::rotate(vec, 0);
        REQUIRE(lz::empty(rotate));
        REQUIRE(!lz::has_many(rotate));
        REQUIRE(!lz::has_one(rotate));
    }

    SECTION("One element") {
        std::vector<int> vec = { 1 };
        auto rotate = lz::rotate(vec, 0);
        REQUIRE(!lz::empty(rotate));
        REQUIRE(!lz::has_many(rotate));
        REQUIRE(lz::has_one(rotate));
    }

    SECTION("Empty with sentinel") {
        auto cstr = lz::c_string("");
        auto rotate = lz::rotate(cstr, 0);
        REQUIRE(lz::empty(rotate));
        REQUIRE(!lz::has_many(rotate));
        REQUIRE(!lz::has_one(rotate));
    }

    SECTION("One element with sentinel") {
        auto cstr = lz::c_string("a");
        auto rotate = lz::rotate(cstr, 0);
        REQUIRE(!lz::empty(rotate));
        REQUIRE(!lz::has_many(rotate));
        REQUIRE(lz::has_one(rotate));
    }
}

TEST_CASE("rotate_iterable to containers") {
    constexpr std::size_t size = 6;
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };
    auto rotator = lz::rotate(vec, 2);

    SECTION("Reverse to container") {
        auto reversed = lz::reverse(rotator);
        REQUIRE((reversed | lz::to<std::array<int, size>>()) == std::array<int, size>{ 2, 1, 6, 5, 4, 3 });
    }

    SECTION("To array") {
        REQUIRE((rotator | lz::to<std::array<int, size>>()) == std::array<int, size>{ 3, 4, 5, 6, 1, 2 });
    }

    SECTION("To vector") {
        REQUIRE((rotator | lz::to<std::vector>()) == std::vector<int>{ 3, 4, 5, 6, 1, 2 });
    }

    SECTION("To other container using to<>()") {
        REQUIRE((rotator | lz::to<std::list<int>>()) == std::list<int>{ 3, 4, 5, 6, 1, 2 });
    }

    SECTION("To map") {
        auto map = rotator | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();
        REQUIRE(map == std::map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 1, 1 }, { 2, 2 } });
    }

    SECTION("To unordered map") {
        auto map = rotator | lz::map([](int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        REQUIRE(map == std::unordered_map<int, int>{ { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 }, { 1, 1 }, { 2, 2 } });
    }
}
