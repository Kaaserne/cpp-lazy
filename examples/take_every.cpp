#include <Lz/take_every.hpp>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> sequence = {1, 2, 3, 4, 5};
    auto take_every = lz::take_every(sequence, 2);

    for (int& i : take_every) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 1 3 5

    std::cout << '\n';

    take_every = lz::take_every(sequence, 2, 1);

    for (int& i : take_every) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 2 4 6

    std::cout << '\n';

    for (int& i : sequence | lz::take_every(2)) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 1 3 5
}