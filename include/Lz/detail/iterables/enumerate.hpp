#pragma once

#ifndef LZ_ENUMERATE_ITERABLE_HPP
#define LZ_ENUMERATE_ITERABLE_HPP

#include <Lz/detail/iterators/enumerate.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {
template<class Iterable, class IntType>
class enumerate_iterable : public lazy_view {
    using iter = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;

    maybe_owned<Iterable> _iterable;
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
    static constexpr bool return_sentinel = !is_bidi_tag<iter_cat_t<iterator>>::value || has_sentinel<Iterable>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr enumerate_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr enumerate_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    constexpr enumerate_iterable(I&& iterable, const IntType start = 0) : _iterable{ iterable }, _start{ start } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterable>)
    {
        return static_cast<size_t>(lz::eager_size(_iterable));
    }

#else

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_sized<I>::value, size_t> size() const {
        return static_cast<size_t>(lz::size(_iterable));
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { _iterable.begin(), _start };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { detail::begin(std::move(_iterable)), _start };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const& {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable.end(), get_last_index() };
        }
        else {
            return _iterable.end();
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!R, iterator> end() const& {
        return { _iterable.end(), get_last_index() };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, sentinel> end() const& {
        return _iterable.end();
    }

#endif

#ifdef LZ_HAS_CONCEPTS
    // clang-format off
    [[nodiscard]] constexpr iterator end() &&
        requires(is_bidi_tag_v<typename iterator::iterator_category> && !is_sentinel_v<iter, sent>)
    {
        return { detail::end(std::move(_iterable)), get_last_index() };
    }
    // clang-format on
#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_bidi_tag<I>::value && !is_sentinel<iter, sent>::value, iterator> end() && {
        return { detail::end(std::move(_iterable)), get_last_index() };
    }

#endif
};
} // namespace detail
} // namespace lz
#endif
