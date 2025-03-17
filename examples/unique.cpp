#include <Lz/unique.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> vector = {5, 3, 2, 5, 6, 42, 2, 3, 56, 3, 1, 12, 3};
    std::sort(vector.begin(), vector.end());

    // operator< is required or add a custom operator < in its second parameter
    const auto unique = lz::unique(vector);

    for (int& i : unique) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 1 2 3 4 5 6 12 42 56

    std::cout << '\n';

    // operator< is required or add a custom operator < in its first parameter
    for (int& i : vector | lz::unique) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 1 2 3 4 5 6 12 42 56
}