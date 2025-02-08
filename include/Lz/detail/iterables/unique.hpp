#pragma once

#ifndef LZ_UNIQUE_ITERABLE_HPP
#define LZ_UNIQUE_ITERABLE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/unique.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable, class BinaryPredicate>
class unique_iterable : public lazy_view {
    using inner_iter = iter_t<Iterable>;
    ref_or_view<Iterable> _iterable;
    func_container<BinaryPredicate> _predicate;

public:
    using iterator = unique_iterator<iter_t<Iterable>, sentinel_t<Iterable>, func_container<BinaryPredicate>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr unique_iterable() = default;

    template<class I>
    constexpr unique_iterable(I&& iterable, BinaryPredicate compare) :
        _iterable{ std::forward<I>(iterable) },
        _predicate{ std::move(compare) } {
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable), _predicate };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _predicate };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi<I>::value, iterator> begin() && {
        return { detail::begin(std::move(_iterable)), detail::end(std::move(_iterable)), std::move(_predicate) };
    }

    template<class I = inner_iter>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi<I>::value, iterator> end() const {
        return { std::end(_iterable), std::begin(_iterable), std::end(_iterable), _predicate };
    }

    template<class I = inner_iter>
    LZ_NODISCARD constexpr enable_if<!is_bidi<I>::value, default_sentinel> end() const noexcept {
        return {};
    }
};
} // namespace detail
} // namespace lz

#endif // LZ_UNIQUE_ITERABLE_HPP
