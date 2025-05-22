#include <Lz/concatenate.hpp>
#include <iostream>


int main() {
    std::array<int, 4> a{1, 2, 3, 4};
    std::array<int, 4> b{5, 6, 7, 8};

    const auto concat = lz::concat(a, b);
    for (int& i : concat) {
        std::cout << i << ' ';
        // or fmt::print("{} ", i);
    }
    // Output: 1 2 3 4 5 6 7 8
    std::cout << '\n';

    std::array<int, 4> c{9, 10, 11, 12};
    std::array<int, 4> d{13, 14, 15, 16};

    const auto concat2 = a | lz::concat(b, c, d);
    for (int& i : concat2) {
        std::cout << i << ' ';
        // or fmt::print("{} ", i);
    }
    // Output: 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
}