#include <Lz/basic_iterable.hpp>
#include <Lz/c_string.hpp>

int main() {
    // - Iterable as template parameter
    // - Iterable as function argument
    // - Iterable has a .size() method because arr has a size
    int arr[] = { 1, 2, 3, 4, 5 };
    lz::basic_iterable<int[5]> iterable_iterable = lz::to_iterable(arr);
    std::cout << iterable_iterable.size() << '\n'; // Output: 5

    // - Iterator as template parameter
    // - Iterable as function argument
    // - Iterable has size() method because std::begin(arr) and std::end(arr) are random access iterators
    lz::basic_iterable<int*> iterator_iterable = lz::to_iterable(arr);
    std::cout << iterator_iterable.size() << '\n'; // Output: 5

    // - Iterable as template parameter
    // - Iterator as function argument
    // - Iterable has size() method because std::begin(arr) and std::end(arr) are random access iterators
    lz::basic_iterable<int[5]> iterable_iterator = lz::to_iterable(std::begin(arr), std::end(arr));
    std::cout << iterable_iterator.size() << '\n'; // Output: 5

    // - Iterator as template parameter
    // - Iterator as function argument
    // - Iterable has size() method because std::begin(arr) and std::end(arr) are random access iterators
    lz::basic_iterable<int*> iterator_iterator = lz::to_iterable(std::begin(arr), std::end(arr));
    std::cout << iterator_iterator.size() << '\n'; // Output: 5

    // You can also work with sentinels:
    auto cstr = lz::c_string("Hello, world!");
    using it = decltype(cstr.begin());
    using sentinel = decltype(cstr.end());
    lz::basic_iterable<it, sentinel> iterable = lz::to_iterable(cstr.begin(), cstr.end());
    // iterable does not contain a size() method because cstr does not have a size and is also not random access
}
