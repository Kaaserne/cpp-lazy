#include <Lz/Zip.hpp>
#include <vector>
#include <iostream>


int main() {
    std::vector<int> a = {1, 2, 3, 4};
    std::vector<int> b = {1, 2, 3};
	
    for (std::tuple<int&, int&> tup : lz::zip(a, b)) {
        std::cout << std::get<0>(tup) << ' ' << std::get<1>(tup) << '\n';
    }
    // Yields:
    // 1 1
    // 2 2
    // 3 3

    std::cout << '\n';

    for (std::tuple<int&, int&> tup : a | lz::zip(b)) {
        std::cout << std::get<0>(tup) << ' ' << std::get<1>(tup) << '\n';
    }
    // Yields:
    // 1 1
    // 2 2
    // 3 3
}