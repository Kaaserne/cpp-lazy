#include <Lz/map.hpp>
#include <vector>
#include <iostream>


struct some_struct {
    std::string s;
    int a;
};

// TODO bloaty tests

int main() {
    std::vector<some_struct> s = {
        some_struct{"Hello"},
        some_struct{"World"}
    };

    const auto mapper = lz::map(s, [](const some_struct& s) -> const std::string& { return s.s; });

    for (const std::string& i : mapper) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: Hello World

    std::cout << '\n';
    for (const std::string& i : s | lz::map([](const some_struct& s) -> const std::string& { return s.s; })) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: Hello World
}