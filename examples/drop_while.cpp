#include <Lz/drop_while.hpp>

int main() {
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7 };
    for (int& i : lz::drop_while(vec, [](int i) { return i < 5; })) {
        std::cout << i << ' ';
        // Or: fmt::print("{} ", i);
    }
    // Output: 5 6 7

    std::cout << '\n';

    for (int& i : vec | lz::drop_while([](int i) { return i < 5; })) {
        std::cout << i << ' ';
        // Or: fmt::print("{} ", i);
    }
    // Output: 5 6 7
}
