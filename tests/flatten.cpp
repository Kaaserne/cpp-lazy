#include <Lz/c_string.hpp>
#include <Lz/filter.hpp>
#include <Lz/flatten.hpp>
#include <Lz/map.hpp>
#include <Lz/reverse.hpp>
#include <catch2/catch.hpp>
#include <forward_list>
#include <list>
#include <map>
#include <unordered_map>

TEST_CASE("Dimensions & sized") {
    int arr[3];
    static_assert(lz::dimensions<decltype(arr)>::value == 1, "Dimensions of array should be 1");
    int arr2[3][3];
    static_assert(lz::dimensions<decltype(arr2)>::value == 2, "Dimensions of array should be 2");
    const int arr3[3][3][3]{};
    static_assert(lz::dimensions<decltype(arr3)>::value == 3, "Dimensions of array should be 3");

    std::vector<int> vec;
    static_assert(lz::dimensions<decltype(vec)>::value == 1, "Dimensions of vector should be 1");
    std::vector<std::vector<int>> vec2;
    static_assert(lz::dimensions<decltype(vec2)>::value == 2, "Dimensions of vector should be 2");
    std::vector<std::vector<std::vector<int>>> vec3;
    static_assert(lz::dimensions<decltype(vec3)>::value == 3, "Dimensions of vector should be 3");

    std::array<int, 3> arr4;
    static_assert(lz::dimensions<decltype(arr4)>::value == 1, "Dimensions of array should be 1");
    std::array<std::array<int, 3>, 3> arr5;
    static_assert(lz::dimensions<decltype(arr5)>::value == 2, "Dimensions of array should be 2");
    const std::array<std::array<std::array<int, 3>, 3>, 3> arr6{};
    static_assert(lz::dimensions<decltype(arr6)>::value == 3, "Dimensions of array should be 3");

    auto str = lz::c_string("Hello, World!");
    static_assert(!lz::detail::all_sized<decltype(str)>::value, "c_string should not be sized");
    std::array<decltype(lz::c_string("")), 2> arr_of_cstr = { lz::c_string(""), lz::c_string("") };
    static_assert(!lz::detail::all_sized<decltype(arr_of_cstr)>::value, "Array of c_string should not all be sized");

    std::vector<int> v;
    auto filter = lz::filter(v, [](int) { return true; });
    static_assert(!lz::detail::all_sized<decltype(filter)>::value, "Filter should not be sized");

    std::array<std::array<decltype(lz::c_string("")), 1>, 1> arr_of_arr_of_cstr = { { lz::c_string("") } };
    static_assert(!lz::detail::all_sized<decltype(arr_of_arr_of_cstr)>::value, "Array of array of c_string should be sized");
}

TEST_CASE("Flatten with sentinels") {
    using c_string = decltype(lz::c_string(""));

    std::forward_list<c_string> lst = { lz::c_string("Hello"), lz::c_string(", "), lz::c_string("World"), lz::c_string("!") };
    auto flattened = lz::flatten(lst);
    static_assert(lz::detail::is_fwd<decltype(flattened.begin())>::value, "Flattened should be fwd");

    auto str = flattened | lz::to<std::string>();
    REQUIRE(str == "Hello, World!");

    auto flatten_one = lz::flatten(lz::c_string("hello, world"));
    REQUIRE((flatten_one | lz::to<std::string>()) == "hello, world");

    c_string arr[] = { lz::c_string("Hello"), lz::c_string(", "), lz::c_string("World"), lz::c_string("!") };
    auto flattened_array = lz::flatten(arr);
    static_assert(lz::detail::is_fwd<decltype(flattened_array.begin())>::value, "Flattened should be fwd");

    REQUIRE((flattened_array | lz::to<std::string>()) == "Hello, World!");

    std::forward_list<std::vector<int>> vec = { { 1, 2, 3 }, { 4, 5 }, { 6, 7 } };
    auto flattened_lst = lz::flatten(vec);
    static_assert(lz::detail::is_fwd<decltype(flattened_lst.begin())>::value, "Flattened should be fwd");
    auto expected = std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 };
    REQUIRE(lz::equal(flattened_lst, expected));
}

