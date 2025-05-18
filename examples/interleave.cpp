#include <Lz/interleave.hpp>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec1 = { 1, 2, 3 }, vec2 = { 4, 5, 6, 7 }, vec3 = { 8, 9, 10, 11, 12 };
    auto interleaved = lz::interleave(vec1, vec2, vec3); // interleaved = { 1, 4, 8, 2, 5, 9, 3, 6, 10 }
    // or
    auto interleaved2 = vec1 | lz::interleave(vec2, vec3); // interleaved = { 1, 4, 8, 2, 5, 9, 3, 6, 10 }

#ifdef LZ_HAS_CXX_17
    
    for (const auto& i : interleaved) {
        std::cout << i << " ";
    }
    // output: 1 4 8 2 5 9 3 6 10
    std::cout << std::endl;

    for (const auto& i : interleaved2) {
        std::cout << i << " ";
    }
    // output: 1 4 8 2 5 9 3 6 10
    std::cout << std::endl;

#else

    lz::for_each(interleaved, [](int i) { std::cout << i << " "; });
    // output: 1 4 8 2 5 9 3 6 10
    std::cout << std::endl;

    lz::for_each(interleaved2, [](int i) { std::cout << i << " "; });
    // output: 1 4 8 2 5 9 3 6 10
    std::cout << std::endl;

#endif
}
