#include <Lz/basic_iterable.hpp>

int main() {
    std::vector<int> v = { 1, 2, 3, 4, 5 };

    lz::print(v); // 1, 2, 3, 4, 5
    std::cout << '\n';

    lz::print(v, " "); // 1 2 3 4 5
    std::cout << '\n';

    v | lz::print; // 1, 2, 3, 4, 5
    std::cout << '\n';

    v | lz::print(" "); // 1 2 3 4 5
    std::cout << '\n';

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
    lz::print(v, ", ", "{:02d}"); // 01, 02, 03, 04, 05
    std::cout << '\n';

    v | lz::print(", ", "{:02d}"); // 01, 02, 03, 04, 05
    std::cout << '\n';

    output = lz::format(v, ", ", "{:02d}"); // 01, 02, 03, 04, 05
    std::cout << output << '\n';

    output = v | lz::format(", ", "{:02d}"); // 01, 02, 03, 04, 05
    std::cout << output << '\n';

    lz::format(v, std::cout, ", ", "{:02d}"); // 01, 02, 03, 04, 05
    std::cout << '\n';

    v | lz::format(std::cout, ", ", "{:02d}"); // 01, 02, 03, 04, 05
    std::cout << '\n';
#endif

    std::cout << lz::to_iterable(v) << '\n'; // 1, 2, 3, 4, 5
}
