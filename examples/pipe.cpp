#include <Lz/lz.hpp>

int main() {
    int arr[]{ 3, 2, 4, 5 };
    int arr2[]{ 3, 2, 5, 5 };
    // the example below doesn't do anything special in particular, but should give an example on how to use chaining

#ifndef LZ_HAS_CXX_11

    // clang-format off
    const auto iterable = arr
        // take all elements
        | lz::take(std::distance(std::begin(arr), std::end(arr)))
        // drop the first 0 elements
        | lz::drop(0)
        // add 1 to each
        | lz::map([](int i) { return i + 1; })
        // cast it to int
        | lz::as<int>;
    // clang-format on

    const auto all_greater_five = lz::all_of(iterable, [](int i) { return i >= 5; });
    fmt::print("{}\n", all_greater_five); // prints false

#else

    // clang-format off
    const auto iterable = arr
        // take all elements
        | lz::take(std::distance(std::begin(arr), std::end(arr)))
        // drop the first 0 elements
        | lz::drop(0)
        // add 1 to each
        | lz::map([](int i) { return i + 1; })
        // cast it to int
        | lz::as<int>{};
    // clang-format on

    const auto all_greater_five = lz::all_of(iterable, [](int i) { return i >= 5; });
    fmt::print("{}\n", all_greater_five); // prints false

#endif
}