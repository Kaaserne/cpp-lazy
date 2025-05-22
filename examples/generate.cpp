#include <Lz/generate.hpp>
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

#ifdef LZ_HAS_CXX_17
    for (std::size_t i : gen) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
// Output: 0 1 2 3
#else
    lz::for_each(gen, [](std::size_t i) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    });
// Output: 0 1 2 3
#endif
}
