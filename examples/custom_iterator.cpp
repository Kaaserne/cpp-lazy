#include <Lz/iter_tools.hpp>
#include <Lz/iterator_base.hpp>
#include <Lz/range.hpp>
#include <Lz/reverse.hpp>
#include <iostream>
#include <vector>

class custom_fwd_iterator
    : public lz::iterator<
          /* Name of 'this' class */ custom_fwd_iterator, /* Reference type returned by operator*. */
          int&,
          /* Pointer type returned by operator->()*/ int*,
          /* Difference type returned by operator-() among others*/ std::ptrdiff_t,
          /* The iterator category. Forward only in this case */ std::forward_iterator_tag,
          /* The sentinel type. Can be custom_fwd_iterator or for e.g. default_sentinel */ custom_fwd_iterator> {
    int _iter;

public:
    using value_type = int;
    using reference = int&;
    using pointer = int*;
    using difference_type = std::ptrdiff_t;

    custom_fwd_iterator(int iter) : _iter(iter) {
    }

    custom_fwd_iterator() = default;

    void increment() {
        ++_iter;
    }

    const int& dereference() const {
        return _iter;
    }

    int& dereference() {
        return _iter;
    }

    const int* arrow() const {
        return &_iter;
    }

    int* arrow() {
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

class custom_bidi_iterator : public lz::iterator<custom_bidi_iterator, int&, int*, std::ptrdiff_t,
                                                 std::bidirectional_iterator_tag, custom_bidi_iterator> {
    int _iter;

public:
    using value_type = int;
    using reference = int&;
    using pointer = int*;
    using difference_type = std::ptrdiff_t;

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

    const int& dereference() const {
        return _iter;
    }

    int& dereference() {
        return _iter;
    }

    const int* arrow() const {
        return &_iter;
    }

    int* arrow() {
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

class custom_ra_iterator
    : public lz::iterator<custom_ra_iterator, int&, int*, std::ptrdiff_t, std::random_access_iterator_tag, custom_ra_iterator> {
    int _iter;

public:
    using value_type = int;
    using reference = int&;
    using pointer = int*;
    using difference_type = std::ptrdiff_t;

    custom_ra_iterator(int iter) : _iter(iter) {
    }

    custom_ra_iterator() = default;

    void increment() {
        ++_iter;
    }

    void decrement() {
        --_iter;
    }

    const int& dereference() const {
        return _iter;
    }

    int& dereference() {
        return _iter;
    }

    const int* arrow() const {
        return &_iter;
    }

    int* arrow() {
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

// If you want to create your own iterable, you can use the following code:
// It is important you inherit from lz::lazy_view if you use lz::ref_or_view as a member variable.

template<class Iterable>
class custom_iterable : public lz::lazy_view {
    // Holds a reference or a copy of the iterable, depending whether Iterable is inherited from lazy_view
    // So std::vector<int> will be hold by reference, but lz::range(0, 10) will be copied
    lz::ref_or_view<Iterable> _iterable;

public:
    custom_iterable(Iterable& iterable) : _iterable(iterable) {
    }

    custom_iterable(const Iterable& iterable) : _iterable(iterable) {
    }

    // or (and remove the above two constructors):
    // template<class I>
    // custom_iterable(I&& iterable) : _iterable(std::forward<I>(iterable)) {}

    // The begin function
    lz::iter_t<Iterable> begin() const {
        return std::begin(_iterable);
    }

    // The end function
    lz::sentinel_t<Iterable> end() const {
        return std::end(_iterable);
    }
};

template<class Iterable>
custom_iterable<typename std::remove_reference<Iterable>::type> make_custom_iterable(Iterable&& iterable) {
    return { iterable };
}

int main() {

    // ---------------- Custom iterable example ----------------
    // Custom iterable example, with for e.g. std::vector. We add a reference to the vector so that the contents are not copied
    std::cout << "Custom iterable example.\n";
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    custom_iterable<std::vector<int>> custom(vec);
    static_cast<void>(custom); // To silence the warning

    // for lz iterators however, you can use the following code:
    using iterable = decltype(lz::range(0, 10));
    custom_iterable<iterable> custom2(lz::range(0, 10));
    // We don't need to pass a reference because only if the iterable is NOT base of lazy_view, it will be copied
    // If it is base of lazy_view, it must be passed as a reference.
    // But if you don't want all that boiler plate code, you can use the following (prefered) function:
    auto custom3 = make_custom_iterable(lz::range(0, 10));
    for (int i : custom3) {
        std::cout << i << '\n';
    }

    std::cout << '\n';

    // ---------------- Custom iterator example ----------------
    std::cout << "Custom forward iterator example, going forward.\n";
    lz::basic_iterable<custom_fwd_iterator> view(custom_fwd_iterator{ 0 }, custom_fwd_iterator{ 10 });
    for (/* const */ int& i : view) {
        std::cout << i << '\n';
    }
    std::cout << '\n';

    std::cout << "Custom bidirectional iterator example, going backward.\n";
    lz::basic_iterable<custom_bidi_iterator> view1(custom_bidi_iterator{ 0 }, custom_bidi_iterator{ 10 });
    for (/* const */ int& i : lz::reverse(view1)) {
        std::cout << i << '\n';
    }
    std::cout << '\n';

    std::cout << "Custom random access iterator example, going forward.\n";
    lz::basic_iterable<custom_ra_iterator> view2(custom_ra_iterator{ 0 }, custom_ra_iterator{ 10 });
    for (/* const */ int& i : view2) {
        std::cout << i << '\n';
    }
    std::cout << '\n';

    std::cout << "Custom random access iterator example, going backward.\n";
    for (/* [const] */ int& i : lz::reverse(view2)) {
        std::cout << i << '\n';
    }
    std::cout << '\n';

    std::cout << "Custom random access iterator example, going forward with step 2.\n";
    auto it_start = view2.begin();
    std::cout << "*it_start = " << *it_start << '\n';
    std::cout << "*(it_start + 2) = " << *(it_start + 2) << '\n';
}
