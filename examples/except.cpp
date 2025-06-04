#include <Lz/except.hpp>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    std::vector<int> to_except = { 5, 3 };
    std::sort(to_except.begin(), to_except.end());

    auto excepted = lz::except(vec, to_except); // excepted = { 1, 2, 4 }

#ifndef LZ_HAS_CXX_17

    lz::for_each(excepted, [](int i) { std::cout << i << " "; });

    // Output: 1 2 4

#else

    for (const auto& i : excepted) {
        std::cout << i << " ";
    }

    // Output: 1 2 4

#endif

    std::cout << std::endl;
}