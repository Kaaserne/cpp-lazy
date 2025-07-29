#pragma once

#ifndef LZ_FLATTEN_ITERATOR_HPP
#define LZ_FLATTEN_ITERATOR_HPP

#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/maybe_owned.hpp>
#include <Lz/iterator_base.hpp>
#include <iterator>

namespace lz {
namespace detail {
template<bool>
struct count_dims_helper;

template<>
struct count_dims_helper<false> {
#ifdef LZ_HAS_CXX_11

    template<class>
    using type = std::integral_constant<std::size_t, 0>;

#else

    template<class>
    static constexpr std::size_t value = 0;

#endif
};

template<>
struct count_dims_helper<true> {
    template<class T>
    using iterable_type = decltype(*std::begin(std::declval<T>()));

#ifdef LZ_HAS_CXX_11

    template<class T>
    using type = std::integral_constant<
        std::size_t, 1 + count_dims_helper<is_iterable<iterable_type<T>>::value>::template type<iterable_type<T>>::value>;

#else

    template<class T>
    static constexpr std::size_t value =
        1 + count_dims_helper<is_iterable<iterable_type<T>>::value>::template value<iterable_type<T>>;

#endif
};

#ifdef LZ_HAS_CXX_11

template<class T>
using count_dims = typename count_dims_helper<is_iterable<T>::value>::template type<T>;

#else

template<class T>
using count_dims = std::integral_constant<std::size_t, count_dims_helper<is_iterable<T>::value>::template value<T>>;

#endif

// Improvement of https://stackoverflow.com/a/21076724/8729023
template<class Iterable>
class flatten_wrapper
    : public iterator<flatten_wrapper<Iterable>, ref_t<iter_t<Iterable>>, fake_ptr_proxy<ref_t<iter_t<Iterable>>>,
                      diff_type<iter_t<Iterable>>, iter_cat_t<iter_t<Iterable>>, default_sentinel_t> {

    using iter = iter_t<Iterable>;

    iter _iterator{};
    maybe_owned<Iterable> _iterable;

    using traits = std::iterator_traits<iter>;

public:
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;

    template<class I>
    constexpr flatten_wrapper(I&& iterable, iter it) : _iterator{ std::move(it) }, _iterable{ std::forward<I>(iterable) } {
    }

#ifdef LZ_HAS_CONCEPTS

    constexpr flatten_wrapper()
        requires std::default_initializable<iter> && std::default_initializable<maybe_owned<Iterable>>
    = default;

#else

    template<class I = iter, class = enable_if<std::is_default_constructible<I>::value &&
                                               std::is_default_constructible<maybe_owned<Iterable>>::value>>
    constexpr flatten_wrapper() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<maybe_owned<Iterable>>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14 flatten_wrapper& operator=(default_sentinel_t) {
        initialize_last();
        return *this;
    }

    constexpr bool has_next() const {
        return _iterator != std::end(_iterable);
    }

    constexpr bool has_prev() const {
        return _iterator != std::begin(_iterable);
    }

    constexpr bool has_prev_inner() const {
        return has_prev();
    }

    constexpr bool has_next_inner() const {
        return has_next();
    }

    constexpr iter iterator() const {
        return _iterator;
    }

    LZ_CONSTEXPR_CXX_14 void iterator(iter c) {
        _iterator = std::move(c);
    }

    constexpr sentinel_t<Iterable> end() const {
        return std::end(_iterable);
    }

    LZ_CONSTEXPR_CXX_14 void initialize_last() {
        _iterator = std::end(_iterable);
    }

    constexpr difference_type current_to_begin() const {
        return _iterator - std::begin(_iterable);
    }

    constexpr difference_type end_to_current() const {
        return std::end(_iterable) - _iterator;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const flatten_wrapper& b) const {
        LZ_ASSERT(std::begin(_iterable) == std::begin(b._iterable) && std::end(_iterable) == std::end(b._iterable),
                  "Incompatible iterators");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator == std::end(_iterable);
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        ++_iterator;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        LZ_ASSERT(_iterator != std::begin(_iterable), "Cannot decrement begin iterator");
        --_iterator;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type n) {
        LZ_ASSERT_SUB_ADDABLE(n < 0 ? -n <= _iterator - std::begin(_iterable) : n <= std::end(_iterable) - _iterator);
        _iterator += n;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const flatten_wrapper& other) const {
        LZ_ASSERT(std::begin(_iterable) == std::begin(other._iterable) && std::end(_iterable) == std::end(other._iterable),
                  "Incompatible iterators");
        return _iterator - other._iterator;
    }

    constexpr difference_type difference(default_sentinel_t) const {
        return _iterator - std::end(_iterable);
    }
};

template<class, std::size_t>
class flatten_iterator;

template<class Iterable, std::size_t N>
using inner = flatten_iterator<remove_ref<ref_iterable_t<Iterable>>, N - 1>;

template<class Iterable, std::size_t N>
using iter_cat = common_type<iter_cat_t<inner<Iterable, N>>, iter_cat_t<flatten_wrapper<Iterable>>>;

template<class Iterable, std::size_t N>
class flatten_iterator
    : public iterator<flatten_iterator<Iterable, N>, ref_t<inner<Iterable, N>>, fake_ptr_proxy<ref_t<inner<Iterable, N>>>,
                      diff_type<inner<Iterable, N>>, iter_cat<Iterable, N>, default_sentinel_t> {

    using iter = iter_t<Iterable>;
    using this_inner = inner<Iterable, N>;

    LZ_CONSTEXPR_CXX_14 void find_next_non_empty_inner() {
        using lz::detail::find_if;
        using std::find_if;
        using ref = decltype(*_outer_iter.iterator());

        ++_outer_iter;
        _outer_iter.iterator(find_if(_outer_iter.iterator(), _outer_iter.end(), [this](ref inner) {
            _inner_iter = this_inner(inner, std::begin(inner));
            return _inner_iter.has_next();
        }));

        if (_inner_iter.has_next()) {
            return;
        }
        _inner_iter = this_inner{};
    }

    LZ_CONSTEXPR_CXX_14 void find_prev_non_empty_inner() {
        while (!_inner_iter.has_prev()) {
            if (!_outer_iter.has_prev()) {
                return;
            }
            previous_outer();
        }
    }

public:
    using reference = typename this_inner::reference;
    using pointer = typename this_inner::pointer;
    using value_type = typename this_inner::value_type;
    using difference_type = typename this_inner::difference_type;

private:
    LZ_CONSTEXPR_CXX_14 void advance() {
        if (_inner_iter.has_next()) {
            return;
        }

        find_next_non_empty_inner();
    }

#ifdef LZ_HAS_CXX_17

    constexpr void previous_outer() {
        --_outer_iter;
        if constexpr (!is_sentinel<iter_t<Iterable>, sentinel_t<Iterable>>::value) {
            _inner_iter = this_inner(*_outer_iter, std::end(*_outer_iter));
        }
        else {
            --_outer_iter;
            _inner_iter =
                this_inner(*_outer_iter, std::begin(*_outer_iter) + (std::end(*_outer_iter) - std::begin(*_outer_iter)));
        }
    }

#else

    template<class I = iter_t<Iterable>>
    LZ_CONSTEXPR_CXX_14 enable_if<!is_sentinel<I, sentinel_t<Iterable>>::value> previous_outer() {
        --_outer_iter;
        _inner_iter = this_inner(*_outer_iter, std::end(*_outer_iter));
    }

    template<class I = iter_t<Iterable>>
    LZ_CONSTEXPR_CXX_14 enable_if<is_sentinel<I, sentinel_t<Iterable>>::value> previous_outer() {
        --_outer_iter;
        _inner_iter = this_inner(*_outer_iter, std::begin(*_outer_iter) + (std::end(*_outer_iter) - std::begin(*_outer_iter)));
    }

#endif

    LZ_CONSTEXPR_CXX_14 void try_previous_inner() {
        previous_outer();
        if (_inner_iter.has_prev()) {
            --_inner_iter;
        }
    }

    flatten_wrapper<Iterable> _outer_iter;
    this_inner _inner_iter;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr flatten_iterator()
        requires std::default_initializable<flatten_wrapper<Iterable>> && std::default_initializable<this_inner>
    = default;

#else

    template<class I = decltype(_outer_iter),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<this_inner>::value>>
    constexpr flatten_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                          std::is_nothrow_default_constructible<this_inner>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 flatten_iterator(I&& iterable, iter it) : _outer_iter{ std::forward<I>(iterable), std::move(it) } {
        if (_outer_iter.has_next()) {
            _inner_iter = this_inner(*_outer_iter, std::begin(*_outer_iter));
            this->advance();
        }
    }

    constexpr flatten_iterator() = default;

    LZ_CONSTEXPR_CXX_14 flatten_iterator operator=(default_sentinel_t) {
        _inner_iter = lz::default_sentinel;
        _outer_iter = lz::default_sentinel;
        return *this;
    }

    constexpr bool has_next() const {
        return _outer_iter.has_next();
    }

    constexpr bool has_prev() const {
        return _outer_iter.has_prev() || _inner_iter.has_prev();
    }

    constexpr bool has_prev_inner() const {
        return _inner_iter.has_prev_inner();
    }

    constexpr bool has_next_inner() const {
        return _inner_iter.has_next();
    }

    constexpr difference_type current_to_begin() const {
        return _inner_iter.current_to_begin();
    }

    constexpr difference_type end_to_current() const {
        return _inner_iter.end_to_current();
    }

    constexpr bool eq(const flatten_iterator& b) const {
        return _outer_iter == b._outer_iter && _inner_iter == b._inner_iter;
    }

    constexpr bool eq(default_sentinel_t) const {
        return !has_next();
    }

    constexpr reference dereference() const {
        return *_inner_iter;
    }

    constexpr pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_inner_iter;
        this->advance();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        if (!_outer_iter.has_next()) {
            initialize_last();
            --_inner_iter;
            return;
        }

        if (_inner_iter.has_prev()) {
            --_inner_iter;
            if (_inner_iter.has_next_inner()) {
                return;
            }
        }

        find_prev_non_empty_inner();
        if (_inner_iter.has_prev()) {
            --_inner_iter;
        }
    }

