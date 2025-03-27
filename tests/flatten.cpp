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

TEST_CASE("tmp") {
    SECTION("A") {
        std::vector<std::vector<int>> vec = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8 }, {}, {}, {}, {} };
        auto flattened = lz::flatten(vec);
        auto end = flattened.end();
        auto begin = flattened.begin();
        begin += 4;
        (void)begin;
        // auto size = end - begin;
        // --end;
        // size = end - begin;
        // ++begin;
        // size = end - begin;
        // --begin;
        // (void)size;

    }
}

// TEST_CASE("Dimensions & sized") {
//     int arr[3];
//     static_assert(lz::dimensions<decltype(arr)>::value == 1, "Dimensions of array should be 1");
//     int arr2[3][3];
//     static_assert(lz::dimensions<decltype(arr2)>::value == 2, "Dimensions of array should be 2");
//     const int arr3[3][3][3]{};
//     static_assert(lz::dimensions<decltype(arr3)>::value == 3, "Dimensions of array should be 3");

//     std::vector<int> vec;
//     static_assert(lz::dimensions<decltype(vec)>::value == 1, "Dimensions of vector should be 1");
//     std::vector<std::vector<int>> vec2;
//     static_assert(lz::dimensions<decltype(vec2)>::value == 2, "Dimensions of vector should be 2");
//     std::vector<std::vector<std::vector<int>>> vec3;
//     static_assert(lz::dimensions<decltype(vec3)>::value == 3, "Dimensions of vector should be 3");

//     std::array<int, 3> arr4;
//     static_assert(lz::dimensions<decltype(arr4)>::value == 1, "Dimensions of array should be 1");
//     std::array<std::array<int, 3>, 3> arr5;
//     static_assert(lz::dimensions<decltype(arr5)>::value == 2, "Dimensions of array should be 2");
//     const std::array<std::array<std::array<int, 3>, 3>, 3> arr6{};
//     static_assert(lz::dimensions<decltype(arr6)>::value == 3, "Dimensions of array should be 3");

//     auto str = lz::c_string("Hello, World!");
//     static_assert(!lz::detail::all_sized<decltype(str)>::value, "c_string should not be sized");
//     std::array<decltype(lz::c_string("")), 2> arr_of_cstr = { lz::c_string(""), lz::c_string("") };
//     static_assert(!lz::detail::all_sized<decltype(arr_of_cstr)>::value, "Array of c_string should not all be sized");

//     std::vector<int> v;
//     auto filter = lz::filter(v, [](int) { return true; });
//     static_assert(!lz::detail::all_sized<decltype(filter)>::value, "Filter should not be sized");

//     std::array<std::array<decltype(lz::c_string("")), 1>, 1> arr_of_arr_of_cstr = { { lz::c_string("") } };
//     static_assert(!lz::detail::all_sized<decltype(arr_of_arr_of_cstr)>::value, "Array of array of c_string should be sized");
// }

// TEST_CASE("Flatten with sentinels") {
//     using c_string = decltype(lz::c_string(""));

//     std::forward_list<c_string> lst = { lz::c_string("Hello"), lz::c_string(", "), lz::c_string("World"), lz::c_string("!") };
//     auto flattened = lz::flatten(lst);
//     auto str = flattened | lz::to<std::string>();
//     REQUIRE(str == "Hello, World!");

//     auto flatten_one = lz::flatten(lz::c_string("hello, world"));
//     REQUIRE((flatten_one | lz::to<std::string>()) == "hello, world");

//     c_string arr[] = { lz::c_string("Hello"), lz::c_string(", "), lz::c_string("World"), lz::c_string("!") };
//     auto flattened_array = lz::flatten(arr);
//     REQUIRE((flattened_array | lz::to<std::string>()) == "Hello, World!");
// }

// TEST_CASE("Empty or one element flatten") {
//     SECTION("Empty") {
//         std::vector<std::vector<int>> empty;
//         auto empty_flattened = lz::flatten(empty);
//         REQUIRE(lz::empty(empty_flattened));
//         REQUIRE(!lz::has_one(empty_flattened));
//         REQUIRE(!lz::has_many(empty_flattened));
//         REQUIRE(empty_flattened.size() == 0);
//     }

//     SECTION("One element") {
//         std::vector<std::vector<int>> one_element = { { 1 } };
//         auto one_elm_flattened = lz::flatten(one_element);
//         REQUIRE(!lz::empty(one_elm_flattened));
//         REQUIRE(lz::has_one(one_elm_flattened));
//         REQUIRE(!lz::has_many(one_elm_flattened));
//         REQUIRE(one_elm_flattened.size() == 1);
//     }
// }

// TEST_CASE("Should flatten") {
//     SECTION("Flatten 1D") {
//         std::vector<int> vec = { 1, 2, 3, 4 };
//         auto flattened = lz::flatten(vec);
//         REQUIRE((flattened | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 4 });
//         REQUIRE((lz::reverse(flattened) | lz::to<std::vector>()) == std::vector<int>{ 4, 3, 2, 1 });
//         REQUIRE(flattened.size() == 4);
//     }

