#pragma once

#ifndef LZ_EXCLUDE_ITERABLE_HPP
#define LZ_EXCLUDE_ITERABLE_HPP

#include <Lz/detail/iterators/exclude.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {
template<class Iterable>
class exclude_iterable : public lazy_view {
    maybe_owned<Iterable> _iterable{};
    diff_iterable_t<Iterable> _from{};
    diff_iterable_t<Iterable> _to{};

    using it = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;

public:
    using iterator = exclude_iterator<maybe_owned<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;
    using sentinel = typename iterator::sentinel;

private:
    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || is_sentinel<it, sent>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr exclude_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if_t<std::is_default_constructible<I>::value>>
    constexpr exclude_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    constexpr exclude_iterable(I&& iterable, const diff_iterable_t<Iterable> from, const diff_iterable_t<Iterable> to) :
        _iterable{ std::forward<I>(iterable) },
        _from{ from },
        _to{ to } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterable>)
    {
        return static_cast<size_t>(lz::size(_iterable) - static_cast<size_t>(_to - _from));
    }

#else

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<is_sized<I>::value, size_t> size() const {
        return static_cast<size_t>(lz::size(_iterable) - static_cast<size_t>(_to - _from));
    }
#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return { _iterable, _iterable.begin(), _from, _to, 0 };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable, _iterable.end(), _from, _to,
                             static_cast<typename iterator::difference_type>(lz::eager_size(_iterable)) };
        }
        else {
            return lz::default_sentinel;
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!R, iterator> end() const {
        return { _iterable, _iterable.end(), _from, _to,
                 static_cast<typename iterator::difference_type>(lz::eager_size(_iterable)) };
    }
    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, sentinel> end() const {
        return lz::default_sentinel;
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_EXCLUDE_ITERABLE_HPP
