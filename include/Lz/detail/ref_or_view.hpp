#pragma once

#ifndef LZ_REF_OR_VIEW_HPP
#define LZ_REF_OR_VIEW_HPP

#include <Lz/detail/procs.hpp>

namespace lz {
namespace detail {
template<class Iterable, bool /* is base of lazy_view && !is c array */>
struct ref_or_view_helper;

template<class Iterable>
class ref_or_view_helper<Iterable, false> : public lazy_view {
    using pointer = conditional<std::is_array<Iterable>::value, Iterable*, decltype(std::addressof(std::declval<Iterable&>()))>;

    pointer _iterable_ref_ptr{};

public:
    constexpr ref_or_view_helper() = default;

    template<class I>
    LZ_CONSTEXPR_CXX_17 ref_or_view_helper(I&& iterable) noexcept : _iterable_ref_ptr{ std::addressof(iterable) } {
    }

    template<class T, std::size_t N>
    constexpr ref_or_view_helper(const T (&iterable)[N]) noexcept : _iterable_ref_ptr{ &iterable } {
    }

    template<class T, std::size_t N>
    constexpr ref_or_view_helper(T (&iterable)[N]) noexcept : _iterable_ref_ptr{ &iterable } {
    }

    template<class I>
    LZ_CONSTEXPR_CXX_17 ref_or_view_helper(const ref_or_view_helper<I, false>& other) noexcept :
        _iterable_ref_ptr{ other._iterable_ref_ptr } {
    }

    template<class I>
    ref_or_view_helper& operator=(const ref_or_view_helper<I, false>& other) noexcept {
        if (this == &other) {
            return *this;
        }
        _iterable_ref_ptr = other._iterable_ref_ptr;
        return *this;
    }

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(lz::size(*_iterable_ref_ptr));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> begin() const& {
        return std::begin(*_iterable_ref_ptr);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> begin() && {
        return detail::begin(std::move(*_iterable_ref_ptr));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 sentinel_t<Iterable> end() const& {
        return std::end(*_iterable_ref_ptr);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 sentinel_t<Iterable> end() && {
        return detail::end(std::move(*_iterable_ref_ptr));
    }
};

// TODO add test with const array
// Class that contains a c-array or a view to a ref_or_view<Iterable, true>
template<class Iterable>
class ref_or_view_helper<Iterable, true> : public lazy_view {
    using it = typename std::remove_cv<Iterable>::type;
    it _iterable_value;

    template<class, bool>
    friend class ref_or_view_helper;

public:
    constexpr ref_or_view_helper() = default;

    constexpr ref_or_view_helper(it&& iterable) : _iterable_value{ std::move(iterable) } {
    }

    constexpr ref_or_view_helper(const it& iterable) : _iterable_value{ iterable } {
    }

    template<class I>
    constexpr ref_or_view_helper(ref_or_view_helper<I, true>&& other) : ref_or_view_helper{ std::move(other._iterable_value) } {
    }

    template<class I>
    constexpr ref_or_view_helper(const ref_or_view_helper<I, true>& other) : ref_or_view_helper{ other._iterable_value } {
    }

    template<class I>
    ref_or_view_helper& operator=(const ref_or_view_helper<I, true>& other) {
        if (this == &other) {
            return *this;
        }
        _iterable_value = other._iterable_value;
        return *this;
    }

    template<class I>
    ref_or_view_helper& operator=(ref_or_view_helper<I, true>&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        _iterable_value = std::move(other._iterable_value);
        return *this;
    }

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(lz::size(_iterable_value));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> begin() const& {
        return std::begin(_iterable_value);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> begin() && {
        return detail::begin(std::move(_iterable_value));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 sentinel_t<Iterable> end() const& {
        return std::end(_iterable_value);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 sentinel_t<Iterable> end() && {
        return detail::end(std::move(_iterable_value));
    }
};

template<class Iterable>
using ref_or_view = ref_or_view_helper<Iterable, std::is_base_of<lazy_view, typename std::remove_cv<Iterable>::type>::value &&
                                                     !std::is_array<Iterable>::value>;
} // namespace detail

/**
 * @brief Class that contains a reference or a copy of an iterable depending on the type of the iterable. If the iterable is
 * inherited from `lz::lazy_view`, it will store a copy of the iterable. In all other cases, it will store a reference to the
 * iterable. This is done because lazy views are generally cheap to copy, because they contain references to the actual container,
 * rather than the container itself. This class is used in various places in the library to store a reference or a copy of an
 * iterable, depending on the type of the iterable.
 * @tparam Iterable The type of the iterable to store a reference or copy of.
 */
using detail::ref_or_view;

} // namespace lz

#endif // LZ_REF_OR_VIEW_HPP
