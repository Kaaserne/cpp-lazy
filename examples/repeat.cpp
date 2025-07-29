#include <Lz/repeat.hpp>
#include <iostream>

int main() {
    // Example usage of lz::repeat, yields elements by value
    const auto to_repeat = 155;
    const auto amount = 4;
    const auto repeater_n = lz::repeat(to_repeat, amount);
    const auto repeater_inf = lz::repeat(to_repeat);

#ifdef LZ_HAS_CXX_17
    for (int i : repeater_n) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 155 155 155 155

    for (const auto i : repeater_inf) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 155 155 155 155 ...
#else
    lz::for_each(repeater_n, [](int i) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    });
    // Output: 155 155 155 155

    lz::for_each(repeater_inf, [](int i) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    });
    // Output: 155 155 155 155 ...
#endif
}