#include <Lz/generate.hpp>
#include <Lz/map.hpp>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <vector>

// In case you have a custom container, you can specialize `lz::custom_copier_for` to copy the elements to your container.
// This is useful if you have a custom container that requires a specific way of copying elements. You will need to specialize
// `lz::custom_copier_for` for your custom container if all of the following are true:
// - Your custom container does not have a `push_back` method
// - Your custom container does not have an `insert` method
// - Your custom container does not have an `insert_after` method (implicitly also requires `before_begin`)
// - Your custom container does not have a `push` method
// - Your custom container is not std::array
template<class T>
class custom_container {
    std::vector<T> _vec;

public:
    void reserve(std::size_t size) {
        _vec.reserve(size);
    }

    std::vector<T>& vec() {
        return _vec;
    }
};

// Specialize `lz::custom_copier_for` for your custom container
template<class T>
struct lz::custom_copier_for<custom_container<T>> {
    template<class Iterable>
    void copy(Iterable&& iterable, custom_container<T>& container) const {
        container.reserve(iterable.size());
        // Copy the contents of the iterable to the container. Container is not yet reserved
        lz::copy(std::forward<Iterable>(iterable), std::back_inserter(container.vec()));
    }
};

int main() {
    char c = 'a';
    auto generator = lz::generate(
        [&c]() {
            auto tmp = c;
            ++c;
            return tmp;
        },
        4);

    // To vector:
    auto vec = generator | lz::to<std::vector>();
    for (char& val : vec) {
        std::cout << val << ' ';
    }
    c = 'a';
    // Output: a b c d
    std::cout << '\n';

    // To set
    auto set = generator | lz::to<std::set>();
    for (char val : set) {
        std::cout << val << ' ';
    }
    c = 'a';
    // Output: a b c d
    std::cout << '\n';

    // To list
    auto list = generator | lz::to<std::list<char>>();
    for (char val : set) {
        std::cout << val << ' ';
    }
    c = 'a';
    // Output: a b c d
    std::cout << "\n\n";

    // To map
    auto map = generator | lz::map([](const char c) { return std::make_pair(static_cast<char>(c + 1), c); }) |
               lz::to<std::map<char, char>>();
    for (std::pair<char, char> pair : map) {
        std::cout << pair.first << ' ' << pair.second << '\n';
    }
    c = 'a';
    // Output:
    // b a
    // c b
    // d c
    // e d

    std::cout << '\n';

    std::vector<char> copy_to_vec;
    lz::copy(generator, std::back_inserter(copy_to_vec));
    for (char val : copy_to_vec) {
        std::cout << val << ' ';
    }
    c = 'a';
    // Output: a b c d

    std::cout << '\n';

    std::vector<int> transform_to_vec;
    lz::transform(generator, std::back_inserter(transform_to_vec), [](const char i) -> char { return i + 1; });
    for (int val : transform_to_vec) {
        std::cout << val << ' ';
    }
    c = 'a';
    // Output: 98 99 100 101

    std::cout << '\n';

    auto out = generator | lz::map([](char c) -> char { return c + 1; }) | lz::to<std::vector>();
    for (char val : out) {
        std::cout << val << ' ';
    }
    // Output: b c d e

    auto custom = generator | lz::to<custom_container<char>>();
    for (char val : custom.vec()) {
        std::cout << val << ' ';
    }
    // Output: a b c d
}