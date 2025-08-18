#pragma once

#ifndef LZ_ZIP_LONGEST_ITERATOR_HPP
#define LZ_ZIP_LONGEST_ITERATOR_HPP

#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <Lz/optional.hpp>

namespace lz {
namespace detail {

template<class>
struct optional_iter_tuple_value_type;

template<class... Iterators>
struct optional_iter_tuple_value_type<std::tuple<Iterators...>> {
    using type = std::tuple<optional<val_t<Iterators>>...>;
};

template<class Iterator, size_t N>
struct optional_iter_tuple_value_type<homogeneous_array<Iterator, N>> {
    using optional_value_type = optional<val_t<Iterator>>;
    using type = decltype(tuple_of<optional_value_type>(make_index_sequence<N>()));
};

template<class IterTuple>
using optional_value_type_iter_tuple_t = typename optional_iter_tuple_value_type<IterTuple>::type;

template<bool>
struct reference_or_value_type;

template<>
struct reference_or_value_type<true /* is lvalue reference */> {
    template<class Ref, class>
    using type = std::reference_wrapper<remove_ref<Ref>>;
};

template<>
struct reference_or_value_type<false /* is lvalue reference */> {
    template<class, class Val>
    using type = Val;
};

template<class Ref, class Val>
using reference_or_value_type_t = typename reference_or_value_type<std::is_lvalue_reference<Ref>::value>::template type<Ref, Val>;

template<class>
struct optional_iter_tuple_ref_type;

template<class... Iterators>
struct optional_iter_tuple_ref_type<std::tuple<Iterators...>> {
    using type = std::tuple<optional<reference_or_value_type_t<ref_t<Iterators>, val_t<Iterators>>>...>;
};

template<class Iterator, size_t N>
struct optional_iter_tuple_ref_type<homogeneous_array<Iterator, N>> {
    using optional_ref_or_value_type = optional<reference_or_value_type_t<ref_t<Iterator>, val_t<Iterator>>>;
    using type = decltype(tuple_of<optional_ref_or_value_type>(make_index_sequence<N>()));
};

template<class IterTuple>
using optional_iter_tuple_ref_type_t = typename optional_iter_tuple_ref_type<IterTuple>::type;

template<bool, class, class>
class zip_longest_iterator;

using std::get;

template<class IterMaybeHomo, class SMaybeHomo>
class zip_longest_iterator<false /* bidi */, IterMaybeHomo, SMaybeHomo>
    : public iterator<zip_longest_iterator<false, IterMaybeHomo, SMaybeHomo>, optional_iter_tuple_ref_type_t<IterMaybeHomo>,
                      fake_ptr_proxy<optional_iter_tuple_ref_type_t<IterMaybeHomo>>, iter_tuple_diff_type_t<IterMaybeHomo>,
                      iter_tuple_iter_cat_t<IterMaybeHomo>, default_sentinel_t> {
public:
    using iterator_category = iter_tuple_iter_cat_t<IterMaybeHomo>;
    using value_type = optional_value_type_iter_tuple_t<IterMaybeHomo>;
    using difference_type = iter_tuple_diff_type_t<IterMaybeHomo>;
    using reference = optional_iter_tuple_ref_type_t<IterMaybeHomo>;
    using pointer = fake_ptr_proxy<value_type>;

private:
    using is = make_index_sequence<tuple_size<IterMaybeHomo>::value>;

    IterMaybeHomo _iterators;
    SMaybeHomo _end;

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 auto
    deref_one() const -> optional<reference_or_value_type_t<ref_t<remove_cvref<decltype(get<I>(_iterators))>>,
                                                            val_t<remove_cvref<decltype(get<I>(_iterators))>>>> {

        using iter_type = remove_cvref<decltype(get<I>(_iterators))>;
        using ref_or_value_type = optional<reference_or_value_type_t<ref_t<iter_type>, val_t<iter_type>>>;

        if (get<I>(_iterators) == get<I>(_end)) {
            return lz::nullopt;
        }

        return ref_or_value_type{ *get<I>(_iterators) };
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 reference dereference(index_sequence<I...>) const {
        return reference{ deref_one<I>()... };
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence<I...>) {
#ifdef LZ_HAS_CXX_17
        ((get<I>(_iterators) != get<I>(_end) ? static_cast<void>(++get<I>(_iterators)) : (void)(0)), ...);
#else
        decompose((get<I>(_iterators) != get<I>(_end) ? static_cast<void>(++get<I>(_iterators)) : (void)(0), 0)...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_17 bool eq(const zip_longest_iterator& other, index_sequence<I...>) const {
#ifdef LZ_HAS_CXX_17
        return ((get<I>(_iterators) == get<I>(other._iterators)) && ...);
#else
        return std::min({ get<I>(_iterators) == get<I>(other._iterators)... });
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_17 bool eq(index_sequence<I...>) const {
#ifdef LZ_HAS_CXX_17
        return ((get<I>(_iterators) == get<I>(_end)) && ...);
#else
        return std::min({ get<I>(_iterators) == get<I>(_end)... });
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void assign_sentinels(index_sequence<I...>) {
#ifdef LZ_HAS_CXX_17
        ((get<I>(_iterators) = get<I>(_end)), ...);
#else
        decompose(get<I>(_iterators) = get<I>(_end)...);
#endif
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr zip_longest_iterator()
        requires(std::default_initializable<IterMaybeHomo> && std::default_initializable<SMaybeHomo>)
    = default;

#else

    template<class I = IterMaybeHomo,
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<SMaybeHomo>::value>>
    constexpr zip_longest_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                              std::is_nothrow_default_constructible<SMaybeHomo>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14 zip_longest_iterator(IterMaybeHomo iterators, SMaybeHomo end) :
        _iterators{ std::move(iterators) },
        _end{ std::move(end) } {
        static_assert(tuple_size<IterMaybeHomo>::value > 1, "Cannot concat one/zero iterables");
    }

    LZ_CONSTEXPR_CXX_14 zip_longest_iterator& operator=(default_sentinel_t) {
        assign_sentinels(is{});
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        return dereference(is{});
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        increment(is{});
    }

    LZ_CONSTEXPR_CXX_17 bool eq(const zip_longest_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_end == other._end);
        return eq(other, is{});
    }

    LZ_CONSTEXPR_CXX_17 bool eq(default_sentinel_t) const {
        return eq(is{});
    }
};

template<class IterMaybeHomo, class SMaybeHomo>
class zip_longest_iterator<true /* bidi */, IterMaybeHomo, SMaybeHomo>
    : public iterator<zip_longest_iterator<true, IterMaybeHomo, SMaybeHomo>, optional_iter_tuple_ref_type_t<IterMaybeHomo>,
                      fake_ptr_proxy<optional_iter_tuple_ref_type_t<IterMaybeHomo>>, iter_tuple_diff_type_t<IterMaybeHomo>,
                      iter_tuple_iter_cat_t<IterMaybeHomo>, default_sentinel_t> {
public:
    using iterator_category = iter_tuple_iter_cat_t<IterMaybeHomo>;
    using value_type = optional_value_type_iter_tuple_t<IterMaybeHomo>;
    using difference_type = iter_tuple_diff_type_t<IterMaybeHomo>;
    using reference = optional_iter_tuple_ref_type_t<IterMaybeHomo>;
    using pointer = fake_ptr_proxy<value_type>;

    static constexpr size_t tup_size = tuple_size<IterMaybeHomo>::value;

private:
    using is = make_index_sequence<tup_size>;
    using difference_tuple = decltype(make_homogeneous_of<difference_type>(is{}));

    IterMaybeHomo _iterators;
    SMaybeHomo _end;
    difference_tuple _distances;

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 auto
    deref_one() const -> optional<reference_or_value_type_t<ref_t<remove_cvref<decltype(get<I>(_iterators))>>,
                                                            val_t<remove_cvref<decltype(get<I>(_iterators))>>>> {

        using iter_type = remove_cvref<decltype(get<I>(_iterators))>;
        using ref_or_value_type = optional<reference_or_value_type_t<ref_t<iter_type>, val_t<iter_type>>>;

        if (get<I>(_iterators) == get<I>(_end)) {
            return lz::nullopt;
        }

        return ref_or_value_type{ *get<I>(_iterators) };
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 reference dereference(index_sequence<I...>) const {
        return reference{ deref_one<I>()... };
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence<I...>) {
#ifdef LZ_HAS_CXX_17
        ((get<I>(_iterators) != get<I>(_end) ? static_cast<void>(++get<I>(_iterators), ++get<I>(_distances)) : (void)(0)), ...);
#else
        decompose((get<I>(_iterators) != get<I>(_end) ? static_cast<void>(++get<I>(_iterators), ++get<I>(_distances)) : (void)(0),
                   0)...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void decrement(index_sequence<I...>) {
        const auto longest = std::max({ get<I>(_distances)... });
        LZ_ASSERT_DECREMENTABLE(longest > 0);
#ifdef LZ_HAS_CXX_17
        ((get<I>(_distances) == longest ? static_cast<void>(--get<I>(_iterators), --get<I>(_distances)) : (void)(0)), ...);
#else
        decompose(
            (get<I>(_distances) == longest ? static_cast<void>(--get<I>(_iterators), --get<I>(_distances)) : (void)(0), 0)...);
#endif
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 void plus_is_one(const difference_type offset) {
        auto& it = get<I>(_iterators);
        const auto& end = get<I>(_end);
        auto& distance = get<I>(_distances);

        const auto difference = end - it;
        if (offset > difference) {
            it = end;
            distance = difference;
        }
        else {
            it += offset;
            distance += offset;
        }
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset, index_sequence<I...>) {
#ifdef LZ_HAS_CXX_17
        (plus_is_one<I>(offset), ...);
#else
        decompose((plus_is_one<I>(offset), 0)...);
#endif
    }

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 void min_is_one(const difference_type longest, const difference_type offset) {
        auto& this_iter_length = get<I>(_distances);
        auto& it = get<I>(_iterators);

        if (this_iter_length == longest) {
            it -= offset;
            this_iter_length -= offset;
            return;
        }
        const auto surplus = this_iter_length + offset;
        if (surplus > longest) {
            const auto surplus_longest_diff = surplus - longest;
            it -= surplus_longest_diff;
            this_iter_length -= surplus_longest_diff;
        }
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void min_is(const difference_type offset, index_sequence<I...>) {
        const auto longest = std::max({ get<I>(_distances)... });
        LZ_ASSERT_SUBTRACTABLE(offset <= longest);
#ifdef LZ_HAS_CXX_17
        (min_is_one<I>(longest, offset), ...);
#else
        decompose((min_is_one<I>(longest, offset), 0)...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 difference_type minus(const zip_longest_iterator& other, index_sequence<I...>) const {
        const auto max = std::max({ static_cast<difference_type>(get<I>(_iterators) - get<I>(other._iterators))... });
        if (max > 0) {
            return max;
        }
        return std::min({ (get<I>(_iterators) - get<I>(other._iterators))... });
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 difference_type minus(index_sequence<I...>) const {
        return std::min({ static_cast<difference_type>(get<I>(_iterators) - get<I>(_end))... });
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 bool eq(index_sequence<I...>) const {
#ifdef LZ_HAS_CXX_17
        return ((get<I>(_iterators) == get<I>(_end)) && ...);
#else
        return std::min({ get<I>(_iterators) == get<I>(_end)... });
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 bool eq(const zip_longest_iterator& other, index_sequence<I...>) const {
        return _iterators == other._iterators;
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void assign_sentinels(index_sequence<I...>) {
#ifdef LZ_HAS_CXX_17
        ((get<I>(_iterators) = get<I>(_end)), ...);
#else
        decompose(get<I>(_iterators) = get<I>(_end)...);
#endif
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr zip_longest_iterator()
        requires(std::default_initializable<IterMaybeHomo> && std::default_initializable<SMaybeHomo>)
    = default;

#else

    template<class I = IterMaybeHomo,
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<SMaybeHomo>::value>>
    constexpr zip_longest_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                              std::is_nothrow_default_constructible<SMaybeHomo>::value) {
    }

#endif

    constexpr zip_longest_iterator(IterMaybeHomo it, SMaybeHomo end, difference_tuple distances) :
        _iterators{ std::move(it) },
        _end{ std::move(end) },
        _distances{ distances } {
        static_assert(tup_size > 1, "Cannot zip one/zero iterables");
    }

    LZ_CONSTEXPR_CXX_14 zip_longest_iterator& operator=(default_sentinel_t) {
        assign_sentinels(is{});
        return *this;
    }

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        LZ_ASSERT_DEREFERENCABLE(!eq(lz::default_sentinel));
        return dereference(is{});
    }

    LZ_CONSTEXPR_CXX_14 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        LZ_ASSERT_INCREMENTABLE(!eq(lz::default_sentinel));
        increment(is{});
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        decrement(is{});
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        if (offset < 0) {
            min_is(-offset, is{});
        }
        else {
            LZ_ASSERT_INCREMENTABLE(difference(lz::default_sentinel) <= offset);
            plus_is(offset, is{});
        }
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(const zip_longest_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_end == other._end);
        return minus(other, is{});
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(default_sentinel_t) const {
        return minus(is{});
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const zip_longest_iterator& other) const {
        LZ_ASSERT_COMPATIBLE(_end == other._end);
        return eq(other, is{});
    }

    LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel_t) const {
        return eq(is{});
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_ZIP_LONGEST_ITERATOR_HPP
