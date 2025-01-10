#include <Lz/Unique.hpp>
#include <iostream>

int main() {
    std::vector<int> vector = {5, 3, 2, 5, 6, 42, 2, 3, 56, 3, 1, 12, 3};
	// operator< is required or add a custom operator < in its second parameter
    std::sort(vector.begin(), vector.end());
    const auto unique = lz::unique(vector);
    std::cout << unique << '\n';
    // Output: 1 2 3 4 5 6 12 42 56
    for (int i : unique) {
        // process i...
    }
}