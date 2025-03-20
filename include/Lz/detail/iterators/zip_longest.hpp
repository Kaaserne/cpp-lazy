#pragma once

#ifndef LZ_ZIP_LONGEST_ITERATOR_HPP
#define LZ_ZIP_LONGEST_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
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

    template<class I, class S>
    LZ_CONSTEXPR_CXX_20 optional<reference_or_value_type<ref_t<I>, val_t<I>>> deref_one(const I& iterator, const S& end) const {
        return iterator == end ? optional<reference_or_value_type<ref_t<I>, val_t<I>>>{}
                               : optional<reference_or_value_type<ref_t<I>, val_t<I>>>(*iterator);
    }

    template<class I, class S>
    LZ_CONSTEXPR_CXX_14 void increment_one(I& iterator, const S& end) const {
        if (iterator == end) {
            return;
        }
        ++iterator;
    }

    template<class I>
    LZ_CONSTEXPR_CXX_14 void decrement_one(I& iterator, const I& begin) const {
        if (iterator == begin) {
            return;
        }
        --iterator;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 reference dereference(index_sequence_helper<I...>) const {
        return reference{ deref_one(std::get<I>(_iterators), std::get<I>(_end))... };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence_helper<I...>) {
        decompose((increment_one(std::get<I>(_iterators), std::get<I>(_end)), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool eq(const zip_longest_iterator& other, index_sequence_helper<I...>) const {
        // Cannot use auto here. Compiler is unable to deduce the type
        const bool expander[] = { (std::get<I>(_iterators) == std::get<I>(other._iterators))... };
        return detail::all_of(std::begin(expander), std::end(expander), [](bool val) { return val; });
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 bool eq(index_sequence_helper<I...>) const {
        // Cannot use auto here. Compiler is unable to deduce the type
        const bool expander[] = { (std::get<I>(_iterators) == std::get<I>(_end))... };
        return detail::all_of(std::begin(expander), std::end(expander), [](bool val) { return val; });
    }

public:
    LZ_CONSTEXPR_CXX_14 zip_longest_iterator(IterTuple iterators, SentinelTuple end) :
        _iterators{ std::move(iterators) },
        _end{ std::move(end) } {
    }

    constexpr zip_longest_iterator() = default;

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

template<std::size_t... Is>
std::tuple<decltype(Is, std::ptrdiff_t{})...> zeroes(index_sequence_helper<Is...>) {
    return std::make_tuple(decltype(Is, std::ptrdiff_t{})(0)...);
}

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
    using difference_tuple = decltype(zeroes(make_idx_sequence_for_this()));

    IterTuple _iterators;
    SentinelTuple _end;
    difference_tuple _distances;

    template<class I>
    LZ_CONSTEXPR_CXX_20 optional<reference_or_value_type<ref_t<I>, val_t<I>>> deref_one(const I& iterator, const I& end) const {
        return iterator == end ? optional<reference_or_value_type<ref_t<I>, val_t<I>>>{}
                               : optional<reference_or_value_type<ref_t<I>, val_t<I>>>(*iterator);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_14 void increment_one(I& iterator, const I& end, difference_type& distance) const {
        if (iterator == end) {
            return;
        }
        ++iterator;
        ++distance;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 reference dereference(index_sequence_helper<I...>) const {
        return reference{ deref_one(std::get<I>(_iterators), std::get<I>(_end))... };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence_helper<I...>) {
        decompose((increment_one(std::get<I>(_iterators), std::get<I>(_end), std::get<I>(_distances)), 0)...);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_14 void decrement_one(I& iterator, const difference_type longest, difference_type& this_iter_length) {
        if (this_iter_length == longest) {
            --iterator;
            --this_iter_length;
        }
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void decrement(index_sequence_helper<I...>) {
        const auto longest = std::max({ std::get<I>(_distances)... });
        decompose((decrement_one(std::get<I>(_iterators), longest, std::get<I>(_distances)), 0)...);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_14 void plus_is_one(I& iterator, const I& end, const difference_type offset, difference_type& distance) {
        const auto difference = end - iterator;
        if (offset > difference) {
            iterator = end;
            distance = difference;
        }
        else {
            iterator += offset;
            distance += offset;
        }
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void plus_is(index_sequence_helper<I...>, const difference_type offset) {
        decompose((plus_is_one(std::get<I>(_iterators), std::get<I>(_end), offset, std::get<I>(_distances)), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 difference_type minus(const zip_longest_iterator& other, index_sequence_helper<I...>) const {
        const difference_type expand[] = { static_cast<difference_type>(
            (std::get<I>(_distances) - std::get<I>(other._distances)))... };
        const auto min_max = std::minmax_element(std::begin(expand), std::end(expand));
        return *min_max.second <= 0 ? *min_max.first : *min_max.second;
    }

    template<class I>
    LZ_CONSTEXPR_CXX_14 void
    min_is_one(I& iterator, difference_type& this_iter_length, const difference_type longest, const difference_type offset) {
        if (this_iter_length == longest) {
            iterator -= offset;
            this_iter_length -= offset;
            return;
        }
        const auto surplus = this_iter_length + offset;
        if (surplus > longest) {
            iterator -= (surplus - longest);
            this_iter_length -= (surplus - longest);
        }
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void min_is(index_sequence_helper<I...>, const difference_type offset) {
        const auto longest = std::max({ std::get<I>(_distances)... });
        decompose((min_is_one(std::get<I>(_iterators), std::get<I>(_distances), longest, offset), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool eq(const zip_longest_iterator& other, index_sequence_helper<I...>) const {
        return std::min({ (std::get<I>(_iterators) == std::get<I>(other._iterators))... });
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool lt(const zip_longest_iterator& other, index_sequence_helper<I...>) const {
        return std::min({ static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(other._iterators))... }) < 0;
    }

public:
    constexpr zip_longest_iterator(IterTuple iterator, SentinelTuple end, difference_tuple distances) :
        _iterators{ std::move(iterator) },
        _end{ std::move(end) },
        _distances{ distances } {
        static_assert(tuple_size > 1, "Cannot zip one/zero iterables");
    }

    constexpr zip_longest_iterator() = default;

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
};
} // namespace detail
} // namespace lz
#endif // LZ_ZIP_LONGEST_ITERATOR_HPP
