import lz;

#include <iostream>
#include <vector>
// TODO test all modules?
int main() {
    // Example usage of cpp-lazy
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    auto doubled = vec | lz::map([](int i) { return i * 2; });
    std::cout << doubled << std::endl; // Output: 2, 4, 6, 8, 10
    std::cout << "Size = " << lz::size(doubled) << std::endl; // Output: Size= 5

    for (const auto& val : doubled) {
        std::cout << val << " "; // Output: 2 4 6 8 10
    }
    std::cout << std::endl;

    return 0;
}
