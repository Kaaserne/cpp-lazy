#include <Lz/intersection.hpp>
#include <vector>
#include <string>

int main() {
    std::vector<int> a = { 1, 2, 3, 4, 5, 6, 7 };
    std::vector<int> b = { 2, 3, 7, 9 };

    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());

    for (int& i : lz::intersection(a, b)) {
        std::cout << i << ' ';
        // Or fmt::print("{} ", i);
    }
    // Output: 2 3 7
    std::cout << '\n';

    for (int& i : a | lz::intersection(b)) {
        std::cout << i << ' ';
        // Or fmt::print("{} ", i);
    }
    // Output: 2 3 7
}
