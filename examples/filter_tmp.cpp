#include <Lz/filter.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> to_filter = {1, 2, 3, 4, 5, 6};
    const auto filter = lz::filter(to_filter, [](const int i) { return i % 2 == 0; });
	for (int& i : filter) {
		std::cout << i << ' ';
		// Or use fmt::print("{} ", i);
	}
    // Output: 2 4 6

	std::cout << '\n';
	for (int& i : to_filter | lz::filter([](const int i) { return i % 2 == 0; })) {
		std::cout << i << ' ';
		// Or use fmt::print("{} ", i);
	}
	// Output: 2 4 6
}