#include <Lz/random.hpp>
#include <vector>
#include <iostream>


int main() {
    const float min = 0;
    const float max = 1;
    const size_t amount = 4;
    // Both use std::std::mt19937 as default random engine
    const auto rng = lz::random(min, max, amount); // return sentinel pair
    const auto rng_common = lz::common_random(min, max, amount); // return common iterator

#ifdef LZ_HAS_CXX_17
    for (float i : rng) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    }
    // Output: random number between [0, 1] random number between [0, 1] random number between [0, 1] random number between [0, 1]

    for (float i : rng_common) {
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

    lz::for_each(rng_common, [](float i) {
        std::cout << i << ' ';
        // Or use fmt::print("{} ", i);
    });


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