#include <Lz/pairwise.hpp>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec1{ 1, 2, 3, 4, 5 };

    for (auto iterable : lz::pairwise(vec1, 2)) {
        for (auto v : iterable) {
            std::cout << v << ' ';
        }
        std::cout << '\n';
    }

    // Output
    // 1 2
    // 2 3
    // 3 4
    // 4 5
}
