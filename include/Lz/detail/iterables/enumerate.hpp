#pragma once

#ifndef LZ_ENUMERATE_ITERABLE_HPP
#define LZ_ENUMERATE_ITERABLE_HPP

#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterators/enumerate.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable, class IntType>
class enumerate_iterable : public lazy_view {
    using iter = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;

    ref_or_view<Iterable> _iterable;
    IntType _start{};

    IntType get_last_index() const {
        return _start + static_cast<IntType>(lz::eager_size(_iterable));
    }

public:
    using iterator = enumerate_iterator<iter, sent, IntType>;
    using const_iterator = iterator;
    using sentinel = sent;
    using value_type = typename iterator::value_type;

private:
    static constexpr bool return_sentinel = !is_bidi_tag<iter_cat_t<iterator>>::value || is_sentinel<iter, sent>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr enumerate_iterable()
        requires std::default_initializable<Iterable>
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr enumerate_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    constexpr enumerate_iterable(I&& iterable, const IntType start = 0) : _iterable{ iterable }, _start{ start } {
    }

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(lz::size(_iterable));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), _start };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { detail::begin(std::move(_iterable)), _start };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const& {
        if constexpr (!return_sentinel) {
            return iterator{ std::end(_iterable), get_last_index() };
        }
        else {
            return std::end(_iterable);
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!R, iterator> end() const& {
        return { std::end(_iterable), get_last_index() };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, sentinel> end() const& {
        return std::end(_iterable);
    }

#endif

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value && !is_sentinel<iter, sent>::value, iterator> end() && {
        return { detail::end(std::move(_iterable)), get_last_index() };
    }
};
} // namespace detail
} // namespace lz
#endif