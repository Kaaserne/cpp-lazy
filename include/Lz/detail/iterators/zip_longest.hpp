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

template<bool, class...>
class zip_longest_iterator;

template<class IterTuple, class SentinelTuple>
class zip_longest_iterator<false /*is bidirectional access*/, IterTuple, SentinelTuple>
    : public iter_base<zip_longest_iterator<false, IterTuple, SentinelTuple>, optional_iter_tuple_ref_type<IterTuple>,
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
    LZ_CONSTEXPR_CXX_20 void increment_one(I& iterator, const S& end) const {
        if (iterator == end) {
            return;
        }
        ++iterator;
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void decrement_one(I& iterator, const I& begin) const {
        if (iterator == begin) {
            return;
        }
        --iterator;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 reference dereference(index_sequence<I...>) const {
        return reference{ deref_one(std::get<I>(_iterators), std::get<I>(_end))... };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void increment(index_sequence<I...>) {
        decompose((increment_one(std::get<I>(_iterators), std::get<I>(_end)), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool eq(const zip_longest_iterator& other, index_sequence<I...>) const {
        // Cannot use auto here. Compiler is unable to deduce the type
        const bool expander[] = { (std::get<I>(_iterators) == std::get<I>(other._iterators))... };
        const auto end = std::end(expander);
        return std::find(std::begin(expander), end, false) == end;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool eq(index_sequence<I...>) const {
        // Cannot use auto here. Compiler is unable to deduce the type
        const bool expander[] = { (std::get<I>(_iterators) == std::get<I>(_end))... };
        const auto end = std::end(expander);
        return detail::find(std::begin(expander), end, false) == end;
    }

public:
    LZ_CONSTEXPR_CXX_20 explicit zip_longest_iterator(IterTuple iterators, SentinelTuple end) :
        _iterators(std::move(iterators)),
        _end(std::move(end)) {
    }

    constexpr zip_longest_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return dereference(make_idx_sequence_for_this());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        increment(make_idx_sequence_for_this());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const zip_longest_iterator& b) const {
        return eq(b, make_idx_sequence_for_this());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const {
        return eq(make_idx_sequence_for_this());
    }
};

template<class IterTuple>
class zip_longest_iterator<true /*is bidirectional access*/, IterTuple, IterTuple>
    : public iter_base<zip_longest_iterator<true, IterTuple, IterTuple>, optional_iter_tuple_ref_type<IterTuple>,
                       fake_ptr_proxy<optional_iter_tuple_ref_type<IterTuple>>, iter_tuple_diff_type_t<IterTuple>,
                       iter_tuple_iter_cat_t<IterTuple>> {
public:
    using iterator_category = iter_tuple_iter_cat_t<IterTuple>;
    using value_type = optional_value_type_iter_tuple_t<IterTuple>;
    using difference_type = iter_tuple_diff_type_t<IterTuple>;
    using reference = optional_iter_tuple_ref_type<IterTuple>;
    using pointer = fake_ptr_proxy<value_type>;

private:
    using make_idx_sequence_for_this = make_index_sequence<std::tuple_size<IterTuple>::value>;
    IterTuple _begin;
    IterTuple _iterators;
    IterTuple _end;

    template<class I>
    LZ_CONSTEXPR_CXX_20 optional<reference_or_value_type<ref_t<I>, val_t<I>>> deref_one(const I& iterator, const I& end) const {
        return iterator == end ? optional<reference_or_value_type<ref_t<I>, val_t<I>>>{}
                               : optional<reference_or_value_type<ref_t<I>, val_t<I>>>(*iterator);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void increment_one(I& iterator, const I& end) const {
        if (iterator == end) {
            return;
        }
        ++iterator;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 reference dereference(index_sequence<I...>) const {
        return reference{ deref_one(std::get<I>(_iterators), std::get<I>(_end))... };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void increment(index_sequence<I...>) {
        decompose((increment_one(std::get<I>(_iterators), std::get<I>(_end)), 0)...);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void decrement_one(I& iterator, const difference_type longest, const difference_type this_iter_length) {
        if (this_iter_length == longest) {
            --iterator;
        }
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void decrement(index_sequence<I...>) {
        const difference_type all_sizes[] = { static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(_begin))... };
        const auto longest = *std::max_element(std::begin(all_sizes), std::end(all_sizes));
        decompose((decrement_one(std::get<I>(_iterators), longest, all_sizes[I]), 0)...);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void plus_is_one(I& iterator, const I& end, const difference_type offset) {
        const auto difference = end - iterator;
        if (offset > difference) {
            iterator = end;
        }
        else {
            iterator += offset;
        }
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void plus_is(index_sequence<I...>, const difference_type offset) {
        decompose((plus_is_one(std::get<I>(_iterators), std::get<I>(_end), offset), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 difference_type minus(const zip_longest_iterator& other, index_sequence<I...>) const {
        const difference_type all_sizes[] = { static_cast<difference_type>(std::get<I>(_iterators) -
                                                                          std::get<I>(other._iterators))... };
        return *std::max_element(std::begin(all_sizes), std::end(all_sizes));
    }

    template<class I>
    LZ_CONSTEXPR_CXX_20 void
    min_is_one(I& iterator, const difference_type this_iter_length, const difference_type longest, const difference_type offset) {
        if (this_iter_length == longest) {
            iterator -= offset;
            return;
        }
        const auto surplus = this_iter_length + offset;
        if (surplus > longest) {
            iterator -= (surplus - longest);
        }
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void min_is(index_sequence<I...>, const difference_type offset) {
        const difference_type all_sizes[] = { static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(_begin))... };
        const auto longest = *std::max_element(std::begin(all_sizes), std::end(all_sizes));
        decompose((min_is_one(std::get<I>(_iterators), all_sizes[I], longest, offset), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool eq(const zip_longest_iterator& other, index_sequence<I...>) const {
        const bool expander[] = { (std::get<I>(_iterators) == std::get<I>(other._iterators))... };
        const auto end = std::end(expander);
        return std::find(std::begin(expander), end, false) == end;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool lt(const zip_longest_iterator& other, index_sequence<I...>) const {
        const difference_type all_sizes[] = { static_cast<difference_type>(std::get<I>(_iterators) -
                                                                          std::get<I>(other._iterators))... };
        return *std::min_element(std::begin(all_sizes), std::end(all_sizes)) < 0;
    }

public:
    LZ_CONSTEXPR_CXX_20 explicit zip_longest_iterator(IterTuple begin, IterTuple iterators, IterTuple end) :
        _begin(std::move(begin)),
        _iterators(std::move(iterators)),
        _end(std::move(end)) {
    }

    constexpr zip_longest_iterator() = default;

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return dereference(make_idx_sequence_for_this());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_20 void increment() {
        increment(make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        decrement(make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_20 void plus_is(const difference_type offset) {
        if (offset < 0) {
            min_is(make_idx_sequence_for_this(), -offset);
        }
        else {
            plus_is(make_idx_sequence_for_this(), offset);
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 difference_type difference(const zip_longest_iterator& other) const {
        return minus(other, make_idx_sequence_for_this());
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference operator[](const difference_type offset) const {
        return *(*this + offset);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool eq(const zip_longest_iterator& b) const {
        return eq(b, make_idx_sequence_for_this());
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_ZIP_LONGEST_ITERATOR_HPP