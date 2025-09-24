#pragma once

#ifndef LZ_MAYBE_OWNED_HPP
#define LZ_MAYBE_OWNED_HPP

#include <Lz/detail/procs.hpp>

namespace lz {
namespace detail {
template<class Iterable, bool /* is base of lazy_view && !is c array */>
class maybe_owned_impl;

template<class Iterable>
class maybe_owned_impl<Iterable, false> : public lazy_view {
    Iterable* _iterable_ref_ptr{};

public:
    static constexpr bool holds_reference = true;

    constexpr maybe_owned_impl() noexcept = default;

    template<class I>
    constexpr maybe_owned_impl(I&& iterable) noexcept : _iterable_ref_ptr{ detail::addressof(iterable) } {
        static_assert(std::is_lvalue_reference<I>::value,
                      "Can only bind to lvalues. Check if you are passing a temporary "
                      "object, forgot to add/remove const/volatile qualifiers or forgot to inherit from lz::lazy_view.");
    }

    template<class T, size_t N>
    constexpr maybe_owned_impl(const T (&iterable)[N]) noexcept : _iterable_ref_ptr{ detail::addressof(iterable) } {
    }

    template<class T, size_t N>
    constexpr maybe_owned_impl(T (&iterable)[N]) noexcept : _iterable_ref_ptr{ detail::addressof(iterable) } {
    }

    template<class I>
    constexpr maybe_owned_impl(const maybe_owned_impl<I, false>& other) noexcept : _iterable_ref_ptr{ other._iterable_ref_ptr } {
    }

    template<class I>
    constexpr maybe_owned_impl(maybe_owned_impl<I, false>& other) noexcept : _iterable_ref_ptr{ other._iterable_ref_ptr } {
    }

    template<class I>
    LZ_CONSTEXPR_CXX_14 maybe_owned_impl& operator=(const maybe_owned_impl<I, false>& other) noexcept {
        if (this == &other) {
            return *this;
        }
        _iterable_ref_ptr = other._iterable_ref_ptr;
        return *this;
    }

