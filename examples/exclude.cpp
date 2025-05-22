#include <Lz/except.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> values = {1, 2, 3, 4, 5};
    std::vector<int> to_except = {2, 3, 4};

    std::sort(to_except.begin(), to_except.end()); // Always sort values to except before creating except object
    const auto except = lz::except(values, to_except);

#ifdef LZ_HAS_CXX_17
    for (int& i : except) {
        std::cout << i << ' ';
        // or use fmt::print("{} ", i);
    }
    std::cout << '\n';
    // Output:
    // 1 5

    for (int& i : values | lz::except(to_except)) {
        std::cout << i << ' ';
        // or use fmt::print("{} ", i);
    }
    std::cout << '\n';
    // Output:
    // 1 5
#else
    lz::for_each(except, [](int i) {
        std::cout << i << ' ';
        // or use fmt::print("{} ", i);
    });
    std::cout << '\n';
    // Output:
    // 1 5

    lz::for_each(values | lz::except(to_except), [](int i) {
        std::cout << i << ' ';
        // or use fmt::print("{} ", i);
    });
    // Output:
    // 1 5
#endif
}