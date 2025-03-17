#include <Lz/slice.hpp>
#include <vector>


int main() {
    std::vector<int> v = { 1, 2, 3, 4, 5 };

    auto s = lz::slice(v, 1, 3);
    for (int& i : s) {
        std::cout << i << " ";
        // Or fmt::print("{} ", i);
    }
    // Output: 2 3

    std::cout << '\n';

    for (int& i : v | lz::slice(1, 3)) {
        std::cout << i << " ";
        // Or fmt::print("{} ", i);
    }
    // Output: 2 3
}