TEST_CASE("Empty or one element flatten") {
    SECTION("Empty 1D") {
        std::vector<std::vector<int>> empty;
        auto empty_flattened = lz::flatten(empty);
        REQUIRE(lz::empty(empty_flattened));
        REQUIRE(!lz::has_one(empty_flattened));
        REQUIRE(!lz::has_many(empty_flattened));
        REQUIRE(empty_flattened.size() == 0);
    }

    SECTION("Empty 2D") {
        std::vector<std::vector<int>> empty;
        auto empty_flattened = lz::flatten(empty);
        REQUIRE(lz::empty(empty_flattened));
        REQUIRE(!lz::has_one(empty_flattened));
        REQUIRE(!lz::has_many(empty_flattened));
        REQUIRE(empty_flattened.size() == 0);
    }

    SECTION("Empty 3D") {
        std::vector<std::vector<std::vector<int>>> empty;
        auto empty_flattened = lz::flatten(empty);
        REQUIRE(lz::empty(empty_flattened));
        REQUIRE(!lz::has_one(empty_flattened));
        REQUIRE(!lz::has_many(empty_flattened));
        REQUIRE(empty_flattened.size() == 0);
    }

    SECTION("Empty 4D") {
        std::vector<std::vector<std::vector<std::vector<int>>>> empty;
        auto empty_flattened = lz::flatten(empty);
        REQUIRE(lz::empty(empty_flattened));
        REQUIRE(!lz::has_one(empty_flattened));
        REQUIRE(!lz::has_many(empty_flattened));
        REQUIRE(empty_flattened.size() == 0);
    }

    SECTION("One element 1D") {
        std::vector<int> one_element = { 1 };
        auto one_elm_flattened = lz::flatten(one_element);
        REQUIRE(!lz::empty(one_elm_flattened));
        REQUIRE(lz::has_one(one_elm_flattened));
        REQUIRE(!lz::has_many(one_elm_flattened));
        REQUIRE(one_elm_flattened.size() == 1);
    }

    SECTION("One element 2D") {
        std::vector<std::vector<int>> one_element = { { 1 } };
        auto one_elm_flattened = lz::flatten(one_element);
        REQUIRE(!lz::empty(one_elm_flattened));
        REQUIRE(lz::has_one(one_elm_flattened));
        REQUIRE(!lz::has_many(one_elm_flattened));
        REQUIRE(one_elm_flattened.size() == 1);
    }

    SECTION("One element 3D") {
        std::vector<std::vector<std::vector<int>>> one_element = { { { 1 } } };
        auto one_elm_flattened = lz::flatten(one_element);
        REQUIRE(!lz::empty(one_elm_flattened));
        REQUIRE(lz::has_one(one_elm_flattened));
        REQUIRE(!lz::has_many(one_elm_flattened));
        REQUIRE(one_elm_flattened.size() == 1);
    }

    SECTION("One element 4D") {
        std::vector<std::vector<std::vector<std::vector<int>>>> one_element = { { { { 1 } } } };
        auto one_elm_flattened = lz::flatten(one_element);
        REQUIRE(!lz::empty(one_elm_flattened));
        REQUIRE(lz::has_one(one_elm_flattened));
        REQUIRE(!lz::has_many(one_elm_flattened));
        REQUIRE(one_elm_flattened.size() == 1);
    }
}

template<class Vector, class ExpectedIterable>
void test_flatten_operators_mm_and_pp(const Vector& vec, const ExpectedIterable& expected) {
    auto flattened = lz::flatten(vec);
    REQUIRE(flattened.size() == lz::size(expected));
    REQUIRE(lz::equal(flattened, expected));
    REQUIRE(lz::equal(lz::reverse(flattened), lz::reverse(expected)));
}

