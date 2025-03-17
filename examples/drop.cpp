#include <Lz/drop.hpp>
#include <vector>

int main() {
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7 };
    for (int& i : lz::drop(vec, 4)) {
        std::cout << i << ' ';
        // Or: fmt::print("{} ", i);
    }
    // Output: 5 6 7

    std::cout << '\n';

    for (int& i : vec | lz::drop(4)) {
        std::cout << i << ' ';
        // Or: fmt::print("{} ", i);
    }
    // Output: 5 6 7
}