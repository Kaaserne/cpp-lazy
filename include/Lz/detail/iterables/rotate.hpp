#pragma once

#ifndef LZ_ROTATE_ITERABLE_HPP
#define LZ_ROTATE_ITERABLE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/iterators/rotate.hpp>
#include <Lz/detail/maybe_owned.hpp>

namespace lz {
namespace detail {

template<class Iterable>
class rotate_iterable : public lazy_view {
    using inner_iter = iter_t<Iterable>;
    using diff_t = diff_type<inner_iter>;

    maybe_owned<Iterable> _iterable;
    inner_iter _start_iter;
    size_t _start_index{};

public:
    using iterator = rotate_iterator<maybe_owned<Iterable>>;
    using sentinel = typename iterator::sentinel;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || is_sentinel<inner_iter, sentinel_t<Iterable>>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr rotate_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>> && std::default_initializable<inner_iter>)
    = default;

#else

    template<class I = decltype(_iterable),
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<inner_iter>::value>>
    constexpr rotate_iterable() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<inner_iter>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 rotate_iterable(I&& iterable, const diff_t start) :
        _iterable{ std::forward<I>(iterable) },
        _start_iter{ next_fast_safe(_iterable, start) },
        _start_index{ static_cast<size_t>(start) } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterable>)
    {
        return static_cast<size_t>(lz::size(_iterable));
    }

#else

    template<class I = Iterable>
    LZ_NODISCARD constexpr enable_if<is_sized<I>::value, size_t> size() const {
        return static_cast<size_t>(lz::size(_iterable));
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const& {
        return { _iterable, _start_iter, _start_iter == _iterable.end() ? _start_index : 0 };
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr iterator begin() const
        requires(return_sentinel)
    {
        return { std::move(_iterable), std::move(_start_iter), _start_iter == _iterable.end() ? _start_index : 0 };
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, iterator> begin() && {
        return { std::move(_iterable), std::move(_start_iter), _start_iter == _iterable.end() ? _start_index : 0 };
    }

#endif

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable, _start_iter, lz::eager_size(_iterable) };
        }
        else {
            return _start_iter;
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!R, iterator> end() const {
        return { _iterable, _start_iter, lz::eager_size(_iterable) };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, inner_iter> end() const {
        return _start_iter;
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_ROTATE_ITERABLE_HPP