    template<class I>
    LZ_CONSTEXPR_CXX_14 maybe_owned_impl& operator=(maybe_owned_impl<I, false>& other) noexcept {
        if (this == &other) {
            return *this;
        }
        _iterable_ref_ptr = other._iterable_ref_ptr;
        return *this;
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterable>)
    {
        return static_cast<size_t>(lz::size(*_iterable_ref_ptr));
    }

#else

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if_t<is_sized<I>::value, size_t> size() const {
        return static_cast<size_t>(lz::size(*_iterable_ref_ptr));
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> begin() const {
        if (!_iterable_ref_ptr) {
            return iter_t<Iterable>{};
        }
        return std::begin(*_iterable_ref_ptr);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 sentinel_t<Iterable> end() const {
        if (!_iterable_ref_ptr) {
            return sentinel_t<Iterable>{};
        }
        return std::end(*_iterable_ref_ptr);
    }
};

// Class that contains a c-array or a view to a maybe_owned_impl<Iterable, true>
template<class Iterable>
class maybe_owned_impl<Iterable, true> : public lazy_view {
    using it = typename std::remove_cv<Iterable>::type;
    it _iterable_value{};

    template<class, bool>
    friend class maybe_owned_impl;

public:
    static constexpr bool holds_reference = false;

#ifdef LZ_HAS_CONCEPTS

    constexpr maybe_owned_impl()
        requires(std::default_initializable<it>)
    = default;

#else

    template<class I = it, class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr maybe_owned_impl() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    constexpr maybe_owned_impl(it&& iterable) noexcept(std::is_nothrow_move_constructible<it>::value) :
        _iterable_value{ std::move(iterable) } {
    }

    constexpr maybe_owned_impl(const it& iterable) : _iterable_value{ iterable } {
    }

    template<class I>
    constexpr maybe_owned_impl(maybe_owned_impl<I, true>&& other) noexcept(std::is_nothrow_move_constructible<it>::value) :
        maybe_owned_impl{ std::move(other._iterable_value) } {
    }

    template<class I>
    constexpr maybe_owned_impl(const maybe_owned_impl<I, true>& other) : maybe_owned_impl{ other._iterable_value } {
    }

    template<class I>
    maybe_owned_impl& operator=(const maybe_owned_impl<I, true>& other) {
        if (this == &other) {
            return *this;
        }
        _iterable_value = other._iterable_value;
        return *this;
    }

    template<class I>
    maybe_owned_impl& operator=(maybe_owned_impl<I, true>&& other) noexcept(std::is_nothrow_move_assignable<it>::value) {
        if (this == &other) {
            return *this;
        }
        _iterable_value = std::move(other._iterable_value);
        return *this;
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterable>)
    {
        return static_cast<size_t>(lz::size(_iterable_value));
    }

#else

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if_t<is_sized<I>::value, size_t> size() const {
        return static_cast<size_t>(lz::size(_iterable_value));
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> begin() const {
        return _iterable_value.begin();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 sentinel_t<Iterable> end() const {
        return _iterable_value.end();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> begin() {
        return _iterable_value.begin();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 sentinel_t<Iterable> end() {
        return _iterable_value.end();
    }
};

template<class Iterable>
using maybe_owned = maybe_owned_impl<Iterable, std::is_base_of<lazy_view, typename std::remove_cv<Iterable>::type>::value>;
} // namespace detail

/**
 * @brief Class that contains a reference or a copy of an iterable depending on the type of the iterable. If the iterable is
 * inherited from `lz::lazy_view`, it will store a copy of the iterable. In all other cases, it will store a reference to the
 * iterable. This is done because lazy views are generally cheap to copy, because they contain references to the actual container,
 * rather than the container itself. This class is used in various places in the library to store a reference or a copy of an
 * iterable, depending on the type of the iterable.
 * Example:
 * ```cpp
 * std::vector<int> vec{ 1, 2, 3 };
 * lz::maybe_owned<std::vector<int>> maybe_owned{ vec }; // will store a reference to the vector
 *
 * auto filter = vec | lz::filter([](int i) { return i > 1; });
 * lz::maybe_owned<decltype(filter)> maybe_owned_filter{ filter }; // will store a copy of the filter
 * ```
 */
using detail::maybe_owned;

/**
 * @brief This class can be useful for iterables that are not inherited from `lz::lazy_view`, but are still easy to copy. For
 * instance, you can also use `lz::copied<boost::iterator_range<int*>>` to hold a copy of the underlying range.
 * Internally, this will only copy the pointers to the data, not the data itself.
 *
 * But, for instance, `lz::copied<std::vector<int>>` will hold a copy of the underlying vector. This of course will in
 * turn be slow because it needs to copy all the vector data into the copied iterable.
 * Example:
 * ```cpp
 * std::vector<int> vec{ 1, 2, 3 };
 * lz::copied<std::vector<int>> copied{ vec }; // will store a copy of the vector
 *
 * boost::iterator_range<int*> range{ vec.data(), vec.data() + vec.size() }); // Holds a copy to vector data
 * // lz::filter(range, [](int i) { return i > 1; }); // *CAREFUL* this will hold a reference because it is not a lazy view
 * // Instead, use:
 * lz::copied<boost::iterator_range<int*>> copied{ range }; // will store a copy of the range
 * ```
 */
template<class Iterable>
using copied = detail::maybe_owned_impl<Iterable, true>;

/**
 * @brief This helper function can be useful for iterables that are not inherited from `lz::lazy_view`, but are still easy to
 * copy. For instance, you can use `lz::copied<boost::iterator_range<int*>>` to hold a copy of the underlying range,
 * instead of a reference (default behaviour, as it is not inherited from lazy_view). Internally, this will only copy the pointers
 * to the data, not the data itself.
 *
 * But, for instance, `lz::copied<std::vector<int>>` will also hold a copy of the underlying vector. This of course will
 * in turn be slow because it needs to copy all the vector data into the copied iterable. Example:
 * ```cpp
 * std::vector<int> vec{ 1, 2, 3 };
 * auto copied_vec = lz::as_copied(vec); // will store a copy of the vector, slow
 * auto copied_vec = lz::as_copied(std::move(vec)); // will store a copy of the vector, fast with std::move
 *
 * boost::iterator_range<int*> range{ vec.data(), vec.data() + vec.size() });
 * // lz::filter(range, [](int i) { return i > 1; }); // *CAREFUL* this will hold a reference to `range` because range is *NOT*
 * inherited
 * // from `lazy_view`. Instead, use:
 * auto copied_range = lz::as_copied(range); // will store a copy of the range, cheap
 * ```
 */
template<class Iterable>
copied<detail::remove_ref_t<Iterable>> as_copied(Iterable&& iterable) {
    return copied<detail::remove_ref_t<Iterable>>{ std::forward<Iterable>(iterable) };
}
} // namespace lz

#endif // LZ_MAYBE_OWNED_HPP
