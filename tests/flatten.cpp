#include <Lz/filter.hpp>
#include <Lz/flatten.hpp>
#include <Lz/generate.hpp>
#include <Lz/map.hpp>
#include <Lz/range.hpp>
#include <Lz/repeat.hpp>
#include <Lz/reverse.hpp>
#include <cpp-lazy-ut-helper/c_string.hpp>
#include <cpp-lazy-ut-helper/test_procs.hpp>
#include <doctest/doctest.h>
#include <forward_list>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>

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

    std::array<std::array<decltype(lz::c_string("")), 1>, 1> arr_of_arr_of_cstr = { { { lz::c_string("") } } };
    static_assert(!lz::detail::all_sized<decltype(arr_of_arr_of_cstr)>::value, "Array of array of c_string should be sized");
}

TEST_CASE("Flatten with sentinels") {
    using c_string = decltype(lz::c_string(""));

    std::forward_list<c_string> lst = { lz::c_string("Hello"), lz::c_string(", "), lz::c_string("World"), lz::c_string("!") };
    lz::flatten_iterable<decltype(lst)> flattened = lz::flatten(lst);
    static_assert(lz::detail::is_fwd<decltype(flattened.begin())>::value, "Flattened should be fwd");

    SUBCASE("Operator= 2D forward") {
        auto it = flattened.begin();
        REQUIRE(it == flattened.begin());
        it = flattened.end();
        REQUIRE(it == flattened.end());
    }

    auto str = flattened | lz::to<std::string>();
    REQUIRE(str == "Hello, World!");

    auto flatten_one = lz::flatten(lz::c_string("hello, world"));
    REQUIRE((flatten_one | lz::to<std::string>()) == "hello, world");

    SUBCASE("Operator= 1D") {
        auto it = flatten_one.begin();
        REQUIRE(it == flatten_one.begin());
        it = flatten_one.end();
        REQUIRE(it == flatten_one.end());
    }

    c_string arr[] = { lz::c_string("Hello"), lz::c_string(", "), lz::c_string("World"), lz::c_string("!") };
    auto flattened_array = lz::flatten(arr);
    static_assert(lz::detail::is_fwd<decltype(flattened_array.begin())>::value, "Flattened should be fwd");

    SUBCASE("Operator= 2D random access") {
        auto it = flattened_array.begin();
        REQUIRE(it == flattened_array.begin());
        it = flattened_array.end();
        REQUIRE(it == flattened_array.end());
    }

    REQUIRE((flattened_array | lz::to<std::string>()) == "Hello, World!");

    std::forward_list<std::vector<int>> vec = { { 1, 2, 3 }, { 4, 5 }, { 6, 7 } };
    auto flattened_lst = lz::flatten(vec);
    static_assert(lz::detail::is_fwd<decltype(flattened_lst.begin())>::value, "Flattened should be fwd");
    auto expected = std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 };
    REQUIRE(lz::equal(flattened_lst, expected));

    SUBCASE("Operator= 2D fwd with std vector") {
        auto it = flattened_lst.begin();
        REQUIRE(it == flattened_lst.begin());
        it = flattened_lst.end();
        REQUIRE(it == flattened_array.end());
    }
}

