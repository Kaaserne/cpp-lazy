#include <Lz/Exclude.hpp>
#include <fmt/format.h>

int main() {
	std::vector<int> vec = {1, 2, 3, 4, 5};
    auto excluded = lz::exclude(vec, 2, 4);
    for (int& i : excluded) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 1 2 5

    std::cout << '\n';
    for (int& i : vec | lz::exclude(2, 4)) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 1 2 5
}