#include <Lz/inclusive_scan.hpp>
#include <iostream>

int main() {
    int array[] = {3, 5, 2, 3, 4, 2, 3};
    // start the scan from arr[0] (3), with init 0
    // operator+ is required or add a custom operator+ in its third parameter

    auto scan = lz::inclusive_scan(array);

    for (int& i : scan) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // prints 3 8 10 13 17 19 22

    // essentially it's:
    // 0 + 3 = 3
    // 0 + 3 + 5 = 8
    // 0 + 3 + 5 + 2 = 10
    // 0 + 3 + 5 + 2 + 3 = 13
    // 0 + 3 + 5 + 2 + 3 + 4 = 17
    // 0 + 3 + 5 + 2 + 3 + 4 + 2 = 19
    // 0 + 3 + 5 + 2 + 3 + 4 + 2 + 3 = 22

    std::cout << '\n';
    // The 0 here is important. Cannot be left empty otherwise it will lead to compilation errors because the initial value cannot
    // be deduced.
    // operator+ is required or add a custom operator+ in its second parameter
    for (int& i : array | lz::inclusive_scan(0)) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // essentially it's:
    // 0 + 3 = 3
    // 0 + 3 + 5 = 8
    // 0 + 3 + 5 + 2 = 10
    // 0 + 3 + 5 + 2 + 3 = 13
    // 0 + 3 + 5 + 2 + 3 + 4 = 17
    // 0 + 3 + 5 + 2 + 3 + 4 + 2 = 19
    // 0 + 3 + 5 + 2 + 3 + 4 + 2 + 3 = 22
}