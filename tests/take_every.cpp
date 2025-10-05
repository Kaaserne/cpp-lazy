#include <Lz/algorithm/empty.hpp>
#include <Lz/algorithm/equal.hpp>
#include <Lz/algorithm/has_many.hpp>
#include <Lz/algorithm/has_one.hpp>
#include <Lz/c_string.hpp>
#include <Lz/filter.hpp>
#include <Lz/map.hpp>
#include <Lz/procs/to.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <Lz/take_every.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <cpp-lazy-ut-helper/ut_helper.hpp>
#include <doctest/doctest.h>

TEST_CASE("take_every_iterable with sentinels") {
    auto cstr = lz::c_string("Hello");
    lz::take_every_iterable<decltype(cstr)> take_every = lz::take_every(cstr, 2);
    static_assert(!std::is_same<decltype(take_every.begin()), decltype(take_every.end())>::value, "Should be sentinel");
    auto expected = lz::c_string("Hlo");
    REQUIRE(lz::equal(take_every, expected));

    SUBCASE("Operator=(default_sentinel_t)") {
        SUBCASE("forward") {
            std::forward_list<int> lst{ 1, 2, 3, 4, 5 };
            auto take_every2 = lz::take_every(lst, 2);
            auto common = make_sentinel_assign_op_tester(take_every2);
            auto expected2 = { 1, 3, 5 };
            REQUIRE(lz::equal(common, expected2));
        }

        SUBCASE("bidirectional") {
            std::list<int> list{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
            auto filtered = list | lz::filter([](int i) { return i % 2 == 0; });
            // Make it so that it has a sentinel and is bidirectional
            auto t = make_sized_bidi_sentinelled(filtered);
            auto take_every2 = lz::take_every(t, 2);
            auto common = make_sentinel_assign_op_tester(take_every2);
            std::vector<int> expected2 = { 2, 6, 10 };
            REQUIRE(lz::equal(common, expected2));
        }

        SUBCASE("random access") {
            auto repeater = lz::repeat(20, 5);
            auto end = repeater.begin();
            end = repeater.end(); // calls operator=(sentinel)
            auto begin = repeater.begin();
            auto common = lz::take_every(lz::make_basic_iterable(begin, end), 2);
            auto expected2 = { 20, 20, 20 };
            REQUIRE(lz::equal(common, expected2));
        }
    }
}

TEST_CASE("take_every_iterable changing and creating elements") {
    constexpr std::size_t size = 4;
    std::array<int, size> array = { 1, 2, 3, 4 };
    auto take_every = array | lz::take_every(2);
    auto iterator = take_every.begin();

    SUBCASE("take_every_iterable should be by reference") {
        *iterator = 0;
        REQUIRE(array[0] == 0);
    }

    SUBCASE("take_every_iterable should select every amount-th") {
        REQUIRE(*iterator == 1);
        ++iterator;
        REQUIRE(*iterator == 3);
        ++iterator;
        REQUIRE(iterator == take_every.end());
    }
}

TEST_CASE("Empty or one element take every") {
    SUBCASE("Empty 1") {
        std::vector<int> vec;
        auto take_every = lz::take_every(vec, 2);
        REQUIRE(take_every.size() == 0);
        REQUIRE(lz::empty(take_every));
        REQUIRE_FALSE(lz::has_many(take_every));
        REQUIRE_FALSE(lz::has_one(take_every));
    }

    SUBCASE("Empty 2") {
        std::vector<int> vec;
        auto take_every = lz::take_every(vec, 2, 3);
        REQUIRE(take_every.size() == 0);
        REQUIRE(lz::empty(take_every));
        REQUIRE_FALSE(lz::has_many(take_every));
        REQUIRE_FALSE(lz::has_one(take_every));
    }

    SUBCASE("Empty 3") {
        std::list<int> vec;
        auto take_every = lz::take_every(vec, 2, 3);
        REQUIRE(take_every.size() == 0);
        REQUIRE(lz::empty(take_every));
        REQUIRE_FALSE(lz::has_many(take_every));
        REQUIRE_FALSE(lz::has_one(take_every));
    }

    SUBCASE("Empty 4") {
        std::list<int> vec;
        auto take_every = lz::take_every(vec, 2);
        REQUIRE(take_every.size() == 0);
        REQUIRE(lz::empty(take_every));
        REQUIRE_FALSE(lz::has_many(take_every));
        REQUIRE_FALSE(lz::has_one(take_every));
    }

    SUBCASE("One element") {
        std::vector<int> vec = { 1 };
        auto take_every = lz::take_every(vec, 2);
        REQUIRE(take_every.size() == 1);
        REQUIRE_FALSE(lz::empty(take_every));
        REQUIRE_FALSE(lz::has_many(take_every));
        REQUIRE(lz::has_one(take_every));
    }

    SUBCASE("One element 2") {
        std::vector<int> vec = { 1 };
        auto take_every = lz::take_every(vec, 2, 3);
        REQUIRE(take_every.size() == 0);
        REQUIRE(lz::empty(take_every));
        REQUIRE_FALSE(lz::has_many(take_every));
        REQUIRE_FALSE(lz::has_one(take_every));
    }

    SUBCASE("One element 3") {
        std::list<int> vec = { 1 };
        auto take_every = lz::take_every(vec, 2);
        REQUIRE(take_every.size() == 1);
        REQUIRE_FALSE(lz::empty(take_every));
        REQUIRE_FALSE(lz::has_many(take_every));
        REQUIRE(lz::has_one(take_every));
    }

    SUBCASE("One element 4") {
        std::list<int> vec = { 1 };
        auto take_every = lz::take_every(vec, 2, 3);
        REQUIRE(take_every.size() == 0);
        REQUIRE(lz::empty(take_every));
        REQUIRE_FALSE(lz::has_many(take_every));
        REQUIRE_FALSE(lz::has_one(take_every));
    }
}

template<class Container1, class Container2>
void operator_pp_test(const Container1& even_sized, const Container2& uneven_sized) {
    INFO("Operator++");
    LZ_ASSERT(lz::eager_size(even_sized) == 4, "Must be size 4");
    LZ_ASSERT(lz::eager_size(uneven_sized) == 5, "Must be size 5");

    auto even_sized_even_take = lz::take_every(even_sized, 2);
    auto even_sized_odd_take = lz::take_every(even_sized, 3);
    auto uneven_sized_even_take = lz::take_every(uneven_sized, 2);
    auto uneven_sized_odd_take = lz::take_every(uneven_sized, 3);

    auto expected1 = { 1, 3 };
    auto expected2 = { 1, 4 };
    auto expected3 = { 1, 3, 5 };
    auto expected4 = { 1, 4 };

    REQUIRE(lz::equal(even_sized_even_take, expected1));
    REQUIRE(lz::equal(even_sized_odd_take, expected2));
    REQUIRE(lz::equal(uneven_sized_even_take, expected3));
    REQUIRE(lz::equal(uneven_sized_odd_take, expected4));
}

template<class Container1, class Container2>
void operator_mm_test(const Container1 even_sized, const Container2& uneven_sized) {
    INFO("Operator--");
    LZ_ASSERT(even_sized.size() == 4, "Must be size 4");
    LZ_ASSERT(uneven_sized.size() == 5, "Must be size 5");

    // even sized = { 1, 2, 3, 4 }
    // uneven sized = { 1, 2, 3, 4, 5 }
    auto even_sized_even_take = lz::take_every(even_sized, 2);     // {3, 1}
    auto even_sized_odd_take = lz::take_every(even_sized, 3);      // {4, 1}
    auto uneven_sized_even_take = lz::take_every(uneven_sized, 2); // {5, 3, 1}
    auto uneven_sized_odd_take = lz::take_every(uneven_sized, 3);  // {4, 1}

    auto expected1 = { 3, 1 };
    auto expected2 = { 4, 1 };
    auto expected3 = { 5, 3, 1 };
    auto expected4 = { 4, 1 };

    REQUIRE(lz::equal(lz::reverse(even_sized_even_take), expected1));
    REQUIRE(lz::equal(lz::reverse(even_sized_odd_take), expected2));
    REQUIRE(lz::equal(lz::reverse(uneven_sized_even_take), expected3));
    REQUIRE(lz::equal(lz::reverse(uneven_sized_odd_take), expected4));
}

TEST_CASE("take_every_iterable binary operations") {
    SUBCASE("Operator++") {
        std::array<int, 4> even_sized = { 1, 2, 3, 4 };
        std::array<int, 5> odd_sized = { 1, 2, 3, 4, 5 };
        operator_pp_test(even_sized, odd_sized);

        std::list<int> even_sized_list = { 1, 2, 3, 4 };
        std::list<int> odd_sized_list = { 1, 2, 3, 4, 5 };
        operator_pp_test(even_sized_list, odd_sized_list);

        std::forward_list<int> even_sized_forward_list = { 1, 2, 3, 4 };
        std::forward_list<int> odd_sized_forward_list = { 1, 2, 3, 4, 5 };
        operator_pp_test(even_sized_forward_list, odd_sized_forward_list);
    }

    SUBCASE("Operator--") {
        std::array<int, 4> even_sized = { 1, 2, 3, 4 };
        std::array<int, 5> odd_sized = { 1, 2, 3, 4, 5 };
        operator_mm_test(even_sized, odd_sized);

        std::list<int> even_sized_list = { 1, 2, 3, 4 };
        std::list<int> odd_sized_list = { 1, 2, 3, 4, 5 };
        operator_mm_test(even_sized_list, odd_sized_list);
    }

    SUBCASE("Operator+") {
        std::vector<int> even_sized = { 1, 2, 3, 4 };
        std::vector<int> odd_sized = { 1, 2, 3, 4, 5 };

        auto even_sized_even_take = lz::take_every(even_sized, 2);
        auto even_sized_odd_take = lz::take_every(even_sized, 3);
        auto uneven_sized_even_take = lz::take_every(odd_sized, 2);
        auto uneven_sized_odd_take = lz::take_every(odd_sized, 3);

        std::vector<int> expected = { 1, 3 };
        test_procs::test_operator_plus(expected, even_sized_even_take);

        expected = { 1, 4 };
        test_procs::test_operator_plus(expected, even_sized_odd_take);

        expected = { 1, 3, 5 };
        test_procs::test_operator_plus(expected, uneven_sized_even_take);

        expected = { 1, 4 };
        test_procs::test_operator_plus(expected, uneven_sized_odd_take);
    }

    SUBCASE("Operator-") {
        std::vector<int> even_sized = { 1, 2, 3, 4 };
        std::vector<int> odd_sized = { 1, 2, 3, 4, 5 };
        auto even_sized_even_take = lz::take_every(even_sized, 2);
        auto even_sized_odd_take = lz::take_every(even_sized, 3);
        auto uneven_sized_even_take = lz::take_every(odd_sized, 2);
        auto uneven_sized_odd_take = lz::take_every(odd_sized, 3);

        test_procs::test_operator_minus(even_sized_even_take);
        test_procs::test_operator_minus(even_sized_odd_take);
        test_procs::test_operator_minus(uneven_sized_even_take);
        test_procs::test_operator_minus(uneven_sized_odd_take);
    }

    SUBCASE("Operator-(default_sentinel_t)") {
        auto even_sized = lz::repeat(1, 4);
        auto odd_sized = lz::repeat(1, 5);
        auto even_sized_even_take = lz::take_every(even_sized, 2);
        auto even_sized_odd_take = lz::take_every(even_sized, 3);
        auto uneven_sized_even_take = lz::take_every(odd_sized, 2);
        auto uneven_sized_odd_take = lz::take_every(odd_sized, 3);

        test_procs::test_operator_minus(even_sized_even_take);
        test_procs::test_operator_minus(even_sized_odd_take);
        test_procs::test_operator_minus(uneven_sized_even_take);
        test_procs::test_operator_minus(uneven_sized_odd_take);
    }

    SUBCASE("Operator+(default_sentinel_t)") {
        auto even_sized = lz::repeat(1, 4);
        auto odd_sized = lz::repeat(1, 5);
        auto even_sized_even_take = lz::take_every(even_sized, 2);
        auto even_sized_odd_take = lz::take_every(even_sized, 3);
        auto uneven_sized_even_take = lz::take_every(odd_sized, 2);
        auto uneven_sized_odd_take = lz::take_every(odd_sized, 3);

        std::vector<int> expected = { 1, 1 };
        test_procs::test_operator_plus(even_sized_even_take, expected);

        expected = { 1, 1 };
        test_procs::test_operator_plus(even_sized_odd_take, expected);

        expected = { 1, 1, 1 };
        test_procs::test_operator_plus(uneven_sized_even_take, expected);

        expected = { 1, 1 };
        test_procs::test_operator_plus(uneven_sized_odd_take, expected);
    }
}

TEST_CASE("take_every_iterable to containers") {
    constexpr std::size_t size = 4;
    std::array<int, size> array = { 1, 2, 3, 4 };
    constexpr std::size_t offset = 2;
    auto take_every = lz::take_every(array, offset);
    REQUIRE(take_every.size() == 2);

    SUBCASE("To array") {
        std::array<int, size / offset> actual = std::move(take_every) | lz::to<std::array<int, offset>>();
        REQUIRE(actual == std::array<int, offset>{ 1, 3 });
    }

    SUBCASE("To vector") {
        std::vector<int> actual = take_every | lz::to<std::vector>();
        REQUIRE(actual == std::vector<int>{ 1, 3 });
    }

    SUBCASE("To other container using to<>()") {
        std::list<int> actual = take_every | lz::to<std::list>();
        REQUIRE(actual == std::list<int>{ 1, 3 });
    }

    SUBCASE("To map") {
        auto actual = take_every | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();
        std::map<int, int> expected = { std::make_pair(1, 1), std::make_pair(3, 3) };
        REQUIRE(actual == expected);
    }

    SUBCASE("To unordered map") {
        auto actual =
            take_every | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        std::unordered_map<int, int> expected = { std::make_pair(1, 1), std::make_pair(3, 3) };
        REQUIRE(actual == expected);
    }
}

TEST_CASE("Take every with start offset") {
    SUBCASE("Start offset 1") {
        std::array<int, 7> arr = { 1, 2, 3, 4, 5, 6, 7 };
        auto take_every = arr | lz::take_every(2, 3);
        REQUIRE(take_every.size() == 2);
        REQUIRE(*take_every.begin() == 4);
        REQUIRE(*(take_every.begin() + 1) == 6);
        REQUIRE((take_every.begin() + 2) == take_every.end());
    }

    SUBCASE("Start offset 2") {
        std::list<int> lst = { 1, 2, 3 };
        auto take_every2 = lst | lz::take_every(2, 1);
        REQUIRE(take_every2.size() == 1);
        REQUIRE(*take_every2.begin() == 2);
        REQUIRE(std::next(take_every2.begin()) == take_every2.end());
    }

    SUBCASE("Start offset 3") {
        std::array<int, 6> arr2 = { 1, 2, 3, 4, 5, 6 };
        auto take_every3 = arr2 | lz::take_every(2, 3);
        REQUIRE(take_every3.size() == 2);
        REQUIRE(*take_every3.begin() == 4);
        REQUIRE(*(take_every3.begin() + 1) == 6);
        REQUIRE((take_every3.begin() + 2) == take_every3.end());
    }

    SUBCASE("Start offset with bidirectional sized iterable where start > size / 2") {
        std::list<int> lst2 = { 1, 2, 3, 4, 5, 6 };
        auto take_every4 = lst2 | lz::take_every(2, 4);
        REQUIRE(take_every4.size() == 1);
        REQUIRE(*take_every4.begin() == 5);
        REQUIRE(std::next(take_every4.begin()) == take_every4.end());
    }

    SUBCASE("Start offset with bidirectional non sized iterable") {
        std::list<int> lst2 = { 1, 2, 3, 4, 5, 6 };
        auto take_every4 = lst2 | lz::filter([](int) { return true; }) | lz::take_every(2, 4);
        REQUIRE(*take_every4.begin() == 5);
        REQUIRE(std::next(take_every4.begin()) == take_every4.end());
    }
}