template<class Vector, class ExpectedIterable>
void test_operator_plus_is(const Vector& vec, const ExpectedIterable& expected) {
    auto flattened = lz::flatten(vec);
    auto begin = flattened.begin();
    auto end = flattened.end();

    REQUIRE(begin + 0 == begin);
    REQUIRE(end + 0 == end);

    for (std::size_t i = 0; i < lz::size(flattened) - 1; ++i) {
        INFO("With i = " << i);
        REQUIRE(*(begin + i) == *(expected.begin() + i));
    }
    REQUIRE(begin + lz::size(flattened) == flattened.end());
    for (std::size_t i = 1; i <= lz::size(flattened); ++i) {
        INFO("With i = " << i);
        REQUIRE(*(end - i) == *(expected.end() - i));
    }
    REQUIRE(end - lz::size(flattened) == flattened.begin());

    std::advance(begin, lz::size(flattened));
    std::advance(end, -static_cast<std::ptrdiff_t>(lz::size(flattened)));
    REQUIRE(begin + 0 == begin);
    REQUIRE(end + 0 == end);

    for (std::size_t i = 0; i < lz::size(flattened) - 1; ++i) {
        INFO("With i = " << i);
        REQUIRE(*(end + i) == *(expected.begin() + i));
    }
    REQUIRE(end + lz::size(flattened) == flattened.end());
    for (std::size_t i = 1; i <= lz::size(flattened); ++i) {
        INFO("With i = " << i);
        REQUIRE(*(begin - i) == *(expected.end() - i));
    }
    REQUIRE(begin - lz::size(flattened) == flattened.begin());
}

template<class Vector>
void test_operator_min(const Vector& vec) {
    auto flattened = lz::flatten(vec);

    auto begin = flattened.begin();
    auto end = flattened.end();

    for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(lz::size(flattened)); ++i) {
        INFO("With i = " << i);
        REQUIRE((end - i) - begin == static_cast<std::ptrdiff_t>(lz::size(flattened) - i));
        REQUIRE(end - (begin + i) == static_cast<std::ptrdiff_t>(lz::size(flattened) - i));
        REQUIRE((begin + i) - end == -static_cast<std::ptrdiff_t>(lz::size(flattened) - i));
        REQUIRE(begin - (end - i) == -static_cast<std::ptrdiff_t>(lz::size(flattened) - i));
    }

    for (std::size_t i = 0; i < lz::size(flattened); ++i) {
        INFO("With i = " << i);
        REQUIRE((end - i) - (begin + i) == static_cast<std::ptrdiff_t>(lz::size(flattened) - 2 * i));
        REQUIRE((begin + i) - (end - i) == -static_cast<std::ptrdiff_t>(lz::size(flattened) - 2 * i));
    }
}

TEST_CASE("Should flatten permutations") {
    SECTION("Flatten 1D") {
        std::vector<int> vec = { 1, 2, 3, 4 };
        auto flattened = lz::flatten(vec);
        REQUIRE((flattened | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 4 });
        REQUIRE((lz::reverse(flattened) | lz::to<std::vector>()) == std::vector<int>{ 4, 3, 2, 1 });
        REQUIRE(flattened.size() == 4);
    }

    SECTION("Flatten 2D") {
        std::vector<int> expected = { 1, 2, 3, 4, 5, 6, 7 };

        std::vector<std::vector<int>> vec = { { 1, 2, 3 }, { 4, 5 }, { 6, 7 } };
        test_flatten_operators_mm_and_pp(vec, expected);
        test_operator_plus_is(vec, expected);
        test_operator_min(vec);

        vec = { { 1, 2, 3 }, {}, { 4, 5 }, { 6, 7 } };
        test_flatten_operators_mm_and_pp(vec, expected);
        test_operator_plus_is(vec, expected);
        test_operator_min(vec);

        vec = { { 1, 2, 3 }, { 4, 5 }, {}, { 6, 7 } };
        test_flatten_operators_mm_and_pp(vec, expected);
        test_operator_plus_is(vec, expected);
        test_operator_min(vec);

        vec = { { 1, 2, 3 }, { 4, 5 }, { 6, 7 }, {} };
        test_flatten_operators_mm_and_pp(vec, expected);
        test_operator_plus_is(vec, expected);
        test_operator_min(vec);

        vec = { { 1, 2, 3 }, {}, {}, { 4, 5, 6, 7 } };
        test_flatten_operators_mm_and_pp(vec, expected);
        test_operator_plus_is(vec, expected);
        test_operator_min(vec);

        vec = { {}, { 1, 2, 3 }, { 4, 5, 6, 7 }, {} };
        test_flatten_operators_mm_and_pp(vec, expected);
        test_operator_plus_is(vec, expected);
        test_operator_min(vec);

        vec = { {}, {}, {}, { 1, 2, 3, 4, 5, 6, 7 } };
        test_flatten_operators_mm_and_pp(vec, expected);
        test_operator_plus_is(vec, expected);
        test_operator_min(vec);

        vec = { { 1, 2, 3, 4, 5, 6, 7 } };
        test_flatten_operators_mm_and_pp(vec, expected);
        test_operator_plus_is(vec, expected);
        test_operator_min(vec);

        vec = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 } };
        test_flatten_operators_mm_and_pp(vec, expected);
        test_operator_plus_is(vec, expected);
        test_operator_min(vec);

        vec = { {}, {}, { 1, 2, 3 }, {}, {}, { 4, 5, 6, 7 }, {}, {} };
        test_flatten_operators_mm_and_pp(vec, expected);
        test_operator_plus_is(vec, expected);
        test_operator_min(vec);
    }

    SECTION("Flatten 3D") {
        std::vector<int> expected = { 1, 4, 5, 6, 7, 8, 9 };

        std::vector<std::vector<std::vector<int>>> vec = { { { 1 }, { 4, 5 } }, { { 6, 7, 8, 9 } } };
        test_operator_plus_is(vec, expected);
        test_flatten_operators_mm_and_pp(vec, expected);
        test_operator_min(vec);

        vec = { { { 1 } }, { { 4 } }, { { 5 } }, { { 6 } }, { { 7 } }, { { 8 } }, { { 9 } } };
        test_operator_plus_is(vec, expected);
        test_operator_min(vec);

        vec = { { { 1 }, {}, { 4, 5 } }, { {}, { 6, 7, 8, 9 } } };
        test_operator_plus_is(vec, expected);
        test_operator_min(vec);

        vec = { { { 1 }, {}, { 4, 5 } }, { {}, {}, { 6, 7, 8, 9 } } };
        test_operator_plus_is(vec, expected);
        test_operator_min(vec);

        vec = { { {}, {}, { 1 }, { 4, 5 } }, { {}, { 6, 7, 8, 9 } } };
        test_operator_plus_is(vec, expected);
        test_operator_min(vec);

        vec = { { {}, {}, { 1 }, { 4, 5 } }, { {}, {}, { 6, 7, 8, 9 } } };
        test_operator_plus_is(vec, expected);

        vec = { { {}, {}, { 1 }, { 4, 5 } }, { {}, {}, { 6, 7, 8, 9 }, {}, {} }, {}, {} };
        test_operator_plus_is(vec, expected);
    }

    SECTION("Flatten 4D") {
        std::vector<int> expected = { 1, 4, 5, 6, 7, 8, 9 };

        std::vector<std::vector<std::vector<std::vector<int>>>> vec = { { { { 1 }, { 4, 5 } } }, { { { 6, 7, 8, 9 } } } };
        test_operator_plus_is(vec, expected);
        test_flatten_operators_mm_and_pp(vec, expected);
        test_operator_min(vec);
    }

    SECTION("Should be by ref") {
        std::vector<std::vector<std::vector<int>>> vectors = {
            { { 0 }, { 1, 2, 3 }, {}, { 4 } }, { {} }, { { 5, 6 }, { 7 }, {} }, { {} }, { {} }
        };
        auto flattened = lz::flatten(vectors);
        REQUIRE(flattened.size() == 8);
        *flattened.begin() = -382753;
        REQUIRE(vectors[0][0][0] == -382753);
    }
}

TEST_CASE("Flatten to container") {
    std::vector<std::vector<int>> vecs = { { 1, 2, 3 }, { 4, 5 }, {}, { 6, 7 } };
    auto flattened = lz::flatten(vecs);
    REQUIRE(flattened.size() == 7);

    SECTION("To array") {
        REQUIRE((flattened | lz::to<std::array<int, 7>>()) == std::array<int, 7>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To vector") {
        REQUIRE((flattened | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To other container using to<>()") {
        REQUIRE((flattened | lz::to<std::list<int>>()) == std::list<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SECTION("To map") {
        auto actual = flattened | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();

        std::map<int, int> expected = {
            std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3), std::make_pair(4, 4),
            std::make_pair(5, 5), std::make_pair(6, 6), std::make_pair(7, 7),
        };

        REQUIRE(expected == actual);
    }

    SECTION("To unordered map") {
        auto actual =
            flattened | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();

        std::unordered_map<int, int> expected = {
            std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3), std::make_pair(4, 4),
            std::make_pair(5, 5), std::make_pair(6, 6), std::make_pair(7, 7),
        };

        REQUIRE(expected == actual);
    }
}
