#include <Lz/take_while.hpp>

int main() {
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6 };

    auto taken = lz::take_while(vec, [](int i) { return i < 4; });

    for (int& i : taken) {
        std::cout << i << ' ';
        // or use fmt::print("{} ", i);
    }
    // Output: 1 2 3

    std::cout << '\n';

    for (int& i : vec | lz::take_while([](int i) { return i < 4; })) {
        std::cout << i << ' ';
        // or use fmt::print("{} ", i);
    }
    // Output: 1 2 3
}