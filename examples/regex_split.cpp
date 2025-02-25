#include <Lz/regex_split.hpp>
#include <iostream>
#include <string>
#include <vector>


int main() {
    std::string input = "Hello,World!This,is,a,test";
    std::regex r(",");
    auto result = lz::regex_split(input, r);

    for (const lz::string_view word : result) {
        // Use .write if using fmt::string_view
        std::cout.write(word.data(), word.size());
        std::cout << ' ';
        // Or use fmt::print("{}\n", word);
    }
    // output: Hello World!This is a test

    std::cout << '\n';

    for (const lz::string_view word : input | lz::regex_split(r)) {
        // Use .write if using fmt::string_view
        std::cout.write(word.data(), word.size());
        std::cout << ' ';
        // Or use fmt::print("{}\n", word);
    }
    // output: Hello World!This is a test
}