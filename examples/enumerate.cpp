#include <Lz/enumerate.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> to_enumerate = {1, 2, 3, 4, 5};

    std::cout << "By value\n";
    for (std::pair<int, int> pair : lz::enumerate(to_enumerate)) {
        std::cout << pair.first << ' ' << pair.second << '\n';
        // Or use fmt::print("{} {}\n", pair.first, pair.second);
    }
    // yields:
    // [(index element)by value] [(container element)]
    //          0                               1
    //          1                               2
    //          2                               3
    //          3                               4
    //          4                               5
    std::cout << '\n';
    for (std::pair<int, int> pair : to_enumerate | lz::enumerate) {
        std::cout << pair.first << ' ' << pair.second << '\n';
        // Or use fmt::print("{} {}\n", pair.first, pair.second);
    }
    // yields:
    // [(index element)by value] [(container element)]
    //          0                               1
    //          1                               2
    //          2                               3
    //          3                               4
    //          4                               5

    std::cout << "\nBy reference\n";
    for (std::pair<int, int&> pair : to_enumerate |  lz::enumerate(1)) {
        std::cout << pair.first << ' ' << pair.second << '\n';
        // Or use fmt::print("{} {}\n", pair.first, pair.second);
    }
    // yields:
    // [(index element)by value] [(container element by reference)]
    //          1                               1
    //          2                               2
    //          3                               3
    //          4                               4
    //          5                               5
}