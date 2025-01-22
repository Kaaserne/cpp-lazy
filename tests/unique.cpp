#include <Lz/c_string.hpp>
#include <Lz/unique.hpp>
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Unique using sentinels") {
    auto str = lz::c_string("aabbcccddefgghhj");
    auto unique = lz::unique(str);
    static_assert(!std::is_same<decltype(unique.begin()), decltype(unique.end())>::value, "Should be sentinel");
    auto expected = lz::c_string("abcdefghj");
    REQUIRE(lz::equal(unique, expected));
}

TEST_CASE("Unique changing and creating elements", "[Unique][Basic functionality]") {
    std::array<int, 4> arr = { 3, 2, 3, 1 };
    std::sort(arr.begin(), arr.end());
    auto unique = lz::unique(arr);
    auto beg = unique.begin();
    constexpr std::size_t size = 3;

    REQUIRE(*beg == 1);
    REQUIRE(static_cast<std::size_t>(std::distance(beg, unique.end())) == size);

    SECTION("Should be unique") {
        std::array<int, size> expected = { 1, 2, 3 };
        REQUIRE(expected == unique.to<std::array<int, size>>());
    }

    SECTION("Should be unique too, using >") {
        std::array<int, size> expected = { 3, 2, 1 };
        auto unique_greater = lz::unique(expected, std::greater<int>());
        REQUIRE(expected == unique_greater.to<std::array<int, size>>());
        REQUIRE(std::is_sorted(expected.begin(), expected.end(), std::greater<int>()));
    }
}

TEST_CASE("Unique binary operations", "[Unique][Binary ops]") {
    SECTION("Operator++") {
        std::array<int, 4> arr = { 3, 2, 3, 1 };
        std::sort(arr.begin(), arr.end());
        auto unique = lz::unique(arr);
        auto beg = unique.begin();
        ++beg;
        REQUIRE(*beg == 2);
    }

    SECTION("Operator==, operator!=") {
        std::array<int, 4> arr = { 3, 2, 3, 1 };
        std::sort(arr.begin(), arr.end());
        auto unique = lz::unique(arr);
        auto beg = unique.begin();
        REQUIRE(beg != unique.end());
        beg = unique.end();
        REQUIRE(beg == unique.end());
    }

    SECTION("Operator--") {
        // 3, 3, 3, 2, 2, 1
        std::array<int, 6> arr = { 3, 2, 3, 1, 3, 2 };
        std::sort(arr.begin(), arr.end(), std::greater<int>());
        auto unique = lz::unique(arr);
        auto iter = unique.end();
        --iter;
        REQUIRE(*iter == 1);
        --iter;
        REQUIRE(*iter == 2);
        REQUIRE(&*iter == &arr[3]);
        --iter;
        REQUIRE(*iter == 3);
        REQUIRE(iter == unique.begin());
    }
}

TEST_CASE("Empty or one element unique") {
    SECTION("Empty") {
        std::vector<int> vec;
        auto unique = lz::unique(vec);
        REQUIRE(lz::empty(unique));
        REQUIRE(!lz::has_many(unique));
        REQUIRE(!lz::has_one(unique));
    }

    SECTION("One element") {
        std::vector<int> vec = { 1 };
        auto unique = lz::unique(vec);
        REQUIRE(lz::has_one(unique));
        REQUIRE(!lz::has_many(unique));
        REQUIRE(!lz::empty(unique));
    }
}

TEST_CASE("Unique to container", "[Unique][To container]") {
    std::array<int, 4> arr = { 3, 2, 3, 1 };
    std::sort(arr.begin(), arr.end());
    constexpr std::size_t size = 3;
    auto unique = lz::unique(arr);

    SECTION("To array") {
        auto unique_array = unique.to<std::array<int, size>>();
        std::array<int, size> expected = { 1, 2, 3 };
        REQUIRE(unique_array == expected);
    }

    SECTION("To vector") {
        auto unique_vec = unique.to_vector();
        std::vector<int> expected = { 1, 2, 3 };
        REQUIRE(unique_vec == expected);
    }

    SECTION("To other container using to<>()") {
        auto unique_list = unique.to<std::list<int>>();
        std::list<int> expected = { 1, 2, 3 };
        REQUIRE(unique_list == expected);
    }

    SECTION("To map") {
        std::map<int, int> actual = unique.to_map([](const int i) { return std::make_pair(i, i); });

        std::map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2),
            std::make_pair(3, 3),
        };

        REQUIRE(expected == actual);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, int> actual = unique.to_unordered_map([](const int i) { return std::make_pair(i, i); });

        std::unordered_map<int, int> expected = {
            std::make_pair(1, 1),
            std::make_pair(2, 2),
            std::make_pair(3, 3),
        };

        REQUIRE(expected == actual);
    }
}
