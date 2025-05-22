#include <Lz/take.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> seq = { 1, 2, 3, 4, 5, 6 };

    auto taken = lz::take(seq, 3);
    for (int& i : taken) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 1 2 3

    std::cout << '\n';

    for (int& i : seq | lz::take(3)) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 1 2 3
}