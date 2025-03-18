#include <Lz/split.hpp>
#include <iostream>
#include <vector>


int main() {
#ifdef LZ_HAS_CXX_17
    std::string to_split = "Hello world ";
    std::string delim = " ";
    // sv_split returns a string_view splitter
    const auto splitter = lz::sv_split(to_split, std::move(delim));

    std::cout << "Using string_views:\n";

    for (lz::string_view substring : splitter) {
        std::cout.write(substring.data(), substring.size());
        std::cout << ' ';
        // Or use fmt::print("{} ", substring);
    }
    // Output: Hello world

    std::cout << '\n';

    for (lz::string_view substring : to_split | lz::sv_split(" ")) {
        std::cout.write(substring.data(), substring.size());
        std::cout << ' ';
        // Or use fmt::print("{} ", substring);
    }
    // Output: Hello world

    std::cout << "\n\nUsing strings:\n";
    const auto splitter2 = lz::s_split(to_split, " ");
    for (std::string substring : splitter2) {
        std::cout << substring << ' ';
        // Or use fmt::print("{} ", substring);
    }
    // Output: Hello world
    std::cout << '\n';

    for (std::string substring : to_split | lz::s_split(" ")) {
        std::cout << substring << ' ';
        // Or use fmt::print("{} ", substring);
    }
    // Output: Hello world

    std::cout << "\n\nUsing other container types:\n";
    std::vector<int> to_split2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto to_split_on = { 5 };
    // to_split_on must be by reference!
    const auto splitter3 = lz::split(to_split2, to_split_on);
    // Returns an iterable of iterables
    for (auto splitted : splitter3) {
        for (auto& i : splitted) {
            std::cout << i << ' ';
            // Or use fmt::print("{} ", i);
        }
        std::cout << '\n';
    }
    // Output:
    // 1 2 3 4
    // 6 7 8 9

    // With custom types
    std::string to_split3 = "Hello world ";
    std::string delim2 = " ";
    auto splitter4 = to_split3 | lz::t_split<std::vector<char>>(delim2);
    for (std::vector<char> substring : splitter4) {
        std::cout.write(substring.data(), substring.size());
        std::cout << ' ';
        // Or use fmt::print("{} ", substring);
    }
    // Output: Hello world
    std::cout << '\n';
#else
    std::string to_split = "Hello world ";
    std::string delim = " ";
    // sv_split returns a string_view splitter
    const auto splitter = lz::sv_split(to_split, std::move(delim));

    std::cout << "Using string_views:\n";

    lz::for_each(splitter, [](lz::string_view substring) {
        std::cout.write(substring.data(), substring.size());
        std::cout << ' ';
        // Or use fmt::print("{} ", substring);
    });
    // Output: Hello world

    std::cout << '\n';

    lz::for_each(splitter, [](lz::string_view substring) {
        std::cout.write(substring.data(), substring.size());
        std::cout << ' ';
        // Or use fmt::print("{} ", substring);
    });
    // Output: Hello world

    std::cout << "\n\nUsing strings:\n";
    const auto splitter2 = lz::s_split(to_split, " ");
    lz::for_each(splitter2, [](std::string substring) {
        std::cout << substring << ' ';
        // Or use fmt::print("{} ", substring);
    });
    // Output: Hello world
    std::cout << '\n';

    lz::for_each(splitter2, [](std::string substring) {
        std::cout << substring << ' ';
        // Or use fmt::print("{} ", substring);
    });
    // Output: Hello world

    std::cout << "\n\nUsing other container types:\n";
    std::vector<int> to_split2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto to_split_on = { 5 };
    // to_split_on must be by reference!
    const auto splitter3 = lz::split(to_split2, to_split_on);
    // Returns an iterable of iterables
    lz::for_each(splitter3, [](lz::basic_iterable<std::vector<int>::iterator> splitted) {
        for (int i : splitted) {
            std::cout << i << ' ';
            // Or use fmt::print("{} ", i);
        }
        std::cout << '\n';
    });
    // Output:
    // 1 2 3 4
    // 6 7 8 9

    // With custom types
    std::string to_split3 = "Hello world ";
    std::string delim2 = " ";
    auto splitter4 = to_split3 | lz::t_split<std::vector<char>>(delim2);
    lz::for_each(splitter4, [](std::vector<char> substring) {
        std::cout.write(substring.data(), substring.size());
        std::cout << ' ';
        // Or use fmt::print("{} ", substring);
    });
    // Output: Hello world
    std::cout << '\n';
#endif
}
