#pragma once

#ifndef LZ_ANY_VIEW_ITERATOR_IMPL_HPP
#define LZ_ANY_VIEW_ITERATOR_IMPL_HPP

#include <Lz/detail/iterators/any_iterable/iterator_base.hpp>
#include <Lz/detail/iterators/common.hpp>
#include <Lz/detail/traits/conditional.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>
#include <Lz/detail/unique_ptr.hpp>
#include <iterator>

namespace lz {
namespace detail {
template<class Iter, class S, class T, class Reference, class IterCat, class DiffType>
class any_iterator_impl;

template<class Iter, class S, class T, class Reference, class DiffType>
class any_iterator_impl<Iter, S, T, Reference, std::forward_iterator_tag, DiffType> final
    : public iterator_base<Reference, std::forward_iterator_tag, DiffType> {

    static constexpr bool is_sent = is_sentinel<S, Iter>::value;
    using iter_type = conditional_t<!is_sent, Iter, common_iterator<Iter, S>>;
    iter_type _iter{};

    using any_iter_base = iterator_base<Reference, std::forward_iterator_tag, DiffType>;

public:
    using value_type = T;
    using reference = Reference;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = DiffType;
    using iterator_category = std::forward_iterator_tag;

    constexpr any_iterator_impl(const any_iterator_impl&) = default;
    LZ_CONSTEXPR_CXX_14 any_iterator_impl& operator=(const any_iterator_impl&) = default;

#ifdef LZ_HAS_CXX_20

    constexpr any_iterator_impl()
        requires(std::default_initializable<iter_type>)
    = default;

#else

    template<class I = iter_type, class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr any_iterator_impl() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    constexpr any_iterator_impl(Iter iter) : _iter{ std::move(iter) } {
    }

#ifdef LZ_HAS_CONCEPTS

    constexpr any_iterator_impl(S s)
        requires(is_sent)
        : _iter{ std::move(s) } {
    }

    constexpr any_iterator_impl(common_iterator<Iter, S> iter)
        requires(is_sent)
        : _iter{ std::move(iter) } {
    }

#else

    template<bool I = is_sent, class = enable_if_t<I>>
    constexpr any_iterator_impl(S s) : _iter{ std::move(s) } {
    }

    template<bool I = is_sent, class = enable_if_t<I>>
    constexpr any_iterator_impl(common_iterator<Iter, S> iter) : _iter{ std::move(iter) } {
    }

#endif

    ~any_iterator_impl() override = default;

    reference dereference() override {
        return *_iter;
    }

    reference dereference() const override {
        return *_iter;
    }

    pointer arrow() override {
        return pointer{ dereference() };
    }

    pointer arrow() const override {
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

    static constexpr bool is_sent = is_sentinel<S, Iter>::value;
    using iter_type = conditional_t<!is_sent, Iter, common_iterator<Iter, S>>;
    iter_type _iter{};

    using any_iter_base = iterator_base<Reference, std::bidirectional_iterator_tag, DiffType>;

public:
    using value_type = T;
    using reference = Reference;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = DiffType;
    using iterator_category = std::bidirectional_iterator_tag;

    constexpr any_iterator_impl(const any_iterator_impl&) = default;
    LZ_CONSTEXPR_CXX_14 any_iterator_impl& operator=(const any_iterator_impl&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr any_iterator_impl()
        requires(std::default_initializable<iter_type>)
    = default;

#else

    template<class I = iter_type, class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr any_iterator_impl() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    constexpr any_iterator_impl(Iter iter) : _iter{ std::move(iter) } {
    }

#ifdef LZ_HAS_CONCEPTS

    constexpr any_iterator_impl(S s)
        requires(is_sent)
        : _iter{ std::move(s) } {
    }

    constexpr any_iterator_impl(common_iterator<Iter, S> iter)
        requires(is_sent)
        : _iter{ std::move(iter) } {
    }

#else

    template<bool I = is_sent, class = enable_if_t<I>>
    constexpr any_iterator_impl(S iter) : _iter{ std::move(iter) } {
    }

    template<bool I = is_sent, class = enable_if_t<I>>
    constexpr any_iterator_impl(common_iterator<Iter, S> iter) : _iter{ std::move(iter) } {
    }

#endif

    ~any_iterator_impl() override = default;

    reference dereference() override {
        return *_iter;
    }

    reference dereference() const override {
        return *_iter;
    }

    pointer arrow() override {
        return pointer{ dereference() };
    }

    pointer arrow() const override {
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

    static constexpr bool is_sent = is_sentinel<S, Iter>::value;
    using iter_type = conditional_t<!is_sent, Iter, common_iterator<Iter, S>>;
    iter_type _iter{};

    using any_iter_base = iterator_base<Reference, std::random_access_iterator_tag, DiffType>;

public:
    using value_type = T;
    using reference = Reference;
    using pointer = fake_ptr_proxy<Reference>;
    using difference_type = DiffType;
    using iterator_category = std::random_access_iterator_tag;

    constexpr any_iterator_impl(const any_iterator_impl&) = default;
    LZ_CONSTEXPR_CXX_14 any_iterator_impl& operator=(const any_iterator_impl&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr any_iterator_impl()
        requires(std::default_initializable<iter_type>)
    = default;

#else

    template<class I = iter_type, class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr any_iterator_impl() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    constexpr any_iterator_impl(Iter iter) : _iter{ std::move(iter) } {
    }

#ifdef LZ_HAS_CONCEPTS

    constexpr any_iterator_impl(S s)
        requires(is_sent)
        : _iter{ std::move(s) } {
    }

    constexpr any_iterator_impl(common_iterator<Iter, S> iter)
        requires(is_sent)
        : _iter{ std::move(iter) } {
    }

#else

    template<bool I = is_sent, class = enable_if_t<I>>
    constexpr any_iterator_impl(S iter) : _iter{ std::move(iter) } {
    }

    template<bool I = is_sent, class = enable_if_t<I>>
    constexpr any_iterator_impl(common_iterator<Iter, S> iter) : _iter{ std::move(iter) } {
    }

#endif

    ~any_iterator_impl() override = default;

    reference dereference() override {
        return *_iter;
    }

    reference dereference() const override {
        return *_iter;
    }

    pointer arrow() override {
        return pointer{ dereference() };
    }

    pointer arrow() const override {
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

    DiffType difference(const any_iter_base& other) const override {
        return _iter - static_cast<const any_iterator_impl&>(other)._iter;
    }

    detail::unique_ptr<any_iter_base> clone() const override {
        return detail::make_unique<any_iterator_impl>(_iter);
    }
};
} // namespace detail
} // namespace lz

#endif
