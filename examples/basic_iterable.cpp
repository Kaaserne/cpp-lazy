#include <Lz/basic_iterable.hpp>
#include <Lz/c_string.hpp>
#include <vector>

int main() {
    // - Iterable as template parameter
    // - Iterable as function argument
    // - Iterable has a .size() method because arr has a size
    int arr[] = { 1, 2, 3, 4, 5 };
    lz::basic_iterable<int[5]> iterable_iterable(arr);
    std::cout << iterable_iterable.size() << '\n'; // Output: 5

    // - Iterator as template parameter
    // - Iterable as function argument
    // - Iterable has size() method because std::begin(arr) and std::end(arr) are random access iterators
    lz::basic_iterable<int*> iterator_iterable(arr);
    std::cout << iterator_iterable.size() << '\n'; // Output: 5

    // - Iterable as template parameter
    // - Iterator as function argument
    // - Iterable has size() method because std::begin(arr) and std::end(arr) are random access iterators
    lz::basic_iterable<int[5]> iterable_iterator(std::begin(arr), std::end(arr));
    std::cout << iterable_iterator.size() << '\n'; // Output: 5

    // - Iterator as template parameter
    // - Iterator as function argument
    // - Iterable has size() method because std::begin(arr) and std::end(arr) are random access iterators
    lz::basic_iterable<int*> iterator_iterator(std::begin(arr), std::end(arr));
    std::cout << iterator_iterator.size() << '\n'; // Output: 5

    // You can also work with sentinels:
    auto cstr = lz::c_string("Hello, world!");
    using it = decltype(cstr.begin());
    using sentinel = decltype(cstr.end());
    lz::basic_iterable<it, sentinel> iterable(cstr.begin(), cstr.end());
    std::cout << "c_string iterable does not contain a size() method\n";
    // iterable does not contain a size() method because cstr does not have a size and is also not random access

    // This also creates a sized iterable
    using it = decltype(cstr.begin());
    using sentinel = decltype(cstr.end());
    // We need to specify that it is a sized iterable
    lz::sized_iterable<it, sentinel> sized_cstr_iterable(cstr.begin(), 5);
    std::cout << sized_cstr_iterable.size() << '\n'; // Output: 5
    // iterable now contains "Hello"

    // The following would get an error because cstr is not random access and there's no way to get the size
    // lz::sized_iterable<it, sentinel> sized_cstr_iterable(cstr.begin(), cstr.end());

    // With STL containers you can use the following:
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    lz::basic_iterable<std::vector<int>::iterator> vec_iterable(vec.begin(), vec.end());
    std::cout << vec_iterable.size() << '\n'; // Output: 5
}
