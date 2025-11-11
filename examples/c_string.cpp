#include <Lz/c_string.hpp>
#include <Lz/algorithm/for_each.hpp>
#include <iostream>


int main() {
    const char* my_str = "Hello, World!";

#ifdef LZ_HAS_CXX_17
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
#else
    lz::for_each(lz::c_string(my_str), [](const char& c) {
        std::cout << c;
        // or use fmt::print("{}\n", c);
    });
    // Output: Hello, World!

    std::cout << '\n';
    const char my_str_iterable[] = "Hello, World!";
    lz::for_each(my_str_iterable | lz::c_string, [](const char& c) {
        std::cout << c;
        // or use fmt::print("{}\n", c);
    });
// Output: Hello, World!
#endif
}
