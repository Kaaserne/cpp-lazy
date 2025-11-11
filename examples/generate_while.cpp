#include <Lz/algorithm/for_each.hpp>
#include <Lz/generate_while.hpp>
#include <iostream>

int main() {
    int i = 0;
    auto generator = lz::generate_while([&i]() {
        auto copy = i++;
        // If `copy` == 4, stop generating values.
        // This function must return a pair like object (e.g. std::pair) where pair::first must
        // be a type that is convertible to bool and where pair::second can be any type
        return std::make_pair(copy, copy != 4);
    });

#ifdef LZ_HAS_CXX_17
    // Iterator returns the same type as the function pair second type. In this case, int.
    for (int i : generator) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: 0 1 2 3
#else
    lz::for_each(generator, [](int idx) {
        std::cout << idx << ' ';
        // Or use fmt::print("{} ", i);
    });
    // Output: 0 1 2 3
#endif
}
