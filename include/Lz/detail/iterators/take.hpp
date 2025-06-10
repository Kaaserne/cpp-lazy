#pragma once

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>
#include <iterator>

namespace lz {
namespace detail {
template<class Iterator>
class n_take_iterator : public iterator<n_take_iterator<Iterator>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                                        diff_type<Iterator>, iter_cat_t<Iterator>,
                                        sentinel_selector<iter_cat_t<Iterator>, n_take_iterator<Iterator>, default_sentinel>> {

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterator _iterator;
    std::size_t _n{};

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr n_take_iterator()
        requires std::default_initializable<Iterator>
    = default;

#else

    template<class I = Iterator, class = enable_if<std::is_default_constructible<I>::value>>
    constexpr n_take_iterator() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    constexpr n_take_iterator(Iterator it, const std::size_t n) : _iterator{ std::move(it) }, _n{ n } {
    }

    LZ_CONSTEXPR_CXX_14 n_take_iterator& operator=(default_sentinel) {
        _n = 0;
        return *this;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
        --_n;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_iterator;
        ++_n;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        _iterator += offset;
        _n -= static_cast<std::size_t>(offset);
    }

    constexpr difference_type difference(const n_take_iterator& b) const {
        return static_cast<difference_type>(b._n) - static_cast<difference_type>(_n);
    }

    constexpr bool eq(const n_take_iterator& b) const {
        return _n == b._n;
    }

    constexpr bool eq(default_sentinel) const {
        return _n == 0;
    }
};

template<class Iterator, class S>
class bounded_take_iterator
    : public iterator<bounded_take_iterator<Iterator, S>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>,
                      iter_cat_t<Iterator>, sentinel_selector<iter_cat_t<Iterator>, bounded_take_iterator<Iterator, S>, S>> {

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterator _iterator;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr bounded_take_iterator()
        requires std::default_initializable<Iterator>
    = default;

#else

    template<class I = Iterator, class = enable_if<std::is_default_constructible<I>::value>>
    constexpr bounded_take_iterator() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    constexpr bounded_take_iterator(Iterator it) : _iterator{ std::move(it) } {
    }

    LZ_CONSTEXPR_CXX_14 bounded_take_iterator& operator=(const S& end) {
        _iterator = end;
        return *this;
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

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        _iterator += offset;
    }

    constexpr difference_type difference(const bounded_take_iterator& b) const {
        return _iterator - b._iterator;
    }

    constexpr bool eq(const bounded_take_iterator& b) const {
        return _iterator == b._iterator;
    }

    constexpr bool eq(const S& sentinel) const {
        return _iterator == sentinel;
    }
};
} // namespace detail
} // namespace lz