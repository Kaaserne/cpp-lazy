#include <Lz/map.hpp>
#include <iostream>

struct non_lz_iterable {
    const int* _begin{};
    const int* _end{};

    non_lz_iterable(const int* begin, const int* end) : _begin{ begin }, _end{ end } {
    }

    const int* begin() {
        return _begin;
    }
    const int* end() {
        return _end;
    }
};

int main() {
    // ref_or_view is a helper class that can be used to store a reference or copy of an iterable, depending on the type of the
    // iterable.
    // If the iterable is inherited from lazy_view, it will store a copy of the iterable. In all other cases, it will store a
    // reference to the iterable.

    const std::vector<int> vec{ 1, 2, 3 };
    // will store a reference to the vector because it is not inherited from lazy_view
    lz::ref_or_view<const std::vector<int>> ref_or_view{ vec };
    std::cout << "vec.begin() == ref_or_view.begin()? " << std::boolalpha << (&(*vec.begin()) == &(*ref_or_view.begin()))
              << '\n'; // true
    static_assert(lz::ref_or_view<std::vector<int>>::holds_reference, "Iterable should hold a reference");

    non_lz_iterable it{ vec.data(), vec.data() + vec.size() };
    // will store a reference to the vector because it is not inherited from lazy_view
    lz::ref_or_view<non_lz_iterable> ref{ it };
    static_assert(lz::ref_or_view<non_lz_iterable>::holds_reference, "Iterable should hold a reference");

    // Sometimes, you don't want this behaviour, for example for other cheap to copy, non lz iterables (like `non_lz_iterable`).
    // In this case, you can use lz::copied_iterable/lz::as_copied_iterable to store a copy of the iterable, instead of a
    // reference.
    lz::copied_iterable<non_lz_iterable> copied{ it }; // will *not* store a reference to the non lz iterable!
    static_assert(!lz::copied_iterable<non_lz_iterable>::holds_reference, "Iterable should hold a reference");

    // You can also use the helper function
    copied = lz::as_copied_iterable(it); // will *not* store a copy of the non lz iterable
    static_assert(!lz::ref_or_view<decltype(copied)>::holds_reference, "Iterable should hold a reference");
}
