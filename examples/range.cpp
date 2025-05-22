#include <Lz/range.hpp>
#include <iostream>


int main() {
    for (int i : lz::range(3)) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 0 1 2

    std::cout << '\n';

    for (int i : lz::range(1, 4)) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 1 2 3

    std::cout << '\n';

    for (double i : lz::range(1.0, 4.0, 0.5)) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 1 1.5 2 2.5 3 3.5
}