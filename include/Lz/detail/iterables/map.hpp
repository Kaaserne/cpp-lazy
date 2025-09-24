#pragma once

#ifndef LZ_MAP_ITERABLE
#define LZ_MAP_ITERABLE

#include <Lz/detail/func_container.hpp>
#include <Lz/detail/iterators/map.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {
template<class Iterable, class UnaryOp>
class map_iterable : public lazy_view { // TODO initialize all members in default ctor -Weffc++
    maybe_owned<Iterable> _iterable{};
    func_container<UnaryOp> _unary_op{};

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
        requires(std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<UnaryOp>)
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<UnaryOp>::value>>
    constexpr map_iterable() noexcept(std::is_nothrow_default_constructible<maybe_owned<Iterable>>::value &&
                                      std::is_nothrow_default_constructible<UnaryOp>::value) {
    }

#endif

    template<class I>
    constexpr map_iterable(I&& iterable, UnaryOp unary_op) :
        _iterable{ std::forward<I>(iterable) },
        _unary_op{ std::move(unary_op) } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterable>)
    {
        return static_cast<size_t>(lz::size(_iterable));
    }

#else

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if_t<is_sized<I>::value, size_t> size() const noexcept {
        return static_cast<size_t>(lz::size(_iterable));
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return { _iterable.begin(), _unary_op };
    }


#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable.end(), _unary_op };
        }
        else {
            return _iterable.end();
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!R, iterator> end() const {
        return { _iterable.end(), _unary_op };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, sentinel> end() const {
        return _iterable.end();
    }

#endif
};
} // namespace detail
} // namespace lz
#endif
