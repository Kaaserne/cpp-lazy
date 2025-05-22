#include <Lz/rotate.hpp>
#include <vector>
#include <iostream>

int main() {
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    for (int& i : lz::rotate(v, 2)) {
        std::cout << i << " ";
        // Or fmt::print("{} ", i);
    }
    // Output: 3 4 5 1 2

    std::cout << '\n';

    for (int& i : v | lz::rotate(2)) {
        std::cout << i << " ";
        // Or fmt::print("{} ", i);
    }
    // Output: 3 4 5 1 2
}
