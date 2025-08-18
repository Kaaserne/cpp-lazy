#include <Lz/concatenate.hpp>
#include <Lz/range.hpp>
#include <array>
#include <iostream>
#include <vector>

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

    // And if one of the iterables yields elements by value, all elements will be yielded by value.
    std::vector<int> f{ 21, 22, 23, 24 }; // normally yields by reference
    auto range = lz::range(5);       // range yields by value
    for (int i : f | lz::concat(range)) { // so, the common reference type is int
        std::cout << i << ' ';
        // or fmt::print("{} ", i);
    }
    // Output: 21 22 23 24 0 1 2 3 4
    std::cout << '\n';
    // TODO add reference wrapper
}