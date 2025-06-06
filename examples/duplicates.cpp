#include <Lz/duplicates.hpp>
#include <iostream>

int main() {
    std::vector<int> input = { 1, 2, 2, 3, 4, 4, 5, 6, 6, 6 };
    std::sort(input.begin(), input.end());
    auto dupes = lz::duplicates(input);
    for (const auto& pair : dupes) {
        std::cout << "Element: " << pair.first << ", Count: " << pair.second << '\n';
    }
    // Example output:
    // Element: 1, Count: 1
    // Element: 2, Count: 2
    // Element: 3, Count: 1
    // Element: 4, Count: 2
    // Element: 5, Count: 1
    // Element: 6, Count: 3
}
