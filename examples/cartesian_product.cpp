#include <Lz/cartesian_product.hpp>

int main() {
    std::vector<int> vec = {1, 2, 3};
    std::vector<char> chars = {'a', 'b', 'c'};

    auto cartesian = lz::cartesian_product(vec, chars);

    for (auto&& tup : cartesian) {
        auto&& first = std::get<0>(tup);
        auto&& snd = std::get<1>(tup);

        std::cout << first << ' ' << snd << '\n';
        // or use fmt::print("{} {}\n", first, snd);
    }
    // Output:
    // 1 a
    // 1 b
    // 1 c
    // 2 a
    // 2 b
    // 2 c
    // 3 a
    // 3 b
    // 3 c
    std::cout << '\n';
    // or
    cartesian = vec | lz::cartesian_product(chars);

    for (auto&& tup : cartesian) {
        auto&& first = std::get<0>(tup);
        auto&& snd = std::get<1>(tup);

        std::cout << first << ' ' << snd << '\n';
        // or use fmt::print("{} {}\n", first, snd);
    }
    // Output:
    // 1 a
    // 1 b
    // 1 c
    // 2 a
    // 2 b
    // 2 c
    // 3 a
    // 3 b
    // 3 c
}