#pragma once

#ifndef LZ_ZIP_ITERATOR_HPP
#define LZ_ZIP_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <Lz/iterator_base.hpp>
#include <algorithm>

namespace lz {
namespace detail {

template<class IterTuple, class SMaybeHomo>
class zip_iterator : public iterator<zip_iterator<IterTuple, SMaybeHomo>, iter_tuple_ref_type_t<IterTuple>,
                                     fake_ptr_proxy<iter_tuple_ref_type_t<IterTuple>>, iter_tuple_diff_type_t<IterTuple>,
                                     iter_tuple_iter_cat_t<IterTuple>, SMaybeHomo> {

public:
    using iterator_category = iter_tuple_iter_cat_t<IterTuple>;
    using value_type = iter_tuple_value_type_t<IterTuple>;
    using difference_type = iter_tuple_diff_type_t<IterTuple>;
    using reference = iter_tuple_ref_type_t<IterTuple>;
    using pointer = fake_ptr_proxy<reference>;

private:
    using is = make_index_sequence<tuple_size<IterTuple>::value>;
    IterTuple _iterators;

    template<size_t... I>
    constexpr reference dereference(index_sequence<I...>) const {
        using std::get;
        return reference{ *get<I>(_iterators)... };
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence<I...>) {
        using std::get;
#ifdef LZ_HAS_CXX_17
        (++get<I>(_iterators), ...);
#else
        decompose(++get<I>(_iterators)...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void decrement(index_sequence<I...>) {
        using std::get;
#ifdef LZ_HAS_CXX_17
        (--get<I>(_iterators), ...);
#else
        decompose(--get<I>(_iterators)...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset, index_sequence<I...>) {
        using std::get;
#ifdef LZ_HAS_CXX_17
        ((get<I>(_iterators) += offset), ...);
#else
        decompose(get<I>(_iterators) += offset...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 difference_type minus(const zip_iterator& other, index_sequence<I...>) const {
        using std::get;
        const auto max = std::max({ (get<I>(_iterators) - get<I>(other._iterators))... });
        if (max > 0) {
            return max;
        }
        return std::min({ (get<I>(_iterators) - get<I>(other._iterators))... });
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 difference_type minus(const SMaybeHomo& other, index_sequence<I...>) const {
        using std::get;
        return std::max({ (get<I>(_iterators) - get<I>(other))... });
    }

    template<class EndIter, size_t... I>
    LZ_CONSTEXPR_CXX_14 bool eq(const EndIter& other, index_sequence<I...>) const {
        using std::get;
#ifdef LZ_HAS_CXX_17
        return ((get<I>(_iterators) == get<I>(other)) || ...);
#else
        return std::max({ (get<I>(_iterators) == get<I>(other))... });
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void assign_sentinels(const SMaybeHomo& other, const index_sequence<I...>&) {
        using std::get;
#ifdef LZ_HAS_CXX_17
        ((get<I>(_iterators) = get<I>(other)), ...);
#else
        decompose(get<I>(_iterators) = get<I>(other)...);
#endif
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr zip_iterator()
        requires(std::default_initializable<IterTuple>)
    = default;

#else

    template<class I = IterTuple, class = enable_if<std::is_default_constructible<I>::value>>
    constexpr zip_iterator() noexcept(std::is_nothrow_default_constructible<I>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14 zip_iterator(IterTuple iterators) : _iterators{ std::move(iterators) } {
        static_assert(tuple_size<IterTuple>::value > 1, "Cannot concat one/zero iterables");
    }

    LZ_CONSTEXPR_CXX_14 zip_iterator& operator=(const SMaybeHomo& end) {
        assign_sentinels(end, is{});
        return *this;
    }

    constexpr reference dereference() const {
        return dereference(is{});
    }

    constexpr pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        increment(is{});
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        decrement(is{});
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        plus_is(offset, is{});
    }

    LZ_CONSTEXPR_CXX_20 difference_type difference(const zip_iterator& other) const {
        return minus(other, is{});
    }

    LZ_CONSTEXPR_CXX_20 difference_type difference(const SMaybeHomo& other) const {
        return minus(other, is{});
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const zip_iterator& other) const {
        return eq(other._iterators, is{});
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const SMaybeHomo& other) const {
        return eq(other, is{});
    }
};
} // namespace detail
} // namespace lz

#endif
