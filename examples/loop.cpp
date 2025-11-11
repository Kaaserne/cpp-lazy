#include <Lz/algorithm/for_each.hpp>
#include <Lz/loop.hpp>
#include <iostream>
#include <vector>

int main() {
#ifdef LZ_HAS_CXX_17
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
#else
    std::vector<int> vec = { 1, 2, 3 };
    lz::for_each(lz::loop(vec, 2), [](int i) {
        std::cout << i << ' ';
    });
    // Output: 1 2 3 1 2 3

    std::cout << '\n';

    lz::for_each(vec | lz::loop(2), [](int i) {
        std::cout << i << ' ';
    });
    // Output: 1 2 3 1 2 3

    std::cout << '\n';

    std::size_t count = 0;

    lz::for_each(lz::loop(vec), [&count](int i) {
        std::cout << i << ' ';
        // Without break this would loop forever
        ++count;
        if (count == 500) {
            return false;
        }
        return true;
    });
    // Output: 1 2 3 1 2 3 1 2 3 1 2 3 1 2 3 1 2 3 ...

    std::cout << '\n';

    count = 0;
    lz::for_each(vec | lz::loop, [&count](int i) {
        std::cout << i << ' ';
        // Without break this would loop forever
        ++count;
        if (count == 500) {
            return false;
        }
        return true;
    });
    // Output: 1 2 3 1 2 3 1 2 3 1 2 3 1 2 3 1 2 3 ...
#endif
}
