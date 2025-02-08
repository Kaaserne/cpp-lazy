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
    using iterable = remove_reference_t<Iterable>;
    using pointer = conditional<std::is_const<iterable>::value, const iterable*, iterable*>;

    pointer _iterable_ref_ptr{};

public:
    constexpr ref_or_view_helper() = default;

    template<class I>
    LZ_CONSTEXPR_CXX_17 ref_or_view_helper(I&& iterable) noexcept : _iterable_ref_ptr{ std::addressof(iterable) } {
    }

    template<class I>
    LZ_CONSTEXPR_CXX_17 ref_or_view_helper(ref_or_view_helper<I, false>&& other) = delete; // cannot get address of rvalue

    template<class I>
    LZ_CONSTEXPR_CXX_17 ref_or_view_helper(const ref_or_view_helper<I, false>& other) noexcept :
        ref_or_view_helper{ *other._iterable_ref_ptr } {
    }

    template<class I>
    LZ_CONSTEXPR_CXX_17 ref_or_view_helper(ref_or_view_helper<I, false>& other) noexcept :
        ref_or_view_helper{ *other._iterable_ref_ptr } {
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

// Class that contains a c-array or a view to a ref_or_view<Iterable, true>
template<class Iterable>
class ref_or_view_helper<Iterable, true> : public lazy_view {
    Iterable _iterable_value;

public:
    constexpr ref_or_view_helper() = default;

    template<class I>
    constexpr ref_or_view_helper(I&& iterable) : _iterable_value{ std::forward<I>(iterable) } {
    }

    template<class I>
    LZ_CONSTEXPR_CXX_17 ref_or_view_helper(ref_or_view_helper<I, true>&& other) :
        ref_or_view_helper{ std::move(other._iterable_value) } {
    }

    template<class I>
    LZ_CONSTEXPR_CXX_17 ref_or_view_helper(const ref_or_view_helper<I, true>& other) :
        ref_or_view_helper{ other._iterable_value } {
    }

    template<class I>
    LZ_CONSTEXPR_CXX_17 ref_or_view_helper(ref_or_view_helper<I, true>& other) : ref_or_view_helper{ other._iterable_value } {
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
using is_c_array = std::is_array<remove_reference_t<Iterable>>;

template<class Iterable>
using ref_or_view =
    ref_or_view_helper<Iterable, std::is_base_of<lazy_view, remove_cvref<Iterable>>::value || is_c_array<Iterable>::value>;
} // namespace detail
} // namespace lz

#endif // LZ_REF_OR_VIEW_HPP
