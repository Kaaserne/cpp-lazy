#include <Lz/algorithm.hpp>
#include <Lz/c_string.hpp>
#include <Lz/common.hpp>
#include <iostream>

int main() {
    auto iterable_with_end_sentinel = lz::c_string("hello world");
    // .end() is a sentinel (lz::default_sentinel_t) meaning it is a different type than its .begin() type
    // To make it a common one, use:
    auto iterable = iterable_with_end_sentinel | lz::common;
    // or
    auto iterable2 = lz::common(iterable_with_end_sentinel);

    // The following would give us a compiler error (normally) because the types are different:
    // std::find_if(iterable_with_end_sentinel.begin(), iterable_with_end_sentinel.end(), [](char c) { return c == 'o'; });

    // But with lz::common, it works:
    auto pos = std::find_if(iterable.begin(), iterable.end(), [](char c) { return c == 'o'; });
    auto pos2 = std::find_if(iterable2.begin(), iterable2.end(), [](char c) { return c == 'o'; });

    std::cout << "found o in (common)iterable with std::find_if at " << std::distance(iterable.begin(), pos) << '\n';    // o
    std::cout << "found o in (common)iterable2 with std::find_if at " << std::distance(iterable2.begin(), pos2) << '\n'; // o

    // Alternatively, you can use lz::find_if (in combination with lz::distance):
    auto pos3 = lz::find_if(iterable_with_end_sentinel, [](char c) { return c == 'o'; });
    std::cout << "found o in iterable_with_end_sentinel with lz::find_if at "
              << lz::distance(iterable_with_end_sentinel.begin(), pos3) << '\n'; // o
}
