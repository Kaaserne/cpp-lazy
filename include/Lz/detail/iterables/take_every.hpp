#pragma once

#ifndef LZ_TAKE_EVERY_ITERABLE_HPP
#define LZ_TAKE_EVERY_ITERABLE_HPP

#include <Lz/detail/iterators/take_every.hpp>
#include <Lz/detail/maybe_owned.hpp>

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
    using iterator = take_every_iterator<maybe_owned<Iterable>>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    maybe_owned<Iterable> _iterable;
    std::size_t _offset{};
    std::size_t _start{};

    using diff = typename iterator::difference_type;

    static constexpr bool return_sentinel =
        !is_bidi_tag<typename iterator::iterator_category>::value || is_sentinel<iter_t<Iterable>, sent>::value;

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr take_every_iterable()
        requires(std::default_initializable<maybe_owned<Iterable>>)
    = default;

#else

    template<class I = decltype(_iterable), class = enable_if<std::is_default_constructible<I>::value>>
    constexpr take_every_iterable() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    template<class I>
    LZ_CONSTEXPR_CXX_14 take_every_iterable(I&& iterable, const std::size_t offset, const std::size_t start) :
        _iterable{ std::forward<I>(iterable) },
        _offset{ offset },
        _start{ start } {
        LZ_ASSERT(_offset != 0, "Cannot increment by 0");
    }

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr std::size_t size() const
        requires(sized<Iterable>)
    {
        const auto cur_size = static_cast<std::size_t>(lz::size(_iterable));
        if (static_cast<diff>(cur_size) - static_cast<diff>(_start) <= 0) {
            return 0;
        }

        const auto actual_size = std::max(diff{ 1 }, static_cast<diff>(cur_size - _start));
        const auto rem = actual_size % static_cast<diff>(_offset);
        const auto quot = actual_size / static_cast<diff>(_offset);

        return static_cast<std::size_t>(quot) + static_cast<std::size_t>(rem == 0 ? 0 : 1);
    }

#else

    template<class I = Iterable>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_sized<I>::value, std::size_t> size() const {
        const auto cur_size = static_cast<std::size_t>(lz::size(_iterable));
        if (static_cast<diff>(cur_size) - static_cast<diff>(_start) <= 0) {
            return 0;
        }

        const auto actual_size = std::max(diff{ 1 }, static_cast<diff>(cur_size - _start));
        const auto rem = actual_size % static_cast<diff>(_offset);
        const auto quot = actual_size / static_cast<diff>(_offset);

        return static_cast<std::size_t>(quot) + static_cast<std::size_t>(rem == 0 ? 0 : 1);
    }

#endif

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr iterator begin() const& {
        auto start_pos = next_fast_safe(_iterable, static_cast<typename iterator::difference_type>(_start));
        if constexpr (is_ra_tag_v<typename iterator::iterator_category>) {
            return { _iterable, start_pos, _offset };
        }
        else if constexpr (is_bidi_tag_v<typename iterator::iterator_category>) {
            return { start_pos, _iterable.end(), _offset, _start };
        }
        else {
            return { start_pos, _iterable.end(), _offset };
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() const& {
        auto start_pos = next_fast_safe(_iterable, static_cast<typename iterator::difference_type>(_start));
        return { start_pos, _iterable.end(), _offset };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<std::is_same<I, std::bidirectional_iterator_tag>::value, iterator> begin() const& {
        auto start_pos = next_fast_safe(_iterable, static_cast<typename iterator::difference_type>(_start));
        return { start_pos, _iterable.end(), _offset, _start };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra_tag<I>::value, iterator> begin() const& {
        // random access iterator can go both ways O(1), no need to use next_fast_safe
        auto start_pos = next_fast_safe(_iterable, static_cast<typename iterator::difference_type>(_start));
        return { _iterable, start_pos, _offset };
    }

#endif

#ifdef LZ_HAS_CONCEPTS

    [[nodiscard]] constexpr iterator begin() &&
        requires(!is_bidi_tag<typename iterator::iterator_category>::value)
    {
        // forward iterator can only go forward, so next_fast_safe is not needed
        auto end = _iterable.end();
        auto start_pos = next_fast_safe(std::move(_iterable), static_cast<typename iterator::difference_type>(_start));
        return { start_pos, end, _offset };
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<!is_bidi_tag<I>::value, iterator> begin() && {
        // forward iterator can only go forward, so next_fast_safe is not needed
        auto end = _iterable.end();
        auto start_pos = next_fast_safe(std::move(_iterable), static_cast<typename iterator::difference_type>(_start));
        return { start_pos, end, _offset };
    }

#endif

#ifdef LZ_HAS_CXX_17

    [[nodiscard]] constexpr auto end() const {
        if constexpr (return_sentinel) {
            return lz::default_sentinel;
        }
        else if constexpr (!is_ra_tag_v<typename iterator::iterator_category>) {
            return iterator{ _iterable.end(), _iterable.end(), _offset, lz::eager_size(_iterable) - _start };
        }
        else {
            return iterator{ _iterable, _iterable.end(), _offset };
        }
    }

#else

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<is_ra_tag<I>::value && !is_sentinel<iter, sent>::value, iterator> end() const {
        return { _iterable, _iterable.end(), _offset };
    }

    template<class I = typename iterator::iterator_category>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14
        // std::is_same is not the same as is_bidi_tag, explicitly check for bidirectional iterator tag
        enable_if<std::is_same<I, std::bidirectional_iterator_tag>::value && !is_sentinel<iter, sent>::value, iterator>
        end() const {
        return { _iterable.end(), _iterable.end(), _offset, lz::eager_size(_iterable) - _start };
    }

    template<bool R = return_sentinel> // checks for not bidirectional or if is sentinel
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 enable_if<R, default_sentinel_t> end() const noexcept {
        return {};
    }

#endif
};

} // namespace detail
} // namespace lz

#endif
