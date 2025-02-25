#include <Lz/chunk_if.hpp>

int main() {
	std::string s = "hello world; this is a message;";
	auto chunked = lz::chunk_if(s, [](const char c) { return c == ';'; });

	for (auto&& chunk : chunked) {
        for (char& c : chunk) {
            std::cout << c;
            // or use fmt::print("{}", c);
        }
        // or use fmt::print("\n");
        std::cout << '\n';
    }
    // Output:
    // hello world
    //  this is a message

    for (auto&& chunk : s | lz::chunk_if([](const char c) { return c == ';'; })) {
        for (char& c : chunk) {
            std::cout << c;
            // or use fmt::print("{}", c);
        }
        // or use fmt::print("\n");
        std::cout << '\n';
    }
    // Output:
    // hello world
    //  this is a message
}