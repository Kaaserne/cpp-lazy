#pragma once

#ifndef LZ_MAP_ITERABLE
#define LZ_MAP_ITERABLE

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/map.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {
template<class Iterable, class UnaryOp>
class map_iterable : public lazy_view {
    maybe_owned<Iterable> _iterable;
    func_container<UnaryOp> _unary_op;

    using iter = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;

public:
    using iterator = map_iterator<iter, sent, func_container<UnaryOp>>;
    using sentinel = sent;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || is_sentinel<iter, sent>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr map_iterable()
        requires std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<UnaryOp>
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<UnaryOp>::value>>
    constexpr map_iterable() noexcept(std::is_nothrow_default_constructible<maybe_owned<Iterable>>::value &&
                                      std::is_nothrow_default_constructible<UnaryOp>::value) {
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
        if constexpr (!return_sentinel) {
            return { detail::begin(std::move(_iterable)), _unary_op };
        }
        else {
            return { detail::begin(std::move(_iterable)), std::move(_unary_op) };
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!R, iterator> begin() && {
        return { detail::begin(std::move(_iterable)), _unary_op };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, iterator> begin() && {
        return { detail::begin(std::move(_iterable)), std::move(_unary_op) };
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), _unary_op };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!R, iterator> end() && {
        return { detail::end(std::move(_iterable)), _unary_op };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const& {
        if constexpr (!return_sentinel) {
            return iterator{ std::end(_iterable), _unary_op };
        }
        else {
            return std::end(_iterable);
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!R, iterator> end() const& {
        return { std::end(_iterable), _unary_op };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, sentinel> end() const& {
        return std::end(_iterable);
    }

#endif
};
} // namespace detail
} // namespace lz
#endif