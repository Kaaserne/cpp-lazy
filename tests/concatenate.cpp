#include <Lz/c_string.hpp>
#include <Lz/concatenate.hpp>
#include <Lz/map.hpp>
#include <catch2/catch.hpp>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Concatenate with sentinels") {
    const char* str = "hello, world!";
    auto cstr = lz::c_string(str);

    std::vector<char> vec = { 'h', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!' };
    auto concat = lz::concat(cstr, vec);
    static_assert(std::is_same<lz::default_sentinel, decltype(concat.end())>::value, "Sentinel type should be default_sentinel");
    std::vector<char> expected = { 'h', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!',
                                   'h', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!' };
    auto actual = concat | lz::to<std::vector<char>>();
    REQUIRE(lz::equal(actual, expected));
}

TEST_CASE("Concat changing and creating elements", "[Concat][Basic functionality]") {
    std::string a = "hello ";
    std::string b = "world";

    auto concat = a | lz::concat(b);
    CHECK(concat.size() == a.size() + b.size());
    static_assert(std::is_same<decltype(concat.begin()), decltype(concat.end())>::value, "Should not be sentinel");

    SECTION("Should be by reference") {
        *concat.begin() = 'd';
        REQUIRE(a[0] == 'd');
    }

    SECTION("Should concat") {
        constexpr const char* expected = "hello world";
        REQUIRE((concat | lz::to<std::string>()) == expected);
    }

    SECTION("Length should be correct") {
        auto dist = static_cast<std::size_t>(std::distance(concat.begin(), concat.end()));
        REQUIRE(dist == a.size() + b.size());
    }

    SECTION("Should (be) sort(ed)") {
        std::array<int, 10> arr1 = { 5, 2, 67, 1235, 654, 23, 6, 1324, 6, 34 };
        std::array<int, 10> arr2 = { 756, 23, 465, 1, 6, 4, 1234, 65, 567, 2 };
        auto concatted = lz::concat(arr1, arr2);
        std::sort(concatted.begin(), concatted.end());
        REQUIRE(std::is_sorted(concatted.begin(), concatted.end()));
    }
}

TEST_CASE("Empty or one element concatenate") {
    SECTION("Empty") {
        std::string a;
        std::string b;
        auto concat = lz::concat(a, b);
        CHECK(concat.size() == 0);
        REQUIRE(lz::empty(concat));
        REQUIRE(!lz::has_one(concat));
        REQUIRE(!lz::has_many(concat));
    }

    SECTION("One element 1") {
        std::string a = "h";
        std::string b;
        auto concat = lz::concat(a, b);
        CHECK(concat.size() == 1);
        REQUIRE(!lz::empty(concat));
        REQUIRE(lz::has_one(concat));
        REQUIRE(!lz::has_many(concat));
    }

    SECTION("One element 2") {
        std::string a;
        std::string b = "w";
        auto concat = lz::concat(a, b);
        CHECK(concat.size() == 1);
        REQUIRE(!lz::empty(concat));
        REQUIRE(lz::has_one(concat));
        REQUIRE(!lz::has_many(concat));
    }

    SECTION("One element both") {
        std::string a = "h";
        std::string b = "w";
        auto concat = lz::concat(a, b);
        CHECK(concat.size() == 2);
        REQUIRE(!lz::empty(concat));
        REQUIRE(!lz::has_one(concat));
        REQUIRE(lz::has_many(concat));
    }
}

TEST_CASE("Concat binary operations", "[Concat][Binary ops]") {
    std::string a = "hello ", b = "world";
    auto concat = lz::concat(a, b);
    CHECK(concat.size() == a.size() + b.size());
    auto begin = concat.begin();

    REQUIRE(*begin == 'h');

    SECTION("Operator++") {
        ++begin;
        REQUIRE(*begin == 'e');
    }

    SECTION("Operator--") {
        ++begin;
        --begin;
        REQUIRE(*begin == 'h');
        ++begin, ++begin, ++begin, ++begin, ++begin, ++begin;
        --begin;
        REQUIRE(*begin == ' ');
        --begin;
        REQUIRE(*begin == 'o');
    }

    SECTION("Operator== & operator!=") {
        REQUIRE(begin != concat.end());
        begin = concat.end();
        REQUIRE(begin == concat.end());
    }

    SECTION("Operator+(int), tests += as well") {
        REQUIRE(*(begin + static_cast<std::ptrdiff_t>(a.size())) == 'w');
    }

    SECTION("Operator-(int), tests -= as well") {
        begin += static_cast<std::ptrdiff_t>(a.size());
        REQUIRE(*begin == 'w');
    }

    SECTION("Operator-(Iterator)") {
        REQUIRE(static_cast<std::size_t>(concat.end() - begin) == a.size() + b.size());
        REQUIRE(static_cast<std::size_t>(std::distance(concat.begin(), concat.end())) == a.size() + b.size());
    }

    SECTION("Operator[]()") {
        REQUIRE(begin[static_cast<std::ptrdiff_t>(a.size())] == 'w');
    }

    SECTION("Operator<, '<, <=, >, >='") {
        auto end = concat.end();
        const auto distance = std::distance(begin, end) - 1;

        REQUIRE(begin < end);
        begin += distance;
        REQUIRE(begin <= end);
        end -= distance;

        REQUIRE(begin > end);
        REQUIRE(begin >= end);
    }
}

TEST_CASE("Concatenate to containers", "[Concatenate][To container]") {
    std::vector<int> v1 = { 1, 2, 3 };
    std::vector<int> v2 = { 4, 5, 6 };
    auto concat = lz::concat(v1, v2);

    SECTION("To array") {
        constexpr std::size_t size = 3 + 3;
        REQUIRE((concat | lz::to<std::array<int, size>>()) == std::array<int, size>{ 1, 2, 3, 4, 5, 6 });
    }

    SECTION("To vector") {
        REQUIRE((concat | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 4, 5, 6 });
    }

    SECTION("To other container using to<>()") {
        REQUIRE((concat | lz::to<std::list<int>>()) == std::list<int>{ 1, 2, 3, 4, 5, 6 });
    }

    SECTION("To map") {
        auto map = concat | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();
        std::map<int, int> expected = { std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3),
                                        std::make_pair(4, 4), std::make_pair(5, 5), std::make_pair(6, 6) };
        REQUIRE(map == expected);
    }

    SECTION("To unordered map") {
        auto map = concat | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        std::unordered_map<int, int> expected = { std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3),
                                                  std::make_pair(4, 4), std::make_pair(5, 5), std::make_pair(6, 6) };
        REQUIRE(map == expected);
    }
}