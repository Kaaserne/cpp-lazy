#pragma once

#ifndef LZ_EXCLUDE_ITERABLE_HPP
#define LZ_EXCLUDE_ITERABLE_HPP

#include <Lz/detail/iterators/exclude.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class exclude_iterable : public lazy_view {
    ref_or_view<Iterable> _iterable;
    diff_iterable_t<Iterable> _from{};
    diff_iterable_t<Iterable> _to{};

    using it = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;

public:
    using iterator = exclude_iterator<it, sent>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;
    using sentinel = typename iterator::sentinel;

private:
    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || is_sentinel<it, sent>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr exclude_iterable()
        requires std::default_initializable<ref_or_view<Iterable>>
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr exclude_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    constexpr exclude_iterable(I&& iterable, const diff_iterable_t<Iterable> from, const diff_iterable_t<Iterable> to) :
        _iterable{ std::forward<I>(iterable) },
        _from{ from },
        _to{ to } {
    }

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<sized<I>::value, std::size_t> size() const {
        return static_cast<std::size_t>(lz::size(_iterable) - static_cast<std::size_t>(_to - _from));
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { std::begin(_iterable), std::end(_iterable), _from, _to, 0 };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() && {
        return { detail::begin(std::move(_iterable)), std::end(_iterable), _from, _to, 0 };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ std::end(_iterable), std::end(_iterable), _from, _to,
                             static_cast<typename iterator::difference_type>(lz::eager_size(_iterable)) };
        }
        else {
            return std::end(_iterable);
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!R, iterator> end() const {
        return { std::end(_iterable), std::end(_iterable), _from, _to,
                 static_cast<typename iterator::difference_type>(lz::eager_size(_iterable)) };
    }
    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, sentinel> end() const {
        return std::end(_iterable);
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_EXCLUDE_ITERABLE_HPP
