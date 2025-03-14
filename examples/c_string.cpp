#include <Lz/c_string.hpp>
#include <iostream>


int main() {
    const char* my_str = "Hello, World!";

    for (const char& c : lz::c_string(my_str)) {
        std::cout << c;
        // or use fmt::print("{}\n", c);
    }
    // Output: Hello, World!
    std::cout << '\n';

    const char my_str_iterable[] = "Hello, World!";
    for (const char& c : my_str_iterable | lz::c_string) {
        std::cout << c;
        // or use fmt::print("{}\n", c);
    }
    // Output: Hello, World!
    std::cout << '\n';
}