#include <Lz/Lz.hpp>
#include <functional>

int main() {
    int arr[]{ 3, 2, 4, 5 };
    int arr2[]{ 3, 2, 5, 5 };
    // TODO
    // the example below doesn't do anything special in particular, but should give an example on how to use chaining

    // clang-format off
    const auto is_all_int = arr
        // take all elements
        | lz::take(std::distance(std::begin(arr), std::end(arr)))
        // drop the first 0 elements
        | lz::drop(0)
        // add 1 to each
        | lz::map([](int i) { return i + 1; })
        // cast it to int
        | lz::as<int>()
        // every number should be an int
        | lz::all([](int i) { return std::is_same<decltype(i), int>::value; });
    // clang-format on

    fmt::print("{}\n", is_all_int); // prints true
}