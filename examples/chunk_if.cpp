#include <Lz/chunk_if.hpp>
#include <vector>

int main() {
	std::string s = "hello world; this is a message;";
	auto chunked = lz::chunk_if(s, [](const char c) { return c == ';'; });

#ifdef LZ_HAS_CXX_17
    for (auto chunk : chunked) {
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

    for (auto chunk : s | lz::chunk_if([](const char c) { return c == ';'; })) {
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

    // With std::string[_view]
    auto chunked_sv = s | lz::sv_chunk_if([](const char c) { return c == ';'; });
    for (auto chunk : chunked_sv) {
        std::cout.write(chunk.data(), chunk.size());
        std::cout << '\n';
    }
    // Output:
    // hello world
    //  this is a message

    auto chunked_s = s | lz::s_chunk_if([](const char c) { return c == ';'; });
    for (auto chunk : chunked_s) {
        std::cout << chunk << '\n';
        // or use fmt::print("{}\n", chunk);
    }
    // Output:
    // hello world
    //  this is a message

    // with custom types
    auto chunked_vector = s | lz::t_chunk_if<std::vector<char>>([](const char c) { return c == ';'; });
    for (auto vec_chunk : chunked_vector) {
        for (char& c : vec_chunk) {
            std::cout << c;
            // or use fmt::print("{}", c);
        }
        // or use fmt::print("\n");
        std::cout << '\n';
    }
#else
    using string_iterable = lz::basic_iterable<std::string::const_iterator>;
    lz::for_each(chunked, [](string_iterable chunk) {
        for (char c : chunk) {
            std::cout << c;
            // or use fmt::print("{}", c);
        }
        std::cout << '\n';
    });
    // Output:
    // hello world
    //  this is a message

    lz::for_each(s | lz::chunk_if([](const char c) { return c == ';'; }), [](string_iterable chunk) {
        for (char c : chunk) {
            std::cout << c;
            // or use fmt::print("{}", c);
        }
        std::cout << '\n';
    });
    // Output:
    // hello world
    //  this is a message

    // With lz::string[_view]
    lz::for_each(s | lz::sv_chunk_if([](const char c) { return c == ';'; }), [](const lz::string_view chunk) {
        std::cout.write(chunk.data(), chunk.size());
        std::cout << '\n';
    });
    // Output:
    // hello world
    //  this is a message

    lz::for_each(s | lz::s_chunk_if([](const char c) { return c == ';'; }), [](const std::string& chunk) {
        std::cout << chunk << '\n';
        // or use fmt::print("{}\n", chunk);
    });
    // Output:
    // hello world
    //  this is a message

    // with custom types
    lz::for_each(s | lz::t_chunk_if<std::vector<char>>([](const char c) { return c == ';'; }),
                 [](const std::vector<char>& vec_chunk) {
                     for (char c : vec_chunk) {
                         std::cout << c;
                         // or use fmt::print("{}", c);
                     }
                     std::cout << '\n';
                 });
    // Output:
    // hello world
    //  this is a message

#endif
}