    LZ_CONSTEXPR_CXX_14 void initialize_last() {
        if (_inner_iter.has_prev_inner()) {
            // inner is not empty, return
            return;
        }

        find_prev_non_empty_inner();
        if (_inner_iter.has_prev_inner()) {
            // inner is not empty, return
            return;
        }
        // inner is empty, proceed to previous
        _inner_iter.initialize_last();
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(difference_type n) {
        while (n > 0) {
            // Check the distance of the inner iterator to the end
            const auto inner_distance = _inner_iter.end_to_current();
            if (n < inner_distance) {
                // n fits, just increment the inner iterator
                _inner_iter += n;
                return;
            }
            // n doesn't fit, increment the inner iterator to the end
            _inner_iter += inner_distance;
            n -= inner_distance;
            // Check if the inner iterator (possibily recursively) has next
            if (_inner_iter.has_next()) {
                continue;
            }
            // Inner didnt have any elements left, go to next outer
            find_next_non_empty_inner();
            // Check if the find_if found an inner iterator with elements
            if (_inner_iter.has_next()) {
                continue;
            }
            // Outer iterator has no next, we are done. Set _inner_iter to end/empty
            _inner_iter = this_inner{};
            return;
        }

        while (n < 0) {
            // Because end may be empty, initialized with = {}, we initialize all iterators with end
            initialize_last();

            // Get inner distance
            difference_type inner_distance = _inner_iter.current_to_begin();
            if (-n <= inner_distance) {
                // Fits
                _inner_iter += n;
                return;
            }

            n += inner_distance;
            // Doesn't fit, decrement the inner iterator to the beginning, possibly recursively
            _inner_iter -= inner_distance;

            if (_inner_iter.has_prev()) {
                // _inner_iter.outer still has previous elements
                continue;
            }

            // Get the previous non empty inner iterator
            find_prev_non_empty_inner();
        }
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const flatten_iterator& other) const {
        if (_outer_iter == other._outer_iter) {
            return _inner_iter - other._inner_iter;
        }

        if (_outer_iter > other._outer_iter) {
            // Make sure to make distance negative or positive again
            return -other.difference(*this);
        }

        difference_type total = 0;
        auto outer_iter = _outer_iter;

        if (outer_iter.has_next()) {
            // If the first outer iterator has next, we need to subtract the distance from the inner iterator
            total -= this_inner(*outer_iter, std::begin(*outer_iter) + (std::end(*outer_iter) - std::begin(*outer_iter))) -
                     _inner_iter;
        }
        if (other._outer_iter.has_next()) {
            total += this_inner(*other._outer_iter, std::begin(*other._outer_iter)) - other._inner_iter;
        }

        for (++outer_iter; outer_iter != other._outer_iter; ++outer_iter) {
            total -= (this_inner(*outer_iter, std::begin(*outer_iter) + (std::end(*outer_iter) - std::begin(*outer_iter))) -
                      this_inner(*outer_iter, std::begin(*outer_iter)));
        }

        return total;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(default_sentinel_t) const {
        difference_type total = 0;
        auto outer_iter = _outer_iter;

        if (outer_iter.has_next()) {
            total -= this_inner(*outer_iter, std::begin(*outer_iter) + (std::end(*outer_iter) - std::begin(*outer_iter))) -
                     _inner_iter;
        }
        for (++outer_iter; outer_iter.has_next(); ++outer_iter) {
            total -= (this_inner(*outer_iter, std::begin(*outer_iter) + (std::end(*outer_iter) - std::begin(*outer_iter))) -
                      this_inner(*outer_iter, std::begin(*outer_iter)));
        }

        return total;
    }
};

template<class Iterable>
class flatten_iterator<Iterable, 0>
    : public iterator<flatten_iterator<Iterable, 0>, ref_t<flatten_wrapper<Iterable>>,
                      fake_ptr_proxy<ref_t<flatten_wrapper<Iterable>>>, diff_type<flatten_wrapper<Iterable>>,
                      iter_cat_t<flatten_wrapper<Iterable>>, default_sentinel_t> {

    using iter = iter_t<Iterable>;
    using traits = std::iterator_traits<iter>;

    flatten_wrapper<Iterable> _iterator;

public:
    using pointer = typename traits::pointer;
    using reference = typename traits::reference;
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr flatten_iterator()
        requires std::default_initializable<flatten_wrapper<Iterable>>
    = default;

#else

    template<class I = decltype(_iterator), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr flatten_iterator() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    constexpr flatten_iterator(I&& iterable, iter it) : _iterator{ std::forward<I>(iterable), std::move(it) } {
    }

    LZ_CONSTEXPR_CXX_14 flatten_iterator& operator=(default_sentinel_t) {
        _iterator = lz::default_sentinel;
        return *this;
    }

    constexpr bool has_prev() const {
        return _iterator.has_prev();
    }

    constexpr bool has_next() const {
        return _iterator.has_next();
    }

    constexpr bool has_prev_inner() const {
        return _iterator.has_prev_inner();
    }

    constexpr bool has_next_inner() const {
        return _iterator.has_next_inner();
    }

    LZ_CONSTEXPR_CXX_14 void initialize_last() {
        _iterator.initialize_last();
    }

    constexpr difference_type current_to_begin() const {
        return _iterator.current_to_begin();
    }

    constexpr difference_type end_to_current() const {
        return _iterator.end_to_current();
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    constexpr pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    constexpr bool eq(const flatten_iterator& b) const {
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _iterator == lz::default_sentinel;
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_iterator;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type n) {
        _iterator += n;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const flatten_iterator& other) const {
        return _iterator - other._iterator;
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(default_sentinel_t) const {
        return _iterator - lz::default_sentinel;
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_FLATTEN_ITERATOR_HPP
