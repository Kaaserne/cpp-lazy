#include <Lz/any_iterable.hpp>
#include <Lz/filter.hpp>
#include <Lz/map.hpp>
#include <iostream>
#include <vector>

namespace {

// First template parmeter is the value_type. The second the reference type.
// Because we do no return any references here, we need to specify a second template parameter: an int by copy:
lz::any_iterable<int, int> filter_map_copy_value(const std::vector<int>& vec) {
    return vec | lz::filter([](int i) { return i % 2 == 0; }) | lz::map([](int i) { return i * 2; });
}

// First template parmeter is the value_type. The second the reference type.
// Implicitly, this is value_type&, so in our case int&, if that is not correct then you need to specify it.
// Because we return a reference here, we do not need to specify a second template parameter:
lz::any_iterable<int> filter_map_reference(std::vector<int>& vec) {
    return vec | lz::filter([](int& i) { return i % 2 == 0; }) | lz::map([](int& i) -> int& { return i; });
}

} // namespace

int main() {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    lz::any_iterable<int, int> view = filter_map_copy_value(vec);
    for (int i : view) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
    // output:
    // 4 8

    auto second_view = filter_map_reference(vec);
    for (int& i : second_view) {
        std::cout << i << ' ';
    }
    std::cout << '\n';

    // output:
    // 2 4
    return 0;
}
