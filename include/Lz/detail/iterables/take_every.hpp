#pragma once

#ifndef LZ_TAKE_EVERY_ITERABLE_HPP
#define LZ_TAKE_EVERY_ITERABLE_HPP

#include <Lz/detail/iterators/take_every.hpp>
#include <Lz/detail/ref_or_view.hpp>

namespace lz {
namespace detail {

// next_fast_safe: check whether it's faster to go forward or backward when incrementing n steps
// Check whether n is within bounds. Only relevant for sized (requesting size is then O(1)).
// Only bidirectional iterators supported currently

#ifdef LZ_HAS_CXX_17

#else

#endif

template<class Iterable>
class take_every_iterable : public lazy_view {
    using iter = iter_t<Iterable>;
    using sent = sentinel_t<Iterable>;

public:
    using iterator = take_every_iterator<ref_or_view<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    ref_or_view<Iterable> _iterable;
    std::size_t _offset{};
    std::size_t _start{};

    template<class I = iter>
    LZ_CONSTEXPR_CXX_14 enable_if<is_ra<I>::value, iter> get_begin() const {
        if (_start >= static_cast<std::size_t>(std::end(_iterable) - std::begin(_iterable))) {
            return std::end(_iterable);
        }
        return std::begin(_iterable) + static_cast<typename iterator::difference_type>(_start);
    }

    using diff_type = typename iterator::difference_type;

    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || is_sentinel<iter_t<Iterable>, sent>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr take_every_iterable()
        requires std::default_initializable<ref_or_view<Iterable>>
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr take_every_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    constexpr take_every_iterable(I&& iterable, const std::size_t offset, const std::size_t start) :
        _iterable{ std::forward<I>(iterable) },
        _offset{ offset },
        _start{ start } {
    }

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<sized<I>::value, std::size_t> size() const {
        const auto cur_size = static_cast<std::size_t>(lz::size(_iterable));
        if (cur_size == 0) {
            return 0;
        }

        const auto actual_size = std::max(diff_type{ 1 }, static_cast<diff_type>(cur_size - _start));
        const auto rem = actual_size % static_cast<diff_type>(_offset);
        const auto quot = actual_size / static_cast<diff_type>(_offset);

        return static_cast<std::size_t>(quot) + static_cast<std::size_t>(rem == 0 ? 0 : 1);
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr iterator begin() const& {
        if constexpr (!is_bidi_tag<typename iterator::iterator_category>::value) {
            // forward iterators can only go forward, so next_fast_safe is not needed
            auto begin = std::begin(_iterable);
            for (std::size_t i = 0; i < _start && begin != std::end(_iterable); ++i, ++begin) {
            }
            return { begin, std::end(_iterable), _offset };
        }
        else if constexpr (std::is_same_v<typename iterator::iterator_category, std::bidirectional_iterator_tag>) {
            // bidirectional iterator can go both ways, so we use next_fast_safe
            auto begin = next_fast_safe(_iterable, static_cast<typename iterator::difference_type>(_start));
            return { begin, std::end(_iterable), _offset, _start };
        }
        else {
            // random access iterator can go both ways O(1), no need to use next_fast_safe
            auto start_pos = get_begin();
            return { start_pos, start_pos, std::end(_iterable), _offset };
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() const& {
        // forward iterator can only go forward, so next_fast_safe is not needed
        auto begin = std::begin(_iterable);
        for (std::size_t i = 0; i < _start && begin != std::end(_iterable); ++i, ++begin) {
        }
        return { begin, std::end(_iterable), _offset };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<std::is_same<I, std::bidirectional_iterator_tag>::value, iterator> begin() const& {
        auto begin = next_fast_safe(_iterable, static_cast<typename iterator::difference_type>(_start));
        return { begin, std::end(_iterable), _offset, _start };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra_tag<I>::value, iterator> begin() const& {
        // random access iterator can go both ways O(1), no need to use next_fast_safe
        auto start_pos = get_begin();
        return { start_pos, start_pos, std::end(_iterable), _offset };
    }

#endif

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        // forward iterator can only go forward, so next_fast_safe is not needed
        auto begin = detail::begin(std::move(_iterable));
        auto end = detail::end(std::move(_iterable));
        for (std::size_t i = 0; i < _start && begin != end; ++i, ++begin) {
        }
        return { begin, end, _offset };
    }

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (return_sentinel) {
            return default_sentinel{};
        }
        else if constexpr (std::is_same_v<typename iterator::iterator_category, std::bidirectional_iterator_tag>) {
            return iterator{ std::end(_iterable), std::end(_iterable), _offset, lz::eager_size(_iterable) - _start };
        }
        else {
            // random access iterator can go both ways O(1), no need to use next_fast_safe
            auto start_pos = get_begin();
            return iterator{ std::end(_iterable), start_pos, std::end(_iterable), _offset };
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra_tag<I>::value && !is_sentinel<iter, sent>::value, iterator> end() const {
        // random access iterator can go both ways O(1), no need to use next_fast_safe
        auto start_pos = get_begin();
        return { std::end(_iterable), start_pos, std::end(_iterable), _offset };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
        // std::is_same is not the same as is_bidi_tag, explicitly check for bidirectional iterator tag
        enable_if<std::is_same<I, std::bidirectional_iterator_tag>::value && !is_sentinel<iter, sent>::value, iterator>
        end() const {
        return { std::end(_iterable), std::end(_iterable), _offset, lz::eager_size(_iterable) - _start };
    }

    template<bool R = return_sentinel> // checks for not bidirectional or if is sentinel
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, default_sentinel> end() const noexcept {
        return {};
    }

#endif
};

} // namespace detail
} // namespace lz

#endif