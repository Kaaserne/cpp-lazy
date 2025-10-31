#pragma once

#ifndef LZ_ROTATE_ITERABLE_HPP
#define LZ_ROTATE_ITERABLE_HPP

#include <Lz/detail/procs/next_fast.hpp>
#include <Lz/detail/iterators/rotate.hpp>
#include <Lz/detail/maybe_owned.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

namespace lz {
namespace detail {

template<class Iterable>
class rotate_iterable : public lazy_view {
    using inner_iter = iter_t<Iterable>;
    using diff_t = diff_type<inner_iter>;

    maybe_owned<Iterable> _iterable{};
    inner_iter _start_iter{};
    diff_t _start_index{};

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
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<inner_iter>::value>>
    constexpr rotate_iterable() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                         std::is_nothrow_default_constructible<inner_iter>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 rotate_iterable(I&& iterable, const diff_t start) :
        _iterable{ std::forward<I>(iterable) },
        _start_iter{ next_fast_safe(_iterable, start) },
        _start_index{ start } {
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr size_t size() const
        requires(sized<Iterable>)
    {
        const auto s = lz::size(_iterable);
        LZ_ASSERT(_start_index <= s, "start index is larger than size of iterable");
        return _start_index == s ? 0 : s;
    }

#else

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<is_sized<I>::value, size_t> size() const {
        const auto s = lz::size(_iterable);
        if (s == 0) {
            return 0;
        }
        return static_cast<size_t>(_start_index) == s ? 0 : s;
    }

#endif

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iterator begin() const {
        return { _iterable, _start_iter, _start_iter == _iterable.end() ? _start_index : 0 };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (!return_sentinel) {
            return iterator{ _iterable, _start_iter, _start_iter == _iterable.end() ? _start_index : lz::eager_ssize(_iterable) };
        }
        else {
            return _start_iter;
        }
    }

#else

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<!R, iterator> end() const {
        return { _iterable, _start_iter, _start_iter == _iterable.end() ? _start_index : lz::eager_ssize(_iterable) };
    }

    template<bool R = return_sentinel>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if_t<R, inner_iter> end() const {
        return _start_iter; // TODO make this a struct type that is private only
    }

#endif
};
} // namespace detail
} // namespace lz

#endif // LZ_ROTATE_ITERABLE_HPP
