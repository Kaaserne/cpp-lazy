#include <Lz/as_iterator.hpp>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    auto iter = lz::as_iterator(vec);

    using vector_iter = typename decltype(iter)::value_type;

    for (const vector_iter vector_iterator : iter) {
        std::cout << *vector_iterator << " "; // prints: 1 2 3 4 5
    }
    std::cout << std::endl;

    return 0;
}
