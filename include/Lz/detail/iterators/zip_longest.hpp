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

template<class IterTuple>
using optional_value_type_iter_tuple_t = typename optional_iter_tuple_value_type_helper<IterTuple>::type;

template<bool>
struct reference_or_value_type_helper;

template<>
struct reference_or_value_type_helper<true /* is lvalue reference */> {
    template<class Ref, class>
    using type = std::reference_wrapper<typename std::remove_reference<Ref>::type>;
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

template<class IterTuple>
using optional_iter_tuple_ref_type = typename optional_iter_tuple_ref_type_helper<IterTuple>::type;

template<bool, class, class>
class zip_longest_iterator;

template<class IterTuple, class SentinelTuple>
class zip_longest_iterator<false /* bidi */, IterTuple, SentinelTuple>
    : public iterator<zip_longest_iterator<false, IterTuple, SentinelTuple>, optional_iter_tuple_ref_type<IterTuple>,
                      fake_ptr_proxy<optional_iter_tuple_ref_type<IterTuple>>, iter_tuple_diff_type_t<IterTuple>,
                      std::forward_iterator_tag, default_sentinel> {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = optional_value_type_iter_tuple_t<IterTuple>;
    using difference_type = iter_tuple_diff_type_t<IterTuple>;
    using reference = optional_iter_tuple_ref_type<IterTuple>;
    using pointer = fake_ptr_proxy<value_type>;

private:
    using make_idx_sequence_for_this = make_index_sequence<std::tuple_size<IterTuple>::value>;

    IterTuple _iterators;
    SentinelTuple _end;

    template<std::size_t I, class It = remove_ref<decltype(std::get<I>(_iterators))>, class Ref = ref_t<It>,
             class Val = val_t<It>>
    LZ_CONSTEXPR_CXX_14 optional<reference_or_value_type<Ref, Val>> deref_one() const {
        const auto& it = std::get<I>(_iterators);
        const auto& end = std::get<I>(_end);

        return it == end ? optional<reference_or_value_type<Ref, Val>>{} : optional<reference_or_value_type<Ref, Val>>(*it);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 reference dereference(index_sequence<I...>) const {
        return reference{ deref_one<I>()... };
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 void increment_one() {
        auto& it = std::get<I>(_iterators);
        const auto& end = std::get<I>(_end);

        if (it == end) {
            return;
        }
        ++it;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence<I...>) {
        decompose((increment_one<I>(), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 bool eq(const zip_longest_iterator& other, index_sequence<I...>) const {
        return std::min({ std::get<I>(_iterators) == std::get<I>(other._iterators)... });
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 bool eq(index_sequence<I...>) const {
        return std::min({ std::get<I>(_iterators) == std::get<I>(_end)... });
    }

public:
    LZ_CONSTEXPR_CXX_14 zip_longest_iterator(IterTuple iterators, SentinelTuple end) :
        _iterators{ std::move(iterators) },
        _end{ std::move(end) } {
        static_assert(std::tuple_size<IterTuple>::value > 1, "Cannot concat one/zero iterables");
    }

    constexpr zip_longest_iterator() = default;

    LZ_CONSTEXPR_CXX_14 zip_longest_iterator& operator=(default_sentinel) {
        _iterators = _end;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return dereference(make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        increment(make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const zip_longest_iterator& b) const {
        LZ_ASSERT(_end == b._end, "Incompatible iterators");
        return eq(b, make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const {
        return eq(make_idx_sequence_for_this());
    }
};

template<class IterTuple, class SentinelTuple>
class zip_longest_iterator<true /* bidi */, IterTuple, SentinelTuple>
    : public iterator<zip_longest_iterator<true, IterTuple, IterTuple>, optional_iter_tuple_ref_type<IterTuple>,
                      fake_ptr_proxy<optional_iter_tuple_ref_type<IterTuple>>, iter_tuple_diff_type_t<IterTuple>,
                      iter_tuple_iter_cat_t<IterTuple>> {
public:
    using iterator_category = iter_tuple_iter_cat_t<IterTuple>;
    using value_type = optional_value_type_iter_tuple_t<IterTuple>;
    using difference_type = iter_tuple_diff_type_t<IterTuple>;
    using reference = optional_iter_tuple_ref_type<IterTuple>;
    using pointer = fake_ptr_proxy<value_type>;

    static constexpr std::size_t tuple_size = std::tuple_size<IterTuple>::value;

private:
    using make_idx_sequence_for_this = make_index_sequence<std::tuple_size<IterTuple>::value>;
    using difference_tuple = decltype(tuple_of_n<difference_type>(make_idx_sequence_for_this()));

    IterTuple _iterators;
    SentinelTuple _end;
    difference_tuple _distances;

    template<std::size_t I, class It = remove_ref<decltype(std::get<I>(_iterators))>, class Ref = ref_t<It>,
             class Val = val_t<It>>
    LZ_CONSTEXPR_CXX_14 optional<reference_or_value_type<Ref, Val>> deref_one() const {
        const auto& it = std::get<I>(_iterators);
        const auto& end = std::get<I>(_end);

        return it == end ? optional<reference_or_value_type<Ref, Val>>{} : optional<reference_or_value_type<Ref, Val>>(*it);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 reference dereference(index_sequence<I...>) const {
        return reference{ deref_one<I>()... };
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 void increment_one() {
        auto& it = std::get<I>(_iterators);
        const auto& end = std::get<I>(_end);
        auto& distance = std::get<I>(_distances);

        if (it == end) {
            return;
        }
        ++it;
        ++distance;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence<I...>) {
        decompose((increment_one<I>(), 0)...);
    }

    template<std::size_t I>
    LZ_CONSTEXPR_CXX_14 void decrement_one(const difference_type longest) {
        auto& it = std::get<I>(_iterators);
        auto& this_iter_length = std::get<I>(_distances);

        if (this_iter_length == longest) {
            --it;
            --this_iter_length;
        }
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void decrement(index_sequence<I...>) {
        const auto longest = std::max({ std::get<I>(_distances)... });
        decompose((decrement_one<I>(longest), 0)...);
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
    LZ_CONSTEXPR_CXX_14 void plus_is(index_sequence<I...>, const difference_type offset) {
        decompose((plus_is_one<I>(offset), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 difference_type minus(const zip_longest_iterator& other, index_sequence<I...>) const {
        const auto min_max = std::minmax({ (std::get<I>(_iterators) - std::get<I>(other._iterators))... });
        return min_max.second <= 0 ? min_max.first : min_max.second;
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
    LZ_CONSTEXPR_CXX_14 void min_is(index_sequence<I...>, const difference_type offset) {
        const auto longest = std::max({ std::get<I>(_distances)... });
        decompose((min_is_one<I>(longest, offset), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 bool eq(index_sequence<I...>) const {
        return std::min({ std::get<I>(_iterators) == std::get<I>(_end)... });
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 bool eq(const zip_longest_iterator& other, index_sequence<I...>) const {
        return std::min({ (std::get<I>(_iterators) == std::get<I>(other._iterators))... });
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 bool lt(const zip_longest_iterator& other, index_sequence<I...>) const {
        return std::min({ static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(other._iterators))... });
    }

public:
    constexpr zip_longest_iterator(IterTuple it, SentinelTuple end, difference_tuple distances) :
        _iterators{ std::move(it) },
        _end{ std::move(end) },
        _distances{ distances } {
        static_assert(tuple_size > 1, "Cannot zip one/zero iterables");
    }

    constexpr zip_longest_iterator() = default;

    LZ_CONSTEXPR_CXX_14 zip_longest_iterator& operator=(default_sentinel) {
        _iterators = _end;
        return *this;
    }

    LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return dereference(make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        increment(make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        decrement(make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        if (offset < 0) {
            min_is(make_idx_sequence_for_this(), -offset);
        }
        else {
            plus_is(make_idx_sequence_for_this(), offset);
        }
    }

    LZ_CONSTEXPR_CXX_20 difference_type difference(const zip_longest_iterator& other) const {
        LZ_ASSERT(_end == other._end, "Incompatible iterators");
        return minus(other, make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const zip_longest_iterator& b) const {
        LZ_ASSERT(_end == b._end, "Incompatible iterators");
        return eq(b, make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const {
        return eq(make_idx_sequence_for_this());
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_ZIP_LONGEST_ITERATOR_HPP
