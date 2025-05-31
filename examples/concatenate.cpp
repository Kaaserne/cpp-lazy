#include <Lz/concatenate.hpp>
#include <iostream>


int main() {
    std::array<int, 4> a{1, 2, 3, 4};
    std::array<int, 4> b{5, 6, 7, 8};

    // Because all containers are mutable, int& is used as the reference type.
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

    // However, if one of the containers is const, all references will be const.
    const std::array<int, 4> e{ 17, 18, 19, 20 };
    const auto concat3 = a | lz::concat(b, e);
    for (const int& i : concat3) {
        std::cout << i << ' ';
        // or fmt::print("{} ", i);
    }
    // Output: 1 2 3 4 5 6 7 8 17 18 19 20
    std::cout << '\n';
}