//     SECTION("Flatten 2D") {
//         std::vector<std::list<int>> nested = { { 1, 2, 3 }, {}, { 1 }, { 4, 5, 6 }, {} };
//         auto flattened = lz::flatten(nested);
//         REQUIRE(flattened.size() == 7);
//         REQUIRE((flattened | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 1, 4, 5, 6 });
//         REQUIRE((lz::reverse(flattened) | lz::to<std::vector>()) == std::vector<int>{ 6, 5, 4, 1, 3, 2, 1 });
//     }

//     SECTION("Flatten 3D") {
//         std::vector<std::vector<std::vector<int>>> vectors = { { { 1, 2, 3 }, {} }, { { 4, 5 }, { 6 } }, { { 7 }, {} } };

//         auto flattened = lz::flatten(vectors);
//         REQUIRE(flattened.size() == 7);
//         REQUIRE((flattened | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 });
//         REQUIRE((lz::reverse(flattened) | lz::to<std::vector>()) == std::vector<int>{ 7, 6, 5, 4, 3, 2, 1 });
//     }

//     SECTION("Flatten 4D") {
//         std::vector<std::vector<std::vector<std::vector<int>>>> vectors = {
//             { { { 1, 2, 3 }, {} }, { { 4, 5 }, { 6 } }, { { 7 }, {} } },
//             { { { 8, 9 }, { 10 } }, { { 11 }, {} } },
//         };

//         auto flattened = lz::flatten(vectors);
//         REQUIRE(flattened.size() == 11);
//         REQUIRE((flattened | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
//         REQUIRE((lz::reverse(flattened) | lz::to<std::vector>()) == std::vector<int>{ 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 });
//     }

//     SECTION("Should be by ref") {
//         std::vector<std::vector<std::vector<int>>> vectors = {
//             { { 0 }, { 1, 2, 3 }, {}, { 4 } }, { {} }, { { 5, 6 }, { 7 }, {} }, { {} }, { {} }
//         };
//         auto flattened = lz::flatten(vectors);
//         REQUIRE(flattened.size() == 8);
//         *flattened.begin() = -382753;
//         REQUIRE(vectors[0][0][0] == -382753);
//     }
// }

// TEST_CASE("Flatten binary operations") {
//     std::vector<std::list<int>> nested = { { 1, 2, 3 }, {}, { 1 }, { 4, 5, 6 }, {} };
//     auto flattened = nested | lz::flatten;
//     REQUIRE(flattened.size() == 7);

//     SECTION("Operator++") {
//         auto begin = flattened.begin();
//         REQUIRE(*begin == 1);
//         ++begin;
//         REQUIRE(*begin == 2);
//         ++begin, ++begin;
//         REQUIRE(*begin == 1);
//     }

//     SECTION("Operator--") {
//         auto begin = flattened.begin();
//         ++begin;
//         REQUIRE(*begin == 2);
//         --begin;
//         REQUIRE(*begin == 1);
//         REQUIRE(begin == flattened.begin());
//         ++begin, ++begin, ++begin;
//         REQUIRE(*begin == 1);
//         --begin;
//         REQUIRE(*begin == 3);
//     }

//     SECTION("Operator== & operator!=") {
//         auto begin = flattened.begin();
//         REQUIRE(begin == flattened.begin());
//         REQUIRE(begin != flattened.end());
//         begin = flattened.end();
//         REQUIRE(begin == flattened.end());
//         REQUIRE(begin != flattened.begin());
//     }

//     // TODO?
//     SECTION("Operator-(Iterator)") {
//         // REQUIRE(flattened.end() - flattened.begin() == 7);
//     }
// }

// TEST_CASE("Flatten to container") {
//     std::vector<std::vector<int>> vecs = { { 1, 2, 3 }, { 4, 5 }, {}, { 6, 7 } };
//     auto flattened = lz::flatten(vecs);
//     REQUIRE(flattened.size() == 7);

//     SECTION("To array") {
//         REQUIRE((flattened | lz::to<std::array<int, 7>>()) == std::array<int, 7>{ 1, 2, 3, 4, 5, 6, 7 });
//     }

//     SECTION("To vector") {
//         REQUIRE((flattened | lz::to<std::vector>()) == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 });
//     }

//     SECTION("To other container using to<>()") {
//         REQUIRE((flattened | lz::to<std::list<int>>()) == std::list<int>{ 1, 2, 3, 4, 5, 6, 7 });
//     }

//     SECTION("To map") {
//         auto actual = flattened | lz::map([](const int i) { return std::make_pair(i, i); }) | lz::to<std::map<int, int>>();

//         std::map<int, int> expected = {
//             std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3), std::make_pair(4, 4),
//             std::make_pair(5, 5), std::make_pair(6, 6), std::make_pair(7, 7),
//         };

//         REQUIRE(expected == actual);
//     }

//     SECTION("To unordered map") {
//         auto actual = flattened | lz::map([](const int i) { return std::make_pair(i, i); }) |
//                       lz::to<std::unordered_map<int, int>>();

//         std::unordered_map<int, int> expected = {
//             std::make_pair(1, 1), std::make_pair(2, 2), std::make_pair(3, 3), std::make_pair(4, 4),
//             std::make_pair(5, 5), std::make_pair(6, 6), std::make_pair(7, 7),
//         };

//         REQUIRE(expected == actual);
//     }
// }