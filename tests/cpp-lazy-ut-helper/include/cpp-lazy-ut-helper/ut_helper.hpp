#pragma once

#ifndef LZ_UT_HELPER_LIB_C_STRING_HPP
#define LZ_UT_HELPER_LIB_C_STRING_HPP

#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <Lz/filter.hpp>
#include <Lz/procs/eager_size.hpp>
#include <Lz/traits/is_sized.hpp>
#include <Lz/traits/lazy_view.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>

template<class Iterable, bool EnableSize = true>
class bidi_sentinelled : public lz::lazy_view {
    using iterable = lz::filter_iterable<Iterable, std::function<bool(lz::detail::ref_iterable_t<Iterable>)>>;
    iterable _iterable{};

public:
    template<class I = Iterable, class = lz::detail::enable_if_t<std::is_default_constructible<I>::value>>
    constexpr bidi_sentinelled() {
    }

    template<class I>
    explicit bidi_sentinelled(I&& i) :
        // clang-format off
        _iterable{ 
            std::forward<I>(i),
            std::function<bool(lz::detail::ref_iterable_t<Iterable>)>{
            [](lz::detail::ref_iterable_t<Iterable>) { 
                return true;
            }}
        } // clang-format on
    {
    }

    template<bool E = EnableSize>
    lz::detail::enable_if_t<E, size_t> size() const {
        return lz::eager_size(_iterable);
    }

    lz::iter_t<iterable> begin() const {
        return _iterable.begin();
    }

    lz::default_sentinel_t end() const {
        return lz::default_sentinel;
    }
};

template<class Iterable>
bidi_sentinelled<lz::detail::remove_cvref_t<Iterable>, true> make_sized_bidi_sentinelled(Iterable&& iterable) {
    return bidi_sentinelled<lz::detail::remove_cvref_t<Iterable>, true>(std::forward<Iterable>(iterable));
}

template<class Iterable>
bidi_sentinelled<lz::detail::remove_cvref_t<Iterable>, false> make_non_sized_bidi_sentinelled(Iterable&& iterable) {
    return bidi_sentinelled<lz::detail::remove_cvref_t<Iterable>, false>(std::forward<Iterable>(iterable));
}

template<class Iterable>
class sentinel_assign_op_tester : public lz::lazy_view {
    lz::maybe_owned<Iterable> _iterable{};

public:
    template<class I = Iterable, class = lz::detail::enable_if_t<std::is_default_constructible<I>::value>>
    constexpr sentinel_assign_op_tester() {
    }

    template<class I>
    explicit sentinel_assign_op_tester(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

    lz::iter_t<Iterable> begin() const {
        return _iterable.begin();
    }

    lz::iter_t<Iterable> end() const {
        auto end = _iterable.begin();
        end = _iterable.end();
        return end;
    }

#ifdef LZ_HAS_CONCEPTS

    size_t size() const
        requires(sized<Iterable>)
    {
        return lz::size(_iterable);
    }

#else

    template<class T = Iterable>
    lz::detail::enable_if_t<lz::is_sized<T>::value, size_t> size() const {
        return lz::size(_iterable);
    }

#endif
};

template<class Iterable>
sentinel_assign_op_tester<lz::detail::remove_cvref_t<Iterable>> make_sentinel_assign_op_tester(Iterable&& iterable) {
    static_assert(!std::is_same<lz::iter_t<Iterable>, lz::sentinel_t<Iterable>>::value, "");

    return sentinel_assign_op_tester<lz::detail::remove_cvref_t<Iterable>>(std::forward<Iterable>(iterable));
}

#endif // LZ_UT_HELPER_LIB_C_STRING_HPP
