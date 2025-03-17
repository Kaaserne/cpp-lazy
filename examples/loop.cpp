#include <Lz/loop.hpp>
#include <vector>

int main() {
    std::vector<int> vec = { 1, 2, 3 };
    for (int& i : lz::loop(vec, 2)) {
        std::cout << i << ' ';
    }
    // Output: 1 2 3 1 2 3

    std::cout << '\n';

    for (int& i : vec | lz::loop(2)) {
        std::cout << i << ' ';
    }

    std::cout << '\n';

    std::size_t count = 0;
    for (int& i : lz::loop(vec)) {
        std::cout << i << ' ';
        // Without break this would loop forever
        ++count;
        if (count == 500) {
            break;
        }
    }
    // Output: 1 2 3 1 2 3 1 2 3 1 2 3 1 2 3 1 2 3 ...

    std::cout << '\n';

    count = 0;
    for (int& i : vec | lz::loop) {
        std::cout << i << ' ';
        // Without break this would loop forever
        ++count;
        if (count == 500) {
            break;
        }
    }
    // Output: 1 2 3 1 2 3 1 2 3 1 2 3 1 2 3 1 2 3 ...
}
