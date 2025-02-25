#pragma once

#ifndef LZ_ANY_VIEW_HELPERS_HPP
#define LZ_ANY_VIEW_HELPERS_HPP

#include <Lz/detail/iterators/anyview/iterator_base.hpp>
#include <Lz/iterator_base.hpp>
#include <iterator>

namespace lz {
namespace detail {

template<class T, class Reference, class IterCat, class DiffType>
class iterator_wrapper : public iterator<iterator_wrapper<T, Reference, IterCat, DiffType>, Reference, fake_ptr_proxy<Reference>,
                                         DiffType, IterCat> {

    using any_iter_base = iterator_base<Reference, IterCat, DiffType>;
    std::unique_ptr<any_iter_base> _implementation{};

public:
    using value_type = T;
    using reference = Reference;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = DiffType;
    using iterator_category = IterCat;

    constexpr iterator_wrapper() = default;

    iterator_wrapper(const std::unique_ptr<any_iter_base>& ptr) : _implementation{ ptr->clone() } {
    }

    iterator_wrapper(std::unique_ptr<any_iter_base>&& ptr) noexcept : _implementation{ std::move(ptr) } {
    }

    iterator_wrapper(const iterator_wrapper& other) : _implementation{ other._implementation->clone() } {
    }

    iterator_wrapper& operator=(const iterator_wrapper& other) {
        if (this != &other) {
            _implementation = other._implementation->clone();
        }
        return *this;
    }

    reference dereference() const {
        return _implementation->dereference();
    }

    pointer arrow() const {
        return _implementation->arrow();
    }

    void increment() {
        _implementation->increment();
    }

    void decrement() {
        _implementation->decrement();
    }

    bool eq(const iterator_wrapper& other) const {
        return _implementation->eq(*other._implementation);
    }

    void plus_is(const DiffType n) {
        _implementation->plus_is(n);
    }

    DiffType difference(const iterator_wrapper& other) const {
        return _implementation->minus(*other._implementation);
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_ANY_VIEW_HELPERS_HPP