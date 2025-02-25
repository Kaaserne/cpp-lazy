#include <Lz/generate.hpp>
#include <Lz/map.hpp>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <vector>

int main() {
    char c = 'a';
    auto generator = lz::generate(
        [&c]() {
            auto tmp = c;
            ++c;
            return tmp;
        },
        4);

    // To vector:
    auto vec = generator | lz::to<std::vector>();
    for (char& val : vec) {
        std::cout << val << ' ';
    }
    c = 'a';
    // Output: a b c d
    std::cout << '\n';

    // To set
    auto set = generator | lz::to<std::set>();
    for (char val : set) {
        std::cout << val << ' ';
    }
    c = 'a';
    // Output: a b c d
    std::cout << '\n';

    // To list
    auto list = generator | lz::to<std::list<char>>();
    for (char val : set) {
        std::cout << val << ' ';
    }
    c = 'a';
    // Output: a b c d
    std::cout << "\n\n";

    // To map
    auto map = generator | lz::map([](const char c) { return std::make_pair(static_cast<char>(c + 1), c); }) |
               lz::to<std::map<char, char>>();
    for (std::pair<char, char> pair : map) {
        std::cout << pair.first << ' ' << pair.second << '\n';
    }
    c = 'a';
    // Output:
    // b a
    // c b
    // d c
    // e d

    std::cout << '\n';

    std::vector<char> copy_to_vec;
    lz::copy(generator, std::back_inserter(copy_to_vec));
    for (char val : copy_to_vec) {
        std::cout << val << ' ';
    }
    c = 'a';
    // Output: a b c d

    std::cout << '\n';

    std::vector<int> transform_to_vec;
    lz::transform(generator, std::back_inserter(transform_to_vec), [](const char i) -> char { return i + 1; });
    for (int val : transform_to_vec) {
        std::cout << val << ' ';
    }
    c = 'a';
    // Output: 98 99 100 101

    std::cout << '\n';

    auto out = generator | lz::map([](char c) -> char { return c + 1; }) | lz::to<std::vector>();
    for (char val : out) {
        std::cout << val << ' ';
    }
    // Output: b c d e
}