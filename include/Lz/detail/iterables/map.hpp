#pragma once

#ifndef LZ_MAP_ITERABLE
#define LZ_MAP_ITERABLE

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/map.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable, class UnaryOp>
class map_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    func_container<UnaryOp> _unary_op;

public:
    using iterator = map_iterator<iter_t<Iterable>, sentinel_t<Iterable>, func_container<UnaryOp>>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_CONCEPTS

    constexpr map_iterable()
        requires std::default_initializable<Iterable> && std::default_initializable<UnaryOp>
    = default;

#else

    template<class I = Iterable,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<UnaryOp>::value>>
    constexpr map_iterable() {
    }

#endif

    template<class I>
    constexpr map_iterable(I&& iterable, UnaryOp unary_op) :
        _iterable{ std::forward<I>(iterable) },
        _unary_op{ std::move(unary_op) } {
    }

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<sized<I>::value, std::size_t> size() const noexcept {
        return static_cast<std::size_t>(lz::size(_iterable));
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr iterator begin() && {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return { detail::begin(std::move(_iterable)), _unary_op };
        }
        else {
            return { detail::begin(std::move(_iterable)), std::move(_unary_op) };
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> begin() && {
        return { detail::begin(std::move(_iterable)), _unary_op };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        return { detail::begin(std::move(_iterable)), std::move(_unary_op) };
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), _unary_op };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() && {
        return { detail::end(std::move(_iterable)), _unary_op };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const& {
        if constexpr (is_bidi_tag<typename iterator::iterator_category>::value) {
            return iterator{ std::end(_iterable), _unary_op };
        }
        else {
            return std::end(_iterable);
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value, iterator> end() const& {
        return { std::end(_iterable), _unary_op };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, sentinel> end() const& {
        return std::end(_iterable);
    }

#endif
};
} // namespace detail
} // namespace lz
#endif