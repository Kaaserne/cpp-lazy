#pragma once

#ifndef LZ_GROUP_BY_ITERABLE_HPP
#define LZ_GROUP_BY_ITERABLE_HPP

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/group_by.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {

template<class Iterable, class BinaryPredicate>
class group_by_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    func_container<BinaryPredicate> _binary_predicate;

public:
    using iterator = group_by_iterator<iter_t<Iterable>, sentinel_t<Iterable>, func_container<BinaryPredicate>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    template<class I>
    LZ_CONSTEXPR_CXX_14 group_by_iterable(I&& iterable, BinaryPredicate binary_predicate) :
        _iterable{ std::forward<I>(iterable) },
        _binary_predicate{ std::move(binary_predicate) } {
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr iterator begin() const& {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable), _binary_predicate };
        }
        else {
            return { std::begin(_iterable), std::end(_iterable), _binary_predicate };
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::begin(_iterable), std::end(_iterable), _binary_predicate };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _binary_predicate };
    }

#endif

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        return { detail::begin(std::move(_iterable)), detail::end(std::move(_iterable)), std::move(_binary_predicate) };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return iterator{ std::end(_iterable), std::begin(_iterable), std::end(_iterable), _binary_predicate };
        }
        else {
            return default_sentinel{};
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const {
        return { std::end(_iterable), std::begin(_iterable), std::end(_iterable), _binary_predicate };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, default_sentinel> end() const {
        return {};
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_GROUP_BY_ITERABLE_HPP