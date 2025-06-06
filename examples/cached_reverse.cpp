#include <Lz/filter.hpp>
#include <Lz/reverse.hpp>
#include <iostream>
#include <numeric>
#include <vector>

int main() {
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    auto reversed = lz::reverse(v);
    for (const auto& elem : reversed) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
    // Output: 5 4 3 2 1

    /**
    Reverses the iterable with a caching mechanism. Normally, the std::reverse_iterator uses an operator-- in its operator*
    to access the previous element, which can be inefficient for some iterables such as iterables that traverses multiple elements
    in its operator-- (such as `lz::filter` or `lz::take_every`). Say filter_iterator::operator-- traverses 10 elements to get
    to the previous element, then this means 20 elements will be traversed when operator-- and operator* are called in a row. This
    iterable prevents that by using a caching mechanism.
    */
    std::vector<int> large_vector(10000000);
    std::iota(large_vector.begin(), large_vector.end(), 0); // Fill with 0 to 9.999.999
    auto reversed_large = large_vector | lz::filter([](int i) { return i < 3; }) | lz::cached_reverse;
    for (const auto& elem : reversed_large) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
    // Output: 2 1 0 (only the first three elements, reversed)
}
