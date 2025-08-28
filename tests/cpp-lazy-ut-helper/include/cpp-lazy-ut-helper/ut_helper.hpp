#pragma once

#ifndef LZ_UT_HELPER_LIB_C_STRING_HPP
#define LZ_UT_HELPER_LIB_C_STRING_HPP

#include <Lz/filter.hpp>
#include <cpp-lazy-ut-helper/pch.hpp>

template<class Iterable>
class bidi_sentinelled : public lz::lazy_view {
    using iterable = lz::filter_iterable<Iterable, std::function<bool(lz::ref_iterable_t<Iterable>)>>;
    iterable _iterable;

public:
    constexpr bidi_sentinelled() = default;

    template<class I>
    explicit bidi_sentinelled(I&& i) :
        // clang-format off
        _iterable{ 
            std::forward<I>(i),
            std::function<bool(lz::ref_iterable_t<Iterable>)>{ [](lz::ref_iterable_t<Iterable>) { 
                return true;
            }}
        } // clang-format on
    {
    }

    lz::iter_t<iterable> begin() const {
        return _iterable.begin();
    }

    lz::default_sentinel_t end() const {
        return lz::default_sentinel;
    }
};

template<class Iterable>
bidi_sentinelled<lz::detail::remove_cvref_t<Iterable>> make_bidi_sentinelled(Iterable&& iterable) {
    return bidi_sentinelled<lz::detail::remove_cvref_t<Iterable>>(std::forward<Iterable>(iterable));
}

template<class Iterable>
class ra_assign_op_tester : public lz::lazy_view {
    static_assert(!std::is_same<lz::iter_t<Iterable>, lz::sentinel_t<Iterable>>::value, "");
    lz::maybe_owned<Iterable> _iterable{};

public:
    constexpr ra_assign_op_tester() = default;

    template<class I>
    explicit ra_assign_op_tester(I&& iterable) : _iterable{ std::forward<I>(iterable) } {
    }

    lz::iter_t<Iterable> begin() const {
        return _iterable.begin();
    }

    lz::iter_t<Iterable> end() const {
        auto end = _iterable.begin();
        end = _iterable.end();
        return end;
    }

    size_t size() const {
        return lz::size(_iterable);
    }
};

template<class Iterable>
ra_assign_op_tester<lz::detail::remove_cvref_t<Iterable>> make_ra_assign_op_tester(Iterable&& iterable) {
    return ra_assign_op_tester<lz::detail::remove_cvref_t<Iterable>>(std::forward<Iterable>(iterable));
}

#endif // LZ_UT_HELPER_LIB_C_STRING_HPP
