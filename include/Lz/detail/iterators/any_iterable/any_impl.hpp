#pragma once

#ifndef LZ_ANY_VIEW_ITERATOR_IMPL_HPP
#define LZ_ANY_VIEW_ITERATOR_IMPL_HPP

#include <Lz/detail/iterators/common.hpp>
#include <Lz/detail/unique_ptr.hpp>
#include <Lz/iterator_base.hpp>
#include <iterator>

namespace lz {
namespace detail {
template<class Iter, class S, class T, class Reference, class IterCat, class DiffType>
class any_iterator_impl;

template<class Iter, class S, class T, class Reference, class DiffType>
class any_iterator_impl<Iter, S, T, Reference, std::forward_iterator_tag, DiffType> final
    : public iterator_base<Reference, std::forward_iterator_tag, DiffType> {

    common_iterator<Iter, S> _iter{};

    using any_iter_base = iterator_base<Reference, std::forward_iterator_tag, DiffType>;

public:
    using value_type = T;
    using reference = Reference;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = DiffType;
    using iterator_category = std::forward_iterator_tag;

    constexpr any_iterator_impl() = default;

    constexpr any_iterator_impl(Iter iter) : _iter{ std::move(iter) } {
    }

    constexpr any_iterator_impl(S s) : _iter{ std::move(s) } {
    }

    constexpr any_iterator_impl(common_iterator<Iter, S> iter) : _iter{ std::move(iter) } {
    }

    ~any_iterator_impl() override = default;

    reference dereference() override {
        return *_iter;
    }

    pointer arrow() override {
        return pointer{ dereference() };
    }

    void increment() override {
        ++_iter;
    }

    bool eq(const any_iter_base& other) const override {
        return _iter == static_cast<const any_iterator_impl&>(other)._iter;
    }

    detail::unique_ptr<any_iter_base> clone() const override {
        return detail::make_unique<any_iterator_impl>(_iter);
    }
};

template<class Iter, class T, class Reference, class DiffType>
class any_iterator_impl<Iter, Iter, T, Reference, std::forward_iterator_tag, DiffType> final
    : public iterator_base<Reference, std::forward_iterator_tag, DiffType> {

    Iter _iter;

    static_assert(std::is_same<Reference, decltype(*_iter)>::value,
                  "The iterator operator* returns a different type than template parameter `Reference`. Try adding/removing "
                  "`&` to `Reference`");

    using any_iter_base = iterator_base<Reference, std::forward_iterator_tag, DiffType>;

public:
    using value_type = T;
    using reference = Reference;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = DiffType;
    using iterator_category = std::forward_iterator_tag;

    constexpr any_iterator_impl() = default;

    constexpr any_iterator_impl(Iter iter) : _iter(std::move(iter)) {
    }

    ~any_iterator_impl() override = default;

    reference dereference() override {
        return *_iter;
    }

    pointer arrow() override {
        return pointer{ dereference() };
    }

    void increment() override {
        ++_iter;
    }

    bool eq(const any_iter_base& other) const override {
        return _iter == static_cast<const any_iterator_impl&>(other)._iter;
    }

    detail::unique_ptr<any_iter_base> clone() const override {
        return detail::make_unique<any_iterator_impl>(_iter);
    }
};

template<class Iter, class S, class T, class Reference, class DiffType>
class any_iterator_impl<Iter, S, T, Reference, std::bidirectional_iterator_tag, DiffType> final
    : public iterator_base<Reference, std::bidirectional_iterator_tag, DiffType> {
    Iter _iter;

    static_assert(std::is_same<Reference, decltype(*_iter)>::value,
                  "The iterator operator* returns a different type than template parameter `Reference`");

    using any_iter_base = iterator_base<Reference, std::bidirectional_iterator_tag, DiffType>;

public:
    using value_type = T;
    using reference = Reference;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = DiffType;
    using iterator_category = std::bidirectional_iterator_tag;

    constexpr any_iterator_impl() = default;

    constexpr any_iterator_impl(Iter iter) : _iter(std::move(iter)) {
    }

    ~any_iterator_impl() override = default;

    reference dereference() override {
        return *_iter;
    }

    pointer arrow() override {
        return pointer{ dereference() };
    }

    void increment() override {
        ++_iter;
    }

    void decrement() override {
        --_iter;
    }

    bool eq(const any_iter_base& other) const override {
        return _iter == static_cast<const any_iterator_impl&>(other)._iter;
    }

    detail::unique_ptr<any_iter_base> clone() const override {
        return detail::make_unique<any_iterator_impl>(_iter);
    }
};

template<class Iter, class S, class T, class Reference, class DiffType>
class any_iterator_impl<Iter, S, T, Reference, std::random_access_iterator_tag, DiffType> final
    : public iterator_base<Reference, std::random_access_iterator_tag, DiffType> {

    Iter _iter;
    static_assert(std::is_same<Reference, decltype(*_iter)>::value,
                  "The iterator operator* returns a different type than template parameter `Reference`");

    using any_iter_base = iterator_base<Reference, std::random_access_iterator_tag, DiffType>;

public:
    using value_type = T;
    using reference = Reference;
    using pointer = fake_ptr_proxy<Reference>;
    using difference_type = DiffType;
    using iterator_category = std::random_access_iterator_tag;

    constexpr any_iterator_impl() = default;

    constexpr any_iterator_impl(Iter iter) : _iter{ std::move(iter) } {
    }

    ~any_iterator_impl() override = default;

    reference dereference() override {
        return *_iter;
    }

    pointer arrow() override {
        return pointer{ dereference() };
    }

    void increment() override {
        ++_iter;
    }

    void decrement() override {
        --_iter;
    }

    bool eq(const any_iter_base& other) const override {
        return _iter == static_cast<const any_iterator_impl&>(other)._iter;
    }

    void plus_is(DiffType n) override {
        _iter += n;
    }

    DiffType minus(const any_iter_base& other) const override {
        return _iter - static_cast<const any_iterator_impl&>(other)._iter;
    }

    bool lt(const any_iter_base& other) const override {
        return _iter < static_cast<const any_iterator_impl&>(other)._iter;
    }

    detail::unique_ptr<any_iter_base> clone() const override {
        return detail::make_unique<any_iterator_impl>(_iter);
    }
};
} // namespace detail
} // namespace lz

#endif