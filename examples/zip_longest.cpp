#include <Lz/zip_longest.hpp>
#include <iostream>
#include <list>
#include <vector>

int main() {
    std::vector<int> v1 = { 1, 2, 3, 4, 5, 6 };
    std::list<char> v2 = { 'h', 'e', 'l', 'l' };
    std::vector<int> v3 = { 1, 2, 3 };

    // iterator is now lowest iterator category of the three iterators. In this case, it is std::bidi_iterator_tag because v2 is a
    // list<char> which has a bidirectional iterator.
    for (auto&& tup : lz::zip_longest(v1, v2, v3)) { // Elements are accessed by reference (std::reference_wrapper)
        auto&& first = std::get<0>(tup);
        auto&& second = std::get<1>(tup);
        auto&& third = std::get<2>(tup);

        if (first) {
            std::cout << *first << ' ';
        }
        if (second) {
            std::cout << *second << ' ';
        }
        if (third) {
            std::cout << *third << ' ';
        }
        std::cout << '\n';
    }
    // Output:
    // 1 h 1
    // 2 e 2
    // 3 l 3
    // 4 l
    // 5
    // 6

    std::cout << '\n';

    // iterator is now lowest iterator category of the three iterators. In this case, it is std::bidi_iterator_tag because v2 is a
    // list<char> which has a bidirectional iterator.
    for (auto&& tup : v1 | lz::zip_longest(v2, v3)) { // Elements are accessed by reference (std::reference_wrapper)
        auto&& first = std::get<0>(tup);
        auto&& second = std::get<1>(tup);
        auto&& third = std::get<2>(tup);

        if (first) {
            std::cout << *first << ' ';
        }
        if (second) {
            std::cout << *second << ' ';
        }
        if (third) {
            std::cout << *third << ' ';
        }
        std::cout << '\n';
    }
    // Output:
    // 1 h 1
    // 2 e 2
    // 3 l 3
    // 4 l
    // 5
    // 6
}