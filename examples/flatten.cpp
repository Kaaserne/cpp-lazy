#include <Lz/flatten.hpp>
#include <iostream>
#include <vector>

int main() {
    std::vector<std::vector<std::vector<int>>> vectors = {
        { { 1,2, 3}, {}, {4} },
        {},
        {{ 5, 6 }, {7} , {}}
    };
    auto flattened = lz::flatten(vectors);

    for (int& i : flattened) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // prints 1 2 3 4 5 6 7

    std::cout << '\n';
    for (int& i : vectors | lz::flatten) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // prints 1 2 3 4 5 6 7
}