TEST_CASE("Empty or one element flatten") {
    SUBCASE("Empty 1D") {
        std::vector<std::vector<int>> empty;
        auto empty_flattened = lz::flatten(empty);
        REQUIRE(lz::empty(empty_flattened));
        REQUIRE_FALSE(lz::has_one(empty_flattened));
        REQUIRE_FALSE(lz::has_many(empty_flattened));
        REQUIRE(empty_flattened.size() == 0);
    }

    SUBCASE("Empty 2D") {
        std::vector<std::vector<int>> empty;
        auto empty_flattened = lz::flatten(empty);
        REQUIRE(lz::empty(empty_flattened));
        REQUIRE_FALSE(lz::has_one(empty_flattened));
        REQUIRE_FALSE(lz::has_many(empty_flattened));
        REQUIRE(empty_flattened.size() == 0);
    }

    SUBCASE("Empty 3D") {
        std::vector<std::vector<std::vector<int>>> empty;
        auto empty_flattened = lz::flatten(empty);
        REQUIRE(lz::empty(empty_flattened));
        REQUIRE_FALSE(lz::has_one(empty_flattened));
        REQUIRE_FALSE(lz::has_many(empty_flattened));
        REQUIRE(empty_flattened.size() == 0);
    }

    SUBCASE("Empty 4D") {
        std::vector<std::vector<std::vector<std::vector<int>>>> empty;
        auto empty_flattened = lz::flatten(empty);
        REQUIRE(lz::empty(empty_flattened));
        REQUIRE_FALSE(lz::has_one(empty_flattened));
        REQUIRE_FALSE(lz::has_many(empty_flattened));
        REQUIRE(empty_flattened.size() == 0);
    }

    SUBCASE("One element 1D") {
        std::vector<int> one_element = { 1 };
        auto one_elm_flattened = lz::flatten(one_element);
        REQUIRE_FALSE(lz::empty(one_elm_flattened));
        REQUIRE(lz::has_one(one_elm_flattened));
        REQUIRE_FALSE(lz::has_many(one_elm_flattened));
        REQUIRE(one_elm_flattened.size() == 1);
    }

    SUBCASE("One element 2D") {
        std::vector<std::vector<int>> one_element = { { 1 } };
        auto one_elm_flattened = lz::flatten(one_element);
        REQUIRE_FALSE(lz::empty(one_elm_flattened));
        REQUIRE(lz::has_one(one_elm_flattened));
        REQUIRE_FALSE(lz::has_many(one_elm_flattened));
        REQUIRE(one_elm_flattened.size() == 1);
    }

    SUBCASE("One element 3D") {
        std::vector<std::vector<std::vector<int>>> one_element = { { { 1 } } };
        auto one_elm_flattened = lz::flatten(one_element);
        REQUIRE_FALSE(lz::empty(one_elm_flattened));
        REQUIRE(lz::has_one(one_elm_flattened));
        REQUIRE_FALSE(lz::has_many(one_elm_flattened));
        REQUIRE(one_elm_flattened.size() == 1);
    }

    SUBCASE("One element 4D") {
        std::vector<std::vector<std::vector<std::vector<int>>>> one_element = { { { { 1 } } } };
        auto one_elm_flattened = lz::flatten(one_element);
        REQUIRE_FALSE(lz::empty(one_elm_flattened));
        REQUIRE(lz::has_one(one_elm_flattened));
        REQUIRE_FALSE(lz::has_many(one_elm_flattened));
        REQUIRE(one_elm_flattened.size() == 1);
    }
}

namespace {

template<class FlattenIterable, class ExpectedIterable>
void test_flatten_operators_mm_and_pp(const FlattenIterable& flattened, const ExpectedIterable& expected) {
    REQUIRE(flattened.size() == lz::size(expected));
    REQUIRE(lz::equal(flattened, expected));
    REQUIRE(lz::equal(lz::reverse(flattened), lz::reverse(expected)));
}

} // namespace

