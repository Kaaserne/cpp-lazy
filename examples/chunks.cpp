#include <Lz/chunks.hpp>
#include <vector>

int main() {
	std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};
	auto chunks = lz::chunks(v, 3);

	for (auto&& chunk : chunks) {
        std::cout << "This chunk has length " << std::distance(chunk.begin(), chunk.end()) << '\n';
        // or use fmt::print("This chunk has length {}\n", std::distance(chunk.begin(), chunk.end()));
        for (int& i : chunk) {
            std::cout << i << ' ';
            // or use fmt::print("{} ", i);
        }
        std::cout << '\n';
        // or use fmt::print("\n");
    }
    std::cout << '\n';
    // Output
    // This chunk has length 3
    // 1 2 3
    // This chunk has length 3
    // 4 5 6
    // This chunk has length 2
    // 7 8

    for (auto&& chunk : v | lz::chunks(3)) {
        std::cout << "This chunk has length " << std::distance(chunk.begin(), chunk.end()) << '\n';
        // or use fmt::print("This chunk has length {}\n", std::distance(chunk.begin(), chunk.end()));
        for (int& i : chunk) {
            std::cout << i << ' ';
            // or use fmt::print("{} ", i);
        }
        std::cout << '\n';
        // or use fmt::print("\n");
    }
}