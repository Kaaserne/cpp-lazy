#pragma once

#ifndef LZ_ZIP_LONGEST_ITERATOR_HPP
#define LZ_ZIP_LONGEST_ITERATOR_HPP

#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <Lz/iterator_base.hpp>
#include <Lz/optional.hpp>

namespace lz {
namespace detail {

template<class>
struct optional_iter_tuple_value_type_helper;

template<class... Iterators>
struct optional_iter_tuple_value_type_helper<std::tuple<Iterators...>> {
    using type = std::tuple<optional<val_t<Iterators>>...>;
};

template<class Iterator, std::size_t N>
struct optional_iter_tuple_value_type_helper<std::array<Iterator, N>> {
    using optional_value_type = optional<val_t<Iterator>>;
    using type = decltype(tuple_of<optional_value_type>(make_index_sequence<N>()));
};

template<class IterTuple>
using optional_value_type_iter_tuple_t = typename optional_iter_tuple_value_type_helper<IterTuple>::type;

template<bool>
struct reference_or_value_type_helper;

template<>
struct reference_or_value_type_helper<true /* is lvalue reference */> {
    template<class Ref, class>
    using type = std::reference_wrapper<remove_ref<Ref>>;
};

template<>
struct reference_or_value_type_helper<false /* is lvalue reference */> {
    template<class, class Val>
    using type = Val;
};

template<class Ref, class Val>
using reference_or_value_type =
    typename reference_or_value_type_helper<std::is_lvalue_reference<Ref>::value>::template type<Ref, Val>;

template<class>
struct optional_iter_tuple_ref_type_helper;

template<class... Iterators>
struct optional_iter_tuple_ref_type_helper<std::tuple<Iterators...>> {
    using type = std::tuple<optional<reference_or_value_type<ref_t<Iterators>, val_t<Iterators>>>...>;
};

template<class Iterator, std::size_t N>
struct optional_iter_tuple_ref_type_helper<std::array<Iterator, N>> {
    using optional_ref_or_value_type = optional<reference_or_value_type<ref_t<Iterator>, val_t<Iterator>>>;
    using type = decltype(tuple_of<optional_ref_or_value_type>(make_index_sequence<N>()));
};

template<class IterTuple>
using optional_iter_tuple_ref_type = typename optional_iter_tuple_ref_type_helper<IterTuple>::type;

template<bool, class, class>
class zip_longest_iterator;

template<class IterMaybeHomo, class SMaybeHomo>
class zip_longest_iterator<false /* bidi */, IterMaybeHomo, SMaybeHomo>
    : public iterator<zip_longest_iterator<false, IterMaybeHomo, SMaybeHomo>, optional_iter_tuple_ref_type<IterMaybeHomo>,
                      fake_ptr_proxy<optional_iter_tuple_ref_type<IterMaybeHomo>>, iter_tuple_diff_type_t<IterMaybeHomo>,
                      iter_tuple_iter_cat_t<IterMaybeHomo>, default_sentinel_t> {
public:
    using iterator_category = iter_tuple_iter_cat_t<IterMaybeHomo>;
    using value_type = optional_value_type_iter_tuple_t<IterMaybeHomo>;
    using difference_type = iter_tuple_diff_type_t<IterMaybeHomo>;
    using reference = optional_iter_tuple_ref_type<IterMaybeHomo>;
    using pointer = fake_ptr_proxy<value_type>;

private:
    using is = make_index_sequence<std::tuple_size<IterMaybeHomo>::value>;

    IterMaybeHomo _iterators;
    SMaybeHomo _end;

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 auto
    deref_one() const -> optional<reference_or_value_type<ref_t<remove_cvref<decltype(std::get<I>(_iterators))>>,
                                                          val_t<remove_cvref<decltype(std::get<I>(_iterators))>>>> {

        using iter_type = remove_cvref<decltype(std::get<I>(_iterators))>;
        using ref_or_value_type = optional<reference_or_value_type<ref_t<iter_type>, val_t<iter_type>>>;

        if (std::get<I>(_iterators) == std::get<I>(_end)) {
            return lz::nullopt;
        }

        return ref_or_value_type{ *std::get<I>(_iterators) };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 reference dereference(index_sequence<I...>) const {
        return reference{ deref_one<I>()... };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence<I...>) {
#ifdef LZ_HAS_CXX_17
        ((std::get<I>(_iterators) != std::get<I>(_end) ? static_cast<void>(++std::get<I>(_iterators)) : (void)(0)), ...);
#else
        decompose(
            (std::get<I>(_iterators) != std::get<I>(_end) ? static_cast<void>(++std::get<I>(_iterators)) : (void)(0), 0)...);
#endif
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_17 bool eq(const zip_longest_iterator& other, index_sequence<I...>) const {
#ifdef LZ_HAS_CXX_17
        return ((std::get<I>(_iterators) == std::get<I>(other._iterators)) && ...);
#else
        return std::min({ std::get<I>(_iterators) == std::get<I>(other._iterators)... });
#endif
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_17 bool eq(index_sequence<I...>) const {
#ifdef LZ_HAS_CXX_17
        return ((std::get<I>(_iterators) == std::get<I>(_end)) && ...);
#else
        return std::min({ std::get<I>(_iterators) == std::get<I>(_end)... });
#endif
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void assign_sentinels(index_sequence<I...>) {
#ifdef LZ_HAS_CXX_17
        ((std::get<I>(_iterators) = std::get<I>(_end)), ...);
#else
        decompose(std::get<I>(_iterators) = std::get<I>(_end)...);
#endif
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr zip_longest_iterator()
        requires std::default_initializable<IterMaybeHomo> && std::default_initializable<SMaybeHomo>
    = default;

#else

    template<class I = IterMaybeHomo,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<SMaybeHomo>::value>>
    constexpr zip_longest_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                              std::is_nothrow_default_constructible<SMaybeHomo>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14 zip_longest_iterator(IterMaybeHomo iterators, SMaybeHomo end) :
        _iterators{ std::move(iterators) },
        _end{ std::move(end) } {
        static_assert(std::tuple_size<IterMaybeHomo>::value > 1, "Cannot concat one/zero iterables");
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

    LZ_CONSTEXPR_CXX_17 bool eq(const zip_longest_iterator& b) const {
        LZ_ASSERT(_end == b._end, "Incompatible iterators");
        return eq(b, is{});
    }

    LZ_CONSTEXPR_CXX_17 bool eq(default_sentinel_t) const {
        return eq(is{});
    }
};

template<class IterMaybeHomo, class SMaybeHomo>
class zip_longest_iterator<true /* bidi */, IterMaybeHomo, SMaybeHomo>
    : public iterator<zip_longest_iterator<true, IterMaybeHomo, IterMaybeHomo>, optional_iter_tuple_ref_type<IterMaybeHomo>,
                      fake_ptr_proxy<optional_iter_tuple_ref_type<IterMaybeHomo>>, iter_tuple_diff_type_t<IterMaybeHomo>,
                      iter_tuple_iter_cat_t<IterMaybeHomo>, default_sentinel_t> {
public:
    using iterator_category = iter_tuple_iter_cat_t<IterMaybeHomo>;
    using value_type = optional_value_type_iter_tuple_t<IterMaybeHomo>;
    using difference_type = iter_tuple_diff_type_t<IterMaybeHomo>;
    using reference = optional_iter_tuple_ref_type<IterMaybeHomo>;
    using pointer = fake_ptr_proxy<value_type>;

    static constexpr std::size_t tuple_size = std::tuple_size<IterMaybeHomo>::value;

private:
    using is = make_index_sequence<std::tuple_size<IterMaybeHomo>::value>;
    using difference_tuple = decltype(make_homogeneous_of<difference_type>(is{}));

    IterMaybeHomo _iterators;
    SMaybeHomo _end;
    difference_tuple _distances;

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 auto
    deref_one() const -> optional<reference_or_value_type<ref_t<remove_cvref<decltype(std::get<I>(_iterators))>>,
                                                          val_t<remove_cvref<decltype(std::get<I>(_iterators))>>>> {

        using iter_type = remove_cvref<decltype(std::get<I>(_iterators))>;
        using ref_or_value_type = optional<reference_or_value_type<ref_t<iter_type>, val_t<iter_type>>>;

        if (std::get<I>(_iterators) == std::get<I>(_end)) {
            return lz::nullopt;
        }

        return ref_or_value_type{ *std::get<I>(_iterators) };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 reference dereference(index_sequence<I...>) const {
        return reference{ deref_one<I>()... };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence<I...>) {
#ifdef LZ_HAS_CXX_17
        ((std::get<I>(_iterators) != std::get<I>(_end) ? static_cast<void>(++std::get<I>(_iterators), ++std::get<I>(_distances))
                                                       : (void)(0)),
         ...);
#else
        decompose((std::get<I>(_iterators) != std::get<I>(_end)
                       ? static_cast<void>(++std::get<I>(_iterators), ++std::get<I>(_distances))
                       : (void)(0),
                   0)...);
#endif
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void decrement(index_sequence<I...>) {
        const auto longest = std::max({ std::get<I>(_distances)... });
        LZ_ASSERT(longest > 0, "Cannot decrement before begin");
#ifdef LZ_HAS_CXX_17
        ((std::get<I>(_distances) == longest ? static_cast<void>(--std::get<I>(_iterators), --std::get<I>(_distances))
                                             : (void)(0)),
         ...);
#else
        decompose((std::get<I>(_distances) == longest ? static_cast<void>(--std::get<I>(_iterators), --std::get<I>(_distances))
                                                      : (void)(0),
                   0)...);
#endif
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 void plus_is_one(const difference_type offset) {
        auto& it = std::get<I>(_iterators);
        const auto& end = std::get<I>(_end);
        auto& distance = std::get<I>(_distances);

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

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset, index_sequence<I...>) {
#ifdef LZ_HAS_CXX_17
        (plus_is_one<I>(offset), ...);
#else
        decompose((plus_is_one<I>(offset), 0)...);
#endif
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 void min_is_one(const difference_type longest, const difference_type offset) {
        auto& this_iter_length = std::get<I>(_distances);
        auto& it = std::get<I>(_iterators);

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

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void min_is(const difference_type offset, index_sequence<I...>) {
        const auto longest = std::max({ std::get<I>(_distances)... });
        LZ_ASSERT_SUBTRACTABLE(offset <= longest);
#ifdef LZ_HAS_CXX_17
        (min_is_one<I>(longest, offset), ...);
#else
        decompose((min_is_one<I>(longest, offset), 0)...);
#endif
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 difference_type minus(const zip_longest_iterator& other, index_sequence<I...>) const {
        const auto max = std::max({ (std::get<I>(_iterators) - std::get<I>(other._iterators))... });
        if (max > 0) {
            return max;
        }
        return std::min({ (std::get<I>(_iterators) - std::get<I>(other._iterators))... });
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 difference_type minus(index_sequence<I...>) const {
        return std::min({ (std::get<I>(_iterators) - std::get<I>(_end))... });
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 bool eq(index_sequence<I...>) const {
        return _iterators == _end;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 bool eq(const zip_longest_iterator& other, index_sequence<I...>) const {
        return _iterators == other._iterators;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void assign_sentinels(index_sequence<I...>) {
#ifdef LZ_HAS_CXX_17
        ((std::get<I>(_iterators) = std::get<I>(_end)), ...);
#else
        decompose(std::get<I>(_iterators) = std::get<I>(_end)...);
#endif
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr zip_longest_iterator()
        requires std::default_initializable<IterMaybeHomo> && std::default_initializable<SMaybeHomo>
    = default;

#else

    template<class I = IterMaybeHomo,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<SMaybeHomo>::value>>
    constexpr zip_longest_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                              std::is_nothrow_default_constructible<SMaybeHomo>::value) {
    }

#endif

    constexpr zip_longest_iterator(IterMaybeHomo it, SMaybeHomo end, difference_tuple distances) :
        _iterators{ std::move(it) },
        _end{ std::move(end) },
        _distances{ distances } {
        static_assert(tuple_size > 1, "Cannot zip one/zero iterables");
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
        LZ_ASSERT(_end == other._end, "Incompatible iterators");
        return minus(other, is{});
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(default_sentinel_t) const {
        return minus(is{});
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const zip_longest_iterator& b) const {
        LZ_ASSERT(_end == b._end, "Incompatible iterators");
        return eq(b, is{});
    }

    LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel_t) const {
        return eq(is{});
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_ZIP_LONGEST_ITERATOR_HPP