TEST_CASE("Should flatten permutations") {
    SUBCASE("Flatten 1D") {
        std::vector<int> vec = { 1, 2, 3, 4 };
        auto flattened = lz::flatten(vec);
        REQUIRE((flattened | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 4 });
        REQUIRE((lz::reverse(flattened) | lz::to<std::vector>()) == std::vector<int>{ 4, 3, 2, 1 });
        REQUIRE(flattened.size() == 4);
    }

    SUBCASE("Flatten 2D") {
        std::vector<int> expected = { 1, 2, 3, 4, 5, 6, 7 };

        std::vector<std::vector<int>> vec = { { 1, 2, 3 }, { 4, 5 }, { 6, 7 } };
        auto f = lz::flatten(vec);

        test_flatten_operators_mm_and_pp(f, expected);
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);

        vec = { { 1, 2, 3 }, {}, { 4, 5 }, { 6, 7 } };
        test_flatten_operators_mm_and_pp(f, expected);
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);

        vec = { { 1, 2, 3 }, { 4, 5 }, {}, { 6, 7 } };
        test_flatten_operators_mm_and_pp(f, expected);
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);

        vec = { { 1, 2, 3 }, { 4, 5 }, { 6, 7 }, {} };
        test_flatten_operators_mm_and_pp(f, expected);
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);

        vec = { { 1, 2, 3 }, {}, {}, { 4, 5, 6, 7 } };
        test_flatten_operators_mm_and_pp(f, expected);
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);

        vec = { {}, { 1, 2, 3 }, { 4, 5, 6, 7 }, {} };
        test_flatten_operators_mm_and_pp(f, expected);
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);

        vec = { {}, {}, {}, { 1, 2, 3, 4, 5, 6, 7 } };
        test_flatten_operators_mm_and_pp(f, expected);
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);

        vec = { { 1, 2, 3, 4, 5, 6, 7 } };
        test_flatten_operators_mm_and_pp(f, expected);
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);

        vec = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 } };
        test_flatten_operators_mm_and_pp(f, expected);
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);

        vec = { {}, {}, { 1, 2, 3 }, {}, {}, { 4, 5, 6, 7 }, {}, {} };
        test_flatten_operators_mm_and_pp(f, expected);
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);
    }

    SUBCASE("Flatten 3D") {
        std::vector<int> expected = { 1, 4, 5, 6, 7, 8, 9 };

        std::vector<std::vector<std::vector<int>>> vec = { { { 1 }, { 4, 5 } }, { { 6, 7, 8, 9 } } };
        auto f = lz::flatten(vec);
        test_procs::test_operator_plus(f, expected);
        test_flatten_operators_mm_and_pp(f, expected);
        test_procs::test_operator_minus(f);

        vec = { { { 1 } }, { { 4 } }, { { 5 } }, { { 6 } }, { { 7 } }, { { 8 } }, { { 9 } } };
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);
        test_flatten_operators_mm_and_pp(f, expected);

        vec = { { { 1 }, {}, { 4, 5 } }, { {}, { 6, 7, 8, 9 } } };
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);
        test_flatten_operators_mm_and_pp(f, expected);

        vec = { { { 1 }, {}, { 4, 5 } }, { {}, {}, { 6, 7, 8, 9 } } };
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);
        test_flatten_operators_mm_and_pp(f, expected);

        vec = { { {}, {}, { 1 }, { 4, 5 } }, { {}, { 6, 7, 8, 9 } } };
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);
        test_flatten_operators_mm_and_pp(f, expected);

        vec = { { {}, {}, { 1 }, { 4, 5 } }, { {}, {}, { 6, 7, 8, 9 } } };
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);
        test_flatten_operators_mm_and_pp(f, expected);

        vec = { { {}, {}, { 1 }, { 4, 5 } }, { {}, {}, { 6, 7, 8, 9 }, {}, {} }, {}, {} };
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);
        test_flatten_operators_mm_and_pp(f, expected);
    }

    SUBCASE("Flatten 4D") {
        std::vector<int> expected = { 1, 4, 5, 6, 7, 8, 9 };

        std::vector<std::vector<std::vector<std::vector<int>>>> vec = { { { { 1 }, { 4, 5 } } }, { { { 6, 7, 8, 9 } } } };
        auto f = lz::flatten(vec);
        test_procs::test_operator_plus(f, expected);
        test_flatten_operators_mm_and_pp(f, expected);
        test_procs::test_operator_minus(f);

        vec = { { { { 1 } }, { { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 } } } };
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);
        test_flatten_operators_mm_and_pp(f, expected);

        vec = { { { { 1 }, {}, { 4, 5 } }, { {}, { 6, 7, 8, 9 } } } };
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);
        test_flatten_operators_mm_and_pp(f, expected);

        vec = { { { {}, {}, { 1 }, { 4, 5 } }, { {}, { 6, 7, 8, 9 } } } };
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);
        test_flatten_operators_mm_and_pp(f, expected);

        vec = { { { {}, {}, { 1 }, { 4, 5 } }, { {}, {}, { 6, 7, 8, 9 } } } };
        test_procs::test_operator_plus(f, expected);
        test_procs::test_operator_minus(f);
        test_flatten_operators_mm_and_pp(f, expected);
    }

    SUBCASE("Flatten with 1D sentinels") {
        auto f = lz::flatten(lz::repeat(1, 10));
        test_procs::test_operator_minus(f);
        f = lz::flatten(lz::repeat(1, 0));
        test_procs::test_operator_minus(f);
    }

    SUBCASE("Flatten with 2D sentinels") {
        auto f = lz::flatten(lz::repeat(lz::repeat(5, 3), 3));
        test_procs::test_operator_minus(f);
        f = lz::flatten(lz::repeat(lz::repeat(5, 3), 0));
        test_procs::test_operator_minus(f);
        f = lz::flatten(lz::repeat(lz::repeat(5, 0), 3));
    }

    SUBCASE("Flatten with 3D sentinels") {
        auto f = lz::flatten(lz::repeat(lz::repeat(lz::repeat(5, 3), 3), 3));
        test_procs::test_operator_minus(f);
        f = lz::flatten(lz::repeat(lz::repeat(lz::repeat(5, 3), 3), 0));
        test_procs::test_operator_minus(f);
        f = lz::flatten(lz::repeat(lz::repeat(lz::repeat(5, 3), 0), 3));
        test_procs::test_operator_minus(f);
        f = lz::flatten(lz::repeat(lz::repeat(lz::repeat(5, 0), 3), 3));
        test_procs::test_operator_minus(f);
    }

    SUBCASE("Flatten with 4D sentinels") {
        auto f = lz::flatten(lz::repeat(lz::repeat(lz::repeat(lz::repeat(5, 2), 2), 1), 2));
        test_procs::test_operator_minus(f);
        f = lz::flatten(lz::repeat(lz::repeat(lz::repeat(lz::repeat(5, 2), 2), 2), 0));
        test_procs::test_operator_minus(f);
        f = lz::flatten(lz::repeat(lz::repeat(lz::repeat(lz::repeat(5, 2), 2), 0), 2));
        test_procs::test_operator_minus(f);
        f = lz::flatten(lz::repeat(lz::repeat(lz::repeat(lz::repeat(5, 0), 0), 2), 2));
        test_procs::test_operator_minus(f);
        f = lz::flatten(lz::repeat(lz::repeat(lz::repeat(lz::repeat(5, 0), 2), 2), 2));
        test_procs::test_operator_minus(f);
    }

    SUBCASE("Should be by ref") {
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

    SUBCASE("To array") {
        REQUIRE((flattened | lz::to<std::array<int, 7>>()) == std::array<int, 7>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SUBCASE("To vector") {
        REQUIRE((flattened | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SUBCASE("To other container using to<>()") {
        REQUIRE((flattened | lz::to<std::list<int>>()) == std::list<int>{ 1, 2, 3, 4, 5, 6, 7 });
    }

    SUBCASE("To map") {
        auto actual = flattened | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();

        std::map<int, int> expected = {
            std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3), std::make_pair(4, 4),
            std::make_pair(5, 5), std::make_pair(6, 6), std::make_pair(7, 7),
        };

        REQUIRE(expected == actual);
    }

    SUBCASE("To unordered map") {
        auto actual =
            flattened | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::unordered_map<int, int>>();

        std::unordered_map<int, int> expected = {
            std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3), std::make_pair(4, 4),
            std::make_pair(5, 5), std::make_pair(6, 6), std::make_pair(7, 7),
        };

        REQUIRE(expected == actual);
    }
}

TEST_CASE("Stack allocated flatten") {
    std::array<std::array<int, 8>, 4> a{};
    int n = 0;
    std::generate(a.begin(), a.end(), [&n]() mutable {
        std::array<int, 8> arr{};
        std::generate(arr.begin(), arr.end(), [&n]() mutable { return n++; });
        return arr;
    });

    const auto expected = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
                            16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
    auto f = lz::flatten(a);
    test_procs::test_operator_plus(f, expected);
    test_procs::test_operator_minus(f);
    test_flatten_operators_mm_and_pp(f, expected);
}
