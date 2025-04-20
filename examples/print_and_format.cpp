#include <Lz/stream.hpp>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = { 1, 2, 3, 4, 5 };

    std::string output = lz::format(v); // 1, 2, 3, 4, 5
    std::cout << output << '\n';

    output = lz::format(v, " "); // 1 2 3 4 5
    std::cout << output << '\n';

    output = v | lz::format; // 1, 2, 3, 4, 5
    std::cout << output << '\n';

    output = v | lz::format(" "); // 1 2 3 4 5
    std::cout << output << '\n';

    lz::format(v, std::cout); // 1, 2, 3, 4, 5
    std::cout << '\n';

    lz::format(v, std::cout, " "); // 1 2 3 4 5
    std::cout << '\n';

    v | lz::format(std::cout); // 1, 2, 3, 4, 5
    std::cout << '\n';

    v | lz::format(std::cout, " "); // 1 2 3 4 5
    std::cout << '\n';

    // If you're using fmt or have std::format
#if !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)
    output = lz::format(v, ", ", "{:02d}"); // 01, 02, 03, 04, 05
    std::cout << output << '\n';

    output = v | lz::format(", ", "{:02d}"); // 01, 02, 03, 04, 05
    std::cout << output << '\n';

    lz::format(v, std::cout, ", ", "{:02d}"); // 01, 02, 03, 04, 05
    std::cout << '\n';

    v | lz::format(std::cout, ", ", "{:02d}"); // 01, 02, 03, 04, 05
    std::cout << '\n';
#endif

    std::cout << lz::basic_iterable<decltype(v.begin())>{ v.begin(), v.end() } << '\n'; // 1, 2, 3, 4, 5
}
