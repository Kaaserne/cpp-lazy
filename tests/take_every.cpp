#include <Lz/c_string.hpp>
#include <Lz/map.hpp>
#include <Lz/take_every.hpp>
#include <array>
#include <catch2/catch.hpp>
#include <forward_list>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("take_every_iterable with sentinels") {
    auto cstr = lz::c_string("Hello");
    auto take_every = lz::take_every(cstr, 2);
    static_assert(!std::is_same<decltype(take_every.begin()), decltype(take_every.end())>::value, "Should be sentinel");
    auto expected = lz::c_string("Hlo");
    REQUIRE(lz::equal(take_every, expected));
}

TEST_CASE("take_every_iterable changing and creating elements") {
    constexpr std::size_t size = 4;
    std::array<int, size> array = { 1, 2, 3, 4 };
    auto take_every = array | lz::take_every(2);
    auto iterator = take_every.begin();

    SECTION("take_every_iterable should be by reference") {
        *iterator = 0;
        REQUIRE(array[0] == 0);
    }

    SECTION("take_every_iterable should select every amount-th") {
        REQUIRE(*iterator == 1);
        ++iterator;
        REQUIRE(*iterator == 3);
        ++iterator;
        REQUIRE(iterator == take_every.end());
    }
}

TEST_CASE("Empty or one element take every") {
    SECTION("Empty 1") {
        std::vector<int> vec;
        auto take_every = lz::take_every(vec, 2);
        REQUIRE(take_every.size() == 0);
        REQUIRE(lz::empty(take_every));
        REQUIRE(!lz::has_many(take_every));
        REQUIRE(!lz::has_one(take_every));
    }

    SECTION("Empty 2") {
        std::vector<int> vec;
        auto take_every = lz::take_every(vec, 2, 3);
        REQUIRE(take_every.size() == 0);
        REQUIRE(lz::empty(take_every));
        REQUIRE(!lz::has_many(take_every));
        REQUIRE(!lz::has_one(take_every));
    }

    SECTION("Empty 3") {
        std::list<int> vec;
        auto take_every = lz::take_every(vec, 2, 3);
        REQUIRE(take_every.size() == 0);
        REQUIRE(lz::empty(take_every));
        REQUIRE(!lz::has_many(take_every));
        REQUIRE(!lz::has_one(take_every));
    }

    SECTION("Empty 4") {
        std::list<int> vec;
        auto take_every = lz::take_every(vec, 2);
        REQUIRE(take_every.size() == 0);
        REQUIRE(lz::empty(take_every));
        REQUIRE(!lz::has_many(take_every));
        REQUIRE(!lz::has_one(take_every));
    }

    SECTION("One element") {
        std::vector<int> vec = { 1 };
        auto take_every = lz::take_every(vec, 2);
        REQUIRE(take_every.size() == 1);
        REQUIRE(!lz::empty(take_every));
        REQUIRE(!lz::has_many(take_every));
        REQUIRE(lz::has_one(take_every));
    }

    SECTION("One element 2") {
        std::vector<int> vec = { 1 };
        auto take_every = lz::take_every(vec, 2, 3);
        REQUIRE(take_every.size() == 1);
        REQUIRE(lz::empty(take_every));
        REQUIRE(!lz::has_many(take_every));
        REQUIRE(!lz::has_one(take_every));
    }

    SECTION("One element 3") {
        std::list<int> vec = { 1 };
        auto take_every = lz::take_every(vec, 2);
        REQUIRE(take_every.size() == 1);
        REQUIRE(!lz::empty(take_every));
        REQUIRE(!lz::has_many(take_every));
        REQUIRE(lz::has_one(take_every));
    }

    SECTION("One element 4") {
        std::list<int> vec = { 1 };
        auto take_every = lz::take_every(vec, 2, 3);
        REQUIRE(take_every.size() == 1);
        REQUIRE(lz::empty(take_every));
        REQUIRE(!lz::has_many(take_every));
        REQUIRE(!lz::has_one(take_every));
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

    auto it = even_sized_even_take.begin();
    auto it2 = even_sized_odd_take.begin();
    auto it3 = uneven_sized_even_take.begin();
    auto it4 = uneven_sized_odd_take.begin();

    REQUIRE(*it == 1);
    REQUIRE(*it2 == 1);
    REQUIRE(*it3 == 1);
    REQUIRE(*it4 == 1);
    ++it;
    ++it2;
    ++it3;
    ++it4;
    REQUIRE(*it == 3);
    REQUIRE(*it2 == 4);
    REQUIRE(*it3 == 3);
    REQUIRE(*it4 == 4);
    ++it;
    ++it2;
    ++it3;
    ++it4;
    REQUIRE(it == even_sized_even_take.end());
    REQUIRE(it2 == even_sized_odd_take.end());
    REQUIRE(*it3 == 5);
    REQUIRE(it4 == uneven_sized_odd_take.end());
    ++it3;
    REQUIRE(it3 == uneven_sized_even_take.end());
}

template<class Container1, class Container2>
void operator_mm_test(const Container1 even_sized, const Container2& uneven_sized) {
    INFO("Operator--");
    LZ_ASSERT(even_sized.size() == 4, "Must be size 4");
    LZ_ASSERT(uneven_sized.size() == 5, "Must be size 5");

    // even sized = { 1, 2, 3, 4 }
    // uneven sized = { 1, 2, 3, 4, 5 }
    auto even_sized_even_take = lz::take_every(even_sized, 2);
    auto even_sized_odd_take = lz::take_every(even_sized, 3);
    auto uneven_sized_even_take = lz::take_every(uneven_sized, 2);
    auto uneven_sized_odd_take = lz::take_every(uneven_sized, 3);

    auto it = even_sized_even_take.end();    // {3, 1}
    auto it2 = even_sized_odd_take.end();    // {4, 1}
    auto it3 = uneven_sized_even_take.end(); // {5, 3, 1}
    auto it4 = uneven_sized_odd_take.end();  // {4, 1}

    --it;
    --it2;
    --it3;
    --it4;
    REQUIRE(*it == 3);
    REQUIRE(*it2 == 4);
    REQUIRE(*it3 == 5);
    REQUIRE(*it4 == 4);
    --it;
    --it2;
    --it3;
    --it4;
    REQUIRE(*it == 1);
    REQUIRE(*it2 == 1);
    REQUIRE(*it3 == 3);
    REQUIRE(*it4 == 1);
    REQUIRE(it == even_sized_even_take.begin());
    REQUIRE(it2 == even_sized_odd_take.begin());
    REQUIRE(it4 == uneven_sized_odd_take.begin());
    --it3;
    REQUIRE(*it3 == 1);
    REQUIRE(it3 == uneven_sized_even_take.begin());
}

TEST_CASE("take_every_iterable binary operations") {
    SECTION("Operator++") {
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

    SECTION("Operator--") {
        std::array<int, 4> even_sized = { 1, 2, 3, 4 };
        std::array<int, 5> odd_sized = { 1, 2, 3, 4, 5 };
        operator_mm_test(even_sized, odd_sized);

        std::list<int> even_sized_list = { 1, 2, 3, 4 };
        std::list<int> odd_sized_list = { 1, 2, 3, 4, 5 };
        operator_mm_test(even_sized_list, odd_sized_list);
    }

    SECTION("Operator+") {
        std::vector<int> even_sized = { 1, 2, 3, 4 };
        std::vector<int> odd_sized = { 1, 2, 3, 4, 5 };
        auto even_sized_even_take = lz::take_every(even_sized, 2);
        auto even_sized_odd_take = lz::take_every(even_sized, 3);
        auto uneven_sized_even_take = lz::take_every(odd_sized, 2);
        auto uneven_sized_odd_take = lz::take_every(odd_sized, 3);

        auto test_operator_plus = [](const std::initializer_list<int> expected, decltype(even_sized_even_take) input) {
            auto it = input.begin();
            auto end = input.end();

            for (std::size_t i = 0; i < lz::size(input); ++i) {
                INFO("With i = " << i);
                REQUIRE(*(it + i) == *(expected.begin() + i));
            }

            REQUIRE(it + lz::size(input) == end);
            std::advance(it, lz::size(input));
            REQUIRE(it == end);

            for (std::size_t i = 1; i <= lz::size(input); ++i) {
                INFO("With i = " << i);
                REQUIRE(*(it - i) == *(expected.end() - i));
            }

            for (std::size_t i = 1; i <= lz::size(input); ++i) {
                INFO("With i = " << i);
                REQUIRE(*(end - i) == *(expected.end() - i));
            }

            REQUIRE(end - lz::size(input) == input.begin());
            std::advance(end, -static_cast<std::ptrdiff_t>(lz::size(input)));
            REQUIRE(end == input.begin());

            for (std::size_t i = 0; i < lz::size(input); ++i) {
                INFO("With i = " << i);
                REQUIRE(*(end + i) == *(expected.begin() + i));
            }
        };

        auto expected = { 1, 3 };
        test_operator_plus(expected, even_sized_even_take);
        expected = { 1, 4 };
        test_operator_plus(expected, even_sized_odd_take);
        expected = { 1, 3, 5 };
        test_operator_plus(expected, uneven_sized_even_take);
        expected = { 1, 4 };
        test_operator_plus(expected, uneven_sized_odd_take);
    }

    SECTION("Operator-") {
    }
}

TEST_CASE("take_every_iterable to containers") {
    constexpr std::size_t size = 4;
    std::array<int, size> array = { 1, 2, 3, 4 };
    constexpr std::size_t offset = 2;
    auto take_every = lz::take_every(array, offset);
    REQUIRE(take_every.size() == 2);

    SECTION("To array") {
        std::array<int, static_cast<std::size_t>(size / offset)> actual = std::move(take_every) | lz::to<std::array<int, offset>>();
        REQUIRE(actual == std::array<int, offset>{ 1, 3 });
    }

    SECTION("To vector") {
        std::vector<int> actual = take_every | lz::to<std::vector>();
        REQUIRE(actual == std::vector<int>{ 1, 3 });
    }

    SECTION("To other container using to<>()") {
        std::list<int> actual = take_every | lz::to<std::list>();
        REQUIRE(actual == std::list<int>{ 1, 3 });
    }

    SECTION("To map") {
        auto actual = take_every | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();
        std::map<int, int> expected = { std::make_pair(1, 1), std::make_pair(3, 3) };
        REQUIRE(actual == expected);
    }

    SECTION("To unordered map") {
        auto actual =
            take_every | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();
        std::unordered_map<int, int> expected = { std::make_pair(1, 1), std::make_pair(3, 3) };
        REQUIRE(actual == expected);
    }
}

TEST_CASE("Take every with start offset") {
    std::array<int, 7> arr = { 1, 2, 3, 4, 5, 6, 7 };
    auto take_every = arr | lz::take_every(2, 3);
    REQUIRE(take_every.size() == 2);
    REQUIRE(*take_every.begin() == 4);
    REQUIRE(*(take_every.begin() + 1) == 6);
    REQUIRE((take_every.begin() + 2) == take_every.end());

    std::list<int> lst = { 1, 2, 3 };
    auto take_every2 = lst | lz::take_every(2, 1);
    REQUIRE(take_every2.size() == 1);
    REQUIRE(*take_every2.begin() == 2);
    REQUIRE(std::next(take_every2.begin()) == take_every2.end());

    std::array<int, 6> arr2 = { 1, 2, 3, 4, 5, 6 };
    auto take_every3 = arr2 | lz::take_every(2, 3);
    REQUIRE(take_every3.size() == 2);
    REQUIRE(*take_every3.begin() == 4);
    REQUIRE(*(take_every3.begin() + 1) == 6);
    REQUIRE((take_every3.begin() + 2) == take_every3.end());
}
