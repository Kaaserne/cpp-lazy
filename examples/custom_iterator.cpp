#include <Lz/iter_tools.hpp>
#include <Lz/iterator_base.hpp>
#include <iostream>

class custom_fwd_iterator
    : public lz::iterator<
          /* Name of 'this' class */ custom_fwd_iterator, /* Reference type returned by operator*. */
          const int&,
          /* Pointer type returned by operator->()*/ const int*,
          /* Difference type returned by operator-() among others*/ std::ptrdiff_t,
          /* The iterator category. Forward only in this case */ std::forward_iterator_tag,
          /* The sentinel type. Can be custom_fwd_iterator or for e.g. default_sentinel */ custom_fwd_iterator> {
    int _iter;

public:
    using value_type = int;
    using reference = int&;
    using pointer = int*;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;

    custom_fwd_iterator(int iter) : _iter(iter) {
    }

    custom_fwd_iterator() = default;

    void increment() {
        ++_iter;
    }

    // Add const to the function
    const int& dereference() const {
        return _iter;
    }

    // Add const to the function
    const int* arrow() const {
        return &_iter;
    }

    // Add const to the function
    bool eq(const custom_fwd_iterator& other) const {
        return _iter == other._iter;
    }

    // Add this function overload if you're dealing with a sentinel. Add const too
    bool eq(lz::default_sentinel) const {
        return false; // Or something else
    }
};

class custom_bidi_iterator : public lz::iterator<custom_bidi_iterator, const int&, const int*, std::ptrdiff_t,
                                                 std::bidirectional_iterator_tag, custom_bidi_iterator> {
    int _iter;

public:
    using value_type = int;
    using reference = int&;
    using pointer = int*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;

    custom_bidi_iterator(int iter) : _iter(iter) {
    }

    custom_bidi_iterator() = default;

    void increment() {
        ++_iter;
    }

    // Added a decrement function
    void decrement() {
        --_iter;
    }

    // Add const to the function
    const int& dereference() const {
        return _iter;
    }

    // Add const to the function
    const int* arrow() const {
        return &_iter;
    }

    // Add const to the function
    bool eq(const custom_bidi_iterator& other) const {
        return _iter == other._iter;
    }

    // Add this function overload if you're dealing with a sentinel
    bool eq(lz::default_sentinel) const {
        return false; // Or something else
    }
};

class custom_ra_iterator : public lz::iterator<custom_ra_iterator, const int&, const int*, std::ptrdiff_t,
                                               std::random_access_iterator_tag, custom_ra_iterator> {
    int _iter;

public:
    using value_type = int;
    using reference = int&;
    using pointer = int*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;

    custom_ra_iterator(int iter) : _iter(iter) {
    }

    custom_ra_iterator() = default;

    void increment() {
        ++_iter;
    }

    void decrement() {
        --_iter;
    }

    // Add const to the function
    const int& dereference() const {
        return _iter;
    }

    // Add const to the function
    const int* arrow() {
        return &_iter;
    }

    // Add const to the function
    bool eq(const custom_ra_iterator& other) const {
        return _iter == other._iter;
    }

    // Add this function overload if you're dealing with a sentinel. Add const too
    bool eq(lz::default_sentinel) const {
        return false; // Or something else
    }

    // Added plus_is function
    void plus_is(difference_type n) {
        _iter += static_cast<int>(n);
    }

    // Added difference function, must be const
    difference_type difference(const custom_ra_iterator& other) const {
        return static_cast<difference_type>(_iter - other._iter);
    }
};


int main() {
    std::cout << "Custom forward iterator example, going forward.\n";
    auto view = lz::to_iterable(custom_fwd_iterator{ 0 }, custom_fwd_iterator{ 10 });
    for (/* const */ int& i : view) {
        std::cout << i << '\n';
    }
    std::cout << '\n';

    std::cout << "Custom bidirectional iterator example, going backward.\n";
    auto view2 = lz::to_iterable(custom_bidi_iterator{ 0 }, custom_bidi_iterator{ 10 });
    for (/* const */ int& i : lz::reverse(view2)) {
        std::cout << i << '\n';
    }
    std::cout << '\n';

    std::cout << "Custom random access iterator example, going forward.\n";
    auto view3 = lz::to_iterable(custom_ra_iterator{ 0 }, custom_ra_iterator{ 10 });
    for (/* const */ int& i : view3) {
        std::cout << i << '\n';
    }
    std::cout << '\n';

    std::cout << "Custom random access iterator example, going backward.\n";
    for (/* [const] */ int& i : lz::reverse(view3)) {
        std::cout << i << '\n';
    }
    std::cout << '\n';

    std::cout << "Custom random access iterator example, going forward with step 2.\n";
    auto it_start = view3.begin();
    std::cout << "*it_start = " << *it_start << '\n';
    std::cout << "*(it_start + 2) = " << *(it_start + 2) << '\n';
}
