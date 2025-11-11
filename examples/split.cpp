#include <Lz/algorithm/for_each.hpp>
#include <Lz/split.hpp>
#include <Lz/util/string_view.hpp>
#include <array>
#include <iostream>
#include <vector>

int main() {
    // With split you can split an iterable on a delimiter or an iterable delimiter (multiple delimiters)
    // The starting point is: if the delimiter is a single value (so no iterable) a string_view or a const char* (any char) type
    // then the delimiter does not have to be by reference.
    // If the delimiter is a non-lz iterable (like a std::string or a std::vector) then it has to be by reference.
#ifdef LZ_HAS_CXX_17
    std::string to_split = "Hello world ";
    std::string delim = " ";
    // sv_split returns a string_view splitter
    // delim has to be by reference, this does not count for const char*/c_string iterator
    const auto splitter = lz::sv_split(to_split, delim);

    std::cout << "Using string_views:\n";

    for (lz::string_view substring : splitter) {
        const auto substring_size = static_cast<std::streamsize>(substring.size());
        std::cout.write(substring.data(), substring_size);
        std::cout << ' ';
        // Or use fmt::print("{} ", substring);
    }
    // Output: Hello world

    std::cout << '\n';
    // const char* doesn't have to be by reference
    for (lz::string_view substring : to_split | lz::sv_split(" ")) {
        const auto substring_size = static_cast<std::streamsize>(substring.size());
        std::cout.write(substring.data(), substring_size);
        std::cout << ' ';
        // Or use fmt::print("{} ", substring);
    }
    // Output: Hello world

    std::cout << "\n\nUsing strings:\n";
    // const char* doesn't have to be by reference
    const auto splitter2 = lz::s_split(to_split, " ");
    for (std::string substring : splitter2) {
        std::cout << substring << ' ';
        // Or use fmt::print("{} ", substring);
    }
    // Output: Hello world
    std::cout << '\n';
    // const char* doesn't have to be by reference
    for (std::string substring : to_split | lz::s_split(" ")) {
        std::cout << substring << ' ';
        // Or use fmt::print("{} ", substring);
    }
    // Output: Hello world

    std::cout << "\n\nUsing other container types:\n";
    std::vector<int> to_split2 = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    std::array<int, 1> to_split_on = { 5 };
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
    // delim must be by reference
    auto splitter4 = to_split3 | lz::t_split<std::vector<char>>(delim2);
    for (std::vector<char> substring : splitter4) {
        const auto substring_size = static_cast<std::streamsize>(substring.size());
        std::cout.write(substring.data(), substring_size);
        std::cout << ' ';
        // Or use fmt::print("{} ", substring);
    }
    // Output: Hello world
    std::cout << '\n';

    // All of the same rules also apply for single arguments:

    std::string to_split4 = "hello world";
    // delim doesn't have to be by reference
    auto splitter_single = lz::sv_split(to_split4, ' ');
    for (lz::string_view substring : splitter_single) {
        const auto substring_size = static_cast<std::streamsize>(substring.size());
        std::cout.write(substring.data(), substring_size);
        std::cout << ' ';
        // Or use fmt::print("{} ", substring);
    }
#else
    std::string to_split = "Hello world ";
    std::string delim = " ";
    // sv_split returns a string_view splitter
    // delim must be by reference
    const auto splitter = lz::sv_split(to_split, delim);

    std::cout << "Using string_views:\n";

    lz::for_each(splitter, [](lz::string_view substring) {
        const auto substring_size = static_cast<std::streamsize>(substring.size());
        std::cout.write(substring.data(), substring_size);
        std::cout << ' ';
        // Or use fmt::print("{} ", substring);
    });
    // Output: Hello world

    std::cout << '\n';

    lz::for_each(splitter, [](lz::string_view substring) {
        const auto substring_size = static_cast<std::streamsize>(substring.size());
        std::cout.write(substring.data(), substring_size);
        std::cout << ' ';
        // Or use fmt::print("{} ", substring);
    });
    // Output: Hello world

    std::cout << "\n\nUsing strings:\n";
    // const char* doesn't have to be by reference
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
    std::array<int, 1> to_split_on = { 5 };
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
#ifdef LZ_HAS_CXX_11
    // delim2 must be by reference
    auto splitter4 = to_split3 | lz::t_split<std::vector<char>>{}(delim2);
#else
    // delim2 must be by reference
    auto splitter4 = to_split3 | lz::t_split<std::vector<char>>(delim2);
#endif
    lz::for_each(splitter4, [](std::vector<char> substring) {
        const auto substring_size = static_cast<std::streamsize>(substring.size());
        std::cout.write(substring.data(), substring_size);
        std::cout << ' ';
        // Or use fmt::print("{} ", substring);
    });
    // Output: Hello world
    std::cout << '\n';

    std::string to_split4 = "hello world";
    // delim doesn't have to be by reference
    auto splitter_single = lz::sv_split(to_split4, ' ');
    lz::for_each(splitter_single, [](lz::string_view substring) {
        const auto substring_size = static_cast<std::streamsize>(substring.size());
        std::cout.write(substring.data(), substring_size);
        std::cout << ' ';
        // Or use fmt::print("{} ", substring);
    });
#endif
}
