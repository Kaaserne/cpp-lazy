#include <Lz/random.hpp>
#include <vector>
#include <iostream>


int main() {
    const float min = 0;
    const float max = 1;
    const size_t amount = 4;
    const auto rng = lz::random(min, max, amount);

#ifdef LZ_HAS_CXX_17
    for (float i : rng) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: random number between [0, 1] random number between [0, 1] random number between [0, 1] random number between [0, 1]

    std::cout << '\n';
    // Or create your own
    static std::random_device rd;
    std::mt19937_64 gen(rd());
    std::poisson_distribution<> d(500'000);
    auto r = lz::random(d, gen, 3);

    for (int i : r) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: [random number] [random number] [random number]
#else
    lz::for_each(rng, [](float i) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    });
    // Output: random number between [0, 1] random number between [0, 1] random number between [0, 1] random number between [0, 1]

    std::cout << '\n';

    // Or create your own
    static std::random_device rd;
    std::mt19937_64 gen(rd());
    std::poisson_distribution<> d(500'000);
    auto r = lz::random(d, gen, 3);

    lz::for_each(r, [](int i) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    });
// Output: [random number] [random number] [random number]
#endif
}