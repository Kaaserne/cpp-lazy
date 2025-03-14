#include <Lz/Repeat.hpp>
#include <iostream>


int main() {
    const auto to_repeat = 155;
    const auto amount = 4;
    const auto repeater = lz::repeat(to_repeat, amount);

    for (int& i : lz::repeat(to_repeat, amount)) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 155 155 155 155
}