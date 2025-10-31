#pragma once

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/procs/assert.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/util/default_sentinel.hpp>

namespace lz {
namespace detail {
template<class Iterator>
class n_take_iterator : public iterator<n_take_iterator<Iterator>, ref_t<Iterator>, fake_ptr_proxy<ref_t<Iterator>>,
                                        diff_type<Iterator>, iter_cat_t<Iterator>, default_sentinel_t> {

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = typename traits::value_type;
    using difference_type = typename traits::difference_type;
    using reference = typename traits::reference;
    using pointer = fake_ptr_proxy<reference>;

private:
    Iterator _iterator{};
    difference_type _n{};

public:
    constexpr n_take_iterator(const n_take_iterator&) = default;
    LZ_CONSTEXPR_CXX_14 n_take_iterator& operator=(const n_take_iterator&) = default;

#ifdef LZ_HAS_CONCEPTS

    constexpr n_take_iterator()
        requires(std::default_initializable<Iterator>)
    = default;

#else

    template<class I = Iterator, class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr n_take_iterator() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    constexpr n_take_iterator(Iterator it, const difference_type n) : _iterator{ std::move(it) }, _n{ n } {
    }

#ifdef LZ_HAS_CXX_17

    constexpr n_take_iterator& operator=(default_sentinel_t) {
        if constexpr (is_bidi_v<Iterator>) {
            _iterator = std::next(_iterator, _n);
        }
        _n = 0;
        return *this;
    }

#else

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_14 enable_if_t<is_bidi<I>::value, n_take_iterator&> operator=(default_sentinel_t) {
        _iterator = std::next(_iterator, _n);
        _n = 0;
        return *this;
    }

    template<class I = Iterator>
    LZ_CONSTEXPR_CXX_14 enable_if_t<!is_bidi<I>::value, n_take_iterator&> operator=(default_sentinel_t) {
        _n = 0;
        return *this;
    }

#endif

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return *_iterator;
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        ++_iterator;
        --_n;
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_iterator;
        ++_n;
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        _iterator += offset;
        _n -= offset;
    }

    constexpr difference_type difference(const n_take_iterator& other) const {
        return other._n - _n;
    }

    constexpr difference_type difference(default_sentinel_t) const {
        return -_n;
    }

    constexpr bool eq(const n_take_iterator& other) const {
        return _n == other._n;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _n == 0;
    }
};
} // namespace detail
} // namespace lz
