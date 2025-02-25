#include <Lz/Generate.hpp>
#include <vector>
#include <functional>
#include <iostream>

int main() {
	std::size_t counter = 0;
	static constexpr std::size_t amount = 4;

    auto gen = lz::generate(
        [&counter]() {
            auto tmp{ counter++ };
            return tmp;
        },
        amount);

    for (int i : gen) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 0 1 2 3
}
