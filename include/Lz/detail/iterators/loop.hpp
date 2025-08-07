#pragma once

#ifndef LZ_LOOP_ITERATOR_HPP
#define LZ_LOOP_ITERATOR_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>

namespace lz {
namespace detail {
template<class Iterable, bool /* is inf */>
class loop_iterator;

template<class Iterable>
class loop_iterator<Iterable, false>
    : public iterator<loop_iterator<Iterable, false>, ref_t<iter_t<Iterable>>, fake_ptr_proxy<ref_t<iter_t<Iterable>>>,
                      diff_type<iter_t<Iterable>>, iter_cat_t<iter_t<Iterable>>, default_sentinel_t> {
    using it = iter_t<Iterable>;
    using traits = std::iterator_traits<it>;

    it _iterator;
    Iterable _iterable;
    std::size_t _rotations_left{};

public:
    using reference = typename traits::reference;
    using value_type = typename traits::value_type;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr loop_iterator()
        requires std::default_initializable<it> && std::default_initializable<Iterable>
    = default;

#else

    template<class I = it,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value>>
    constexpr loop_iterator() noexcept(std::is_nothrow_default_constructible<it>::value &&
                                       std::is_nothrow_default_constructible<Iterable>::value) {
    }

#endif

    template<class I>
    constexpr loop_iterator(I&& iterable, it iter, std::size_t amount) :
        _iterator{ std::move(iter) },
        _iterable{ std::forward<I>(iterable) },
        _rotations_left{ amount } {
    }

    LZ_CONSTEXPR_CXX_14 loop_iterator& operator=(default_sentinel_t) {
        _rotations_left = 0;
        // We don't set _iterator to end here otherwise eq will return true
        return *this;
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

        if (_iterator == std::end(_iterable)) {
            --_rotations_left;
            _iterator = std::begin(_iterable);
        }
        if (static_cast<difference_type>(_rotations_left) == -1) {
            _iterator = std::end(_iterable);
            _rotations_left = 0;
        }
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        --_iterator;
        if (_iterator == std::begin(_iterable)) {
            _iterator = std::end(_iterable);
            ++_rotations_left;
        }
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        const auto iter_length = std::end(_iterable) - std::begin(_iterable);
        const auto remainder = offset % iter_length;
        _iterator += offset % iter_length;
        _rotations_left -= static_cast<std::size_t>(offset / iter_length);

        LZ_ASSERT_ADDABLE(static_cast<difference_type>(_rotations_left) >= -1);

        if (_iterator == std::begin(_iterable) && static_cast<difference_type>(_rotations_left) == -1) {
            // We are exactly at end (rotations left is unsigned)
            _iterator = std::end(_iterable);
            _rotations_left = 0;
        }
        else if (_iterator == std::end(_iterable) && offset < 0 && remainder == 0) {
            _iterator = std::begin(_iterable);
            --_rotations_left;
        }
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const loop_iterator& other) const {
        LZ_ASSERT_COMPTABLE(std::begin(_iterable) == std::begin(other._iterable) &&
                            std::end(_iterable) == std::end(other._iterable));
        const auto rotations_left_diff =
            static_cast<difference_type>(other._rotations_left) - static_cast<difference_type>(_rotations_left);
        return (_iterator - other._iterator) + rotations_left_diff * (std::end(_iterable) - std::begin(_iterable));
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(default_sentinel_t) const {
        const auto rotations_left_diff = static_cast<difference_type>(_rotations_left);
        return -((std::end(_iterable) - _iterator) + rotations_left_diff * (std::end(_iterable) - std::begin(_iterable)));
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const loop_iterator& other) const {
        LZ_ASSERT_COMPTABLE(std::begin(_iterable) == std::begin(other._iterable) &&
                            std::end(_iterable) == std::end(other._iterable));
        return _rotations_left == other._rotations_left && _iterator == other._iterator;
    }

    constexpr bool eq(default_sentinel_t) const {
        return _rotations_left == 0 && _iterator == std::end(_iterable);
    }
};

template<class Iterable>
class loop_iterator<Iterable, true>
    : public iterator<loop_iterator<Iterable, true>, ref_t<iter_t<Iterable>>, fake_ptr_proxy<ref_t<iter_t<Iterable>>>,
                      diff_type<iter_t<Iterable>>, iter_cat_t<iter_t<Iterable>>, default_sentinel_t> {

    using it = iter_t<Iterable>;
    using traits = std::iterator_traits<it>;

    it _iterator;
    Iterable _iterable;

public:
    using reference = typename traits::reference;
    using value_type = typename traits::value_type;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;
    using iterator_category = typename traits::iterator_category;

#ifdef LZ_HAS_CONCEPTS

    constexpr loop_iterator()
        requires std::default_initializable<Iterable> && std::default_initializable<it>
    = default;

#else

    template<class I = it,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<Iterable>::value>>
    constexpr loop_iterator() noexcept(std::is_nothrow_default_constructible<it>::value &&
                                       std::is_nothrow_default_constructible<Iterable>::value) {
    }

#endif

    template<class I>
    constexpr loop_iterator(I&& iterable, it iter) : _iterator{ std::move(iter) }, _iterable{ std::forward<I>(iterable) } {
    }

    LZ_CONSTEXPR_CXX_14 loop_iterator& operator=(default_sentinel_t) noexcept {
        return *this;
    }

    constexpr reference dereference() const {
        return *_iterator;
    }

    constexpr pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        ++_iterator;
        if (_iterator == std::end(_iterable)) {
            _iterator = std::begin(_iterable);
        }
    }

    constexpr bool eq(const loop_iterator&) const {
        return *this == lz::default_sentinel;
    }

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4717) // Recursive type
#endif

    constexpr bool eq(default_sentinel_t) const {
        return std::begin(_iterable) == std::end(_iterable);
    }

#ifdef _MSC_VER
#pragma warning(pop)
#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_LOOP_ITERATOR_HPP