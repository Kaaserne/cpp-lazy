#pragma once

#ifndef LZ_FLATTEN_ITERATOR_HPP
#define LZ_FLATTEN_ITERATOR_HPP

#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>

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
#ifdef LZ_HAS_CXX_11

    template<class T>
    using type =
        std::integral_constant<std::size_t, 1 + count_dims_helper<is_iterable<decltype(*std::begin(std::declval<T>()))>::value>::
                                                    template type<decltype(*std::begin(std::declval<T>()))>::value>;

#else

    template<class T>
    static constexpr std::size_t value =
        1 + count_dims_helper<is_iterable<decltype(*std::begin(std::declval<T>()))>::value>::template value<decltype(*std::begin(
                std::declval<T>()))>;

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
template<class T, class S, class = void>
class flatten_wrapper;

template<class Iterator, class S>
class flatten_wrapper<Iterator, S, enable_if<is_bidi<Iterator>::value>>
    : public iterator<flatten_wrapper<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      iter_cat_t<Iterator>, flatten_wrapper<Iterator, S>> {

    Iterator _begin;
    Iterator _iterator;
    S _end;

    using traits = std::iterator_traits<Iterator>;

public:
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;

    constexpr flatten_wrapper(Iterator current, Iterator begin, S end) :
        _begin{ std::move(begin) },
        _iterator{ std::move(current) },
        _end{ std::move(end) } {
    }

    constexpr flatten_wrapper() = default;

    LZ_CONSTEXPR_CXX_14 flatten_wrapper& operator=(default_sentinel) {
        _iterator = _end;
        return *this;
    }

    constexpr bool has_next() const {
        return _iterator != _end;
    }

    constexpr bool has_prev() const {
        return _iterator != _begin;
    }

    constexpr bool has_prev_inner() const {
        return has_prev();
    }

    constexpr bool has_next_inner() const {
        return has_next();
    }

    constexpr Iterator iterator() const {
        return _iterator;
    }

    LZ_CONSTEXPR_CXX_14 void iterator(Iterator c) {
        _iterator = std::move(c);
    }

    constexpr S end() const {
        return _end;
    }

    LZ_CONSTEXPR_CXX_14 void initialize_last() {
        _iterator = _end;
    }

    constexpr difference_type current_to_begin() const {
        return _iterator - _begin;
    }

    constexpr difference_type end_to_current() const {
        return _end - _iterator;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const flatten_wrapper& b) const {
        LZ_ASSERT(_begin == b._begin && _end == b._end, "Incompatible iterators");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
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

    LZ_CONSTEXPR_CXX_14 difference_type difference(const flatten_wrapper& other) const {
        return _iterator - other._iterator;
    }
};

template<class Iterator, class S>
class flatten_wrapper<Iterator, S, enable_if<!is_bidi<Iterator>::value>>
    : public iterator<flatten_wrapper<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      iter_cat_t<Iterator>, default_sentinel> {

    Iterator _iterator;
    S _end;

    using traits = std::iterator_traits<Iterator>;

public:
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;

    constexpr flatten_wrapper(Iterator /* iterator */, Iterator begin, S end) :
        _iterator{ std::move(begin) },
        _end{ std::move(end) } {
    }

    constexpr flatten_wrapper() = default;

    LZ_CONSTEXPR_CXX_14 flatten_wrapper& operator=(default_sentinel) {
        _iterator = _end;
        return *this;
    }

    constexpr bool has_next() const {
        return _iterator != _end;
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const flatten_wrapper& b) const {
        LZ_ASSERT(_end == b._end, "Incompatible iterators");
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator == _end;
    }

    constexpr Iterator iterator() const {
        return _iterator;
    }

    LZ_CONSTEXPR_CXX_14 void iterator(Iterator c) {
        _iterator = std::move(c);
    }

    constexpr S end() const {
        return _end;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
    }
};

template<class, class, std::size_t>
class flatten_iterator;

template<class Iterator, std::size_t N>
using inner =
    flatten_iterator<decltype(std::begin(*std::declval<Iterator>())), decltype(std::end(*std::declval<Iterator>())), N - 1>;

template<class Iterator, class S, std::size_t N>
using iter_cat = common_type<iter_cat_t<inner<Iterator, N>>, iter_cat_t<flatten_wrapper<Iterator, S>>>;

template<class Iterator, class S, std::size_t N>
class flatten_iterator
    : public iterator<flatten_iterator<Iterator, S, N>, ref_t<inner<Iterator, N>>, fake_ptr_proxy<ref_t<inner<Iterator, N>>>,
                      diff_type<inner<Iterator, N>>, iter_cat<Iterator, S, N>,
                      sentinel_selector<iter_cat<Iterator, S, N>, flatten_iterator<Iterator, S, N>>> {

    using this_inner = inner<Iterator, N>;

    LZ_CONSTEXPR_CXX_14 void find_next_non_empty_inner() {
        using lz::detail::find_if;
        using std::find_if;
        using ref = decltype(*_outer_iter.iterator());

        ++_outer_iter;
        _outer_iter.iterator(find_if(_outer_iter.iterator(), _outer_iter.end(), [this](const ref& inner) {
            _inner_iter = this_inner(std::begin(inner), std::begin(inner), std::end(inner));
            return _inner_iter.has_next();
        }));
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
        if (_inner_iter.has_next()) {
            return;
        }
    }

    LZ_CONSTEXPR_CXX_14 void previous_outer() {
        --_outer_iter;
        _inner_iter = this_inner(std::end(*_outer_iter), std::begin(*_outer_iter), std::end(*_outer_iter));
    }

    LZ_CONSTEXPR_CXX_14 void try_previous_inner() {
        previous_outer();
        if (_inner_iter.has_prev()) {
            --_inner_iter;
        }
    }

    flatten_wrapper<Iterator, S> _outer_iter;
    this_inner _inner_iter;

public:
    LZ_CONSTEXPR_CXX_14 flatten_iterator(Iterator it, Iterator begin, S end) :
        _outer_iter{ std::move(it), std::move(begin), std::move(end) } {
        if (_outer_iter.has_next()) {
            _inner_iter = this_inner(std::begin(*_outer_iter), std::begin(*_outer_iter), std::end(*_outer_iter));
            this->advance();
        }
    }

    constexpr flatten_iterator() = default;

    LZ_CONSTEXPR_CXX_14 flatten_iterator operator=(default_sentinel) {
        _inner_iter = default_sentinel{};
        _outer_iter = default_sentinel{};
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
        return _outer_iter == b._outer_iter;
    }

    constexpr bool eq(default_sentinel) const {
        return !has_next();
    }

    constexpr reference dereference() const {
        return *_inner_iter;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
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
            _inner_iter = this_inner();
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

        total -= this_inner(std::end(*outer_iter), std::begin(*outer_iter), std::end(*outer_iter)) - _inner_iter;
        if (other._outer_iter.has_next()) {
            total += this_inner(std::begin(*other._outer_iter), std::begin(*other._outer_iter), std::end(*other._outer_iter)) -
                     other._inner_iter;
        }

        for (++outer_iter; outer_iter != other._outer_iter; ++outer_iter) {
            total -= (this_inner(std::end(*outer_iter), std::begin(*outer_iter), std::end(*outer_iter)) -
                      this_inner(std::begin(*outer_iter), std::begin(*outer_iter), std::end(*outer_iter)));
        }

        return total;
    }
};

template<class Iterator, class S>
class flatten_iterator<Iterator, S, 0>
    : public iterator<flatten_iterator<Iterator, S, 0>, ref_t<flatten_wrapper<Iterator, S>>,
                      fake_ptr_proxy<ref_t<flatten_wrapper<Iterator, S>>>, diff_type<flatten_wrapper<Iterator, S>>,
                      iter_cat_t<flatten_wrapper<Iterator, S>>,
                      sentinel_selector<iter_cat_t<flatten_wrapper<Iterator, S>>, flatten_iterator<Iterator, S, 0>>> {

    flatten_wrapper<Iterator, S> _iterator;
    using traits = std::iterator_traits<Iterator>;

public:
    using pointer = typename traits::pointer;
    using reference = typename traits::reference;
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;

    constexpr flatten_iterator() = default;

    constexpr flatten_iterator(Iterator it, Iterator begin, S end) :
        _iterator{ std::move(it), std::move(begin), std::move(end) } {
    }

    LZ_CONSTEXPR_CXX_14 flatten_iterator& operator=(default_sentinel) {
        _iterator = default_sentinel{};
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

    void initialize_last() {
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

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    constexpr bool eq(const flatten_iterator& b) const {
        return _iterator == b._iterator;
    }

    constexpr bool eq(default_sentinel) const {
        return _iterator.eq(default_sentinel{});
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
};
} // namespace detail
} // namespace lz

#endif // LZ_FLATTEN_ITERATOR_HPP
