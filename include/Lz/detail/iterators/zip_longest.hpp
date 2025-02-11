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
    using type = std::reference_wrapper<remove_reference_t<Ref>>;
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
class zip_longest_iterator<false /*is bidirectional access*/, IterTuple, SentinelTuple>
    : public iter_base<zip_longest_iterator<false, IterTuple, SentinelTuple>, optional_iter_tuple_ref_type<IterTuple>,
                       fake_ptr_proxy<optional_iter_tuple_ref_type<IterTuple>>, iter_tuple_diff_type_t<IterTuple>,
                       iter_tuple_iter_cat_t<IterTuple>, default_sentinel> {
public:
    using iterator_category = iter_tuple_iter_cat_t<IterTuple>;
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
        if (iterator == end) {
            return nullopt;
        }
        return optional<reference_or_value_type<ref_t<I>, val_t<I>>>(*iterator);
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
        return eq(b, make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel) const {
        return eq(make_idx_sequence_for_this());
    }
};

template<std::size_t... I>
LZ_CONSTEXPR_CXX_14 std::tuple<decltype(I, bool{})...> create_bool_tuple(std::index_sequence<I...>) noexcept {
    return std::tuple<decltype(I, bool{})...>{};
}

template<class IterTuple, class SentinelTuple>
class zip_longest_iterator<true /*is bidirectional access*/, IterTuple, SentinelTuple>
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
    using bool_tuple = decltype(create_bool_tuple(make_idx_sequence_for_this{}));

    IterTuple _begin;
    IterTuple _iterators;
    SentinelTuple _end;
    bool_tuple _reached_end{};

    template<class I>
    LZ_CONSTEXPR_CXX_20 optional<reference_or_value_type<ref_t<I>, val_t<I>>>
    deref_one(const I& iterator, const bool reached_end) const {
        if (reached_end) {
            return nullopt;
        }
        return optional<reference_or_value_type<ref_t<I>, val_t<I>>>(*iterator);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 reference dereference(index_sequence_helper<I...>) const {
        return reference{ deref_one(std::get<I>(_iterators), std::get<I>(_reached_end))... };
    }

    template<class I, class S>
    LZ_CONSTEXPR_CXX_14 void increment_one(I& iterator, const S& end, bool& reached_end) const {
        if (reached_end) {
            return;
        }
        ++iterator;
        reached_end = iterator == end;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence_helper<I...>) {
        decompose((increment_one(std::get<I>(_iterators), std::get<I>(_end), std::get<I>(_reached_end)), 0)...);
    }

    template<class I>
    LZ_CONSTEXPR_CXX_14 void decrement_one(I& iterator, const I& begin, bool& reached_end) const {
        reached_end = iterator == begin;
        if (reached_end) {
            return;
        }
        --iterator;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void decrement(index_sequence_helper<I...>) {
        decompose((decrement_one(std::get<I>(_iterators), std::get<I>(_begin), std::get<I>(_reached_end)), 0)...);
    }

    template<class I, class S>
    LZ_CONSTEXPR_CXX_14 void plus_is_one(I& iterator, const S& end, const difference_type offset, bool& reached_end) {
        if (reached_end) {
            return;
        }
        const auto difference = end - iterator;
        if (offset > difference) {
            iterator = end;
        }
        else {
            iterator += offset;
        }
        reached_end = iterator == end;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void plus_is(index_sequence_helper<I...>, const difference_type offset) {
        decompose((plus_is_one(std::get<I>(_iterators), std::get<I>(_end), offset, std::get<I>(_reached_end)), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 difference_type minus(const zip_longest_iterator& other, index_sequence_helper<I...>) const {
        const difference_type all_sizes[] = { static_cast<difference_type>(std::get<I>(_iterators) -
                                                                           std::get<I>(other._iterators))... };
        return *std::max_element(std::begin(all_sizes), std::end(all_sizes));
    }

    template<class I>
    LZ_CONSTEXPR_CXX_14 void
    min_is_one(I& iterator, const difference_type this_iter_length, const difference_type offset, bool& reached_end) {
        reached_end = iterator == std::get<I>(_begin);
        if (reached_end) {
            return;
        }
        if (offset == this_iter_length) {
            iterator -= this_iter_length;
        }
        else if (offset > this_iter_length) {
            iterator -= this_iter_length;
            reached_end = true;
        }
        else {
            iterator -= offset;
        }
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 void min_is(index_sequence_helper<I...>, const difference_type offset) {
        const difference_type all_sizes[] = { static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(_begin))... };
        decompose((min_is_one(std::get<I>(_iterators), all_sizes[I], offset, std::get<I>(_reached_end)), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool eq(const zip_longest_iterator& other, index_sequence_helper<I...>) const {
        const bool expander[] = { (std::get<I>(_iterators) == std::get<I>(other._iterators))... };
        const auto end = std::end(expander);
        return std::find(std::begin(expander), end, false) == end;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool lt(const zip_longest_iterator& other, index_sequence_helper<I...>) const {
        const difference_type all_sizes[] = { static_cast<difference_type>(std::get<I>(_iterators) -
                                                                           std::get<I>(other._iterators))... };
        return *std::min_element(std::begin(all_sizes), std::end(all_sizes)) < 0;
    }

    template<std::size_t... Is>
    LZ_CONSTEXPR_CXX_14 bool_tuple init_bool_tuple(index_sequence_helper<Is...>) const {
        return bool_tuple{ (std::get<Is>(_begin) == std::get<Is>(_end))... };
    }

public:
    LZ_CONSTEXPR_CXX_14 zip_longest_iterator(IterTuple begin, IterTuple iterators, IterTuple end) :
        _begin{ std::move(begin) },
        _iterators{ std::move(iterators) },
        _end{ std::move(end) },
        _reached_end{ init_bool_tuple(make_idx_sequence_for_this{}) } {
    }

    constexpr zip_longest_iterator() = default;

    LZ_CONSTEXPR_CXX_20 reference dereference() const {
        return dereference(make_idx_sequence_for_this{});
    }

    LZ_CONSTEXPR_CXX_20 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        increment(make_idx_sequence_for_this{});
    }

    LZ_CONSTEXPR_CXX_20 void decrement() {
        decrement(make_idx_sequence_for_this{});
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        if (offset < 0) {
            min_is(make_idx_sequence_for_this{}, -offset);
        }
        else {
            plus_is(make_idx_sequence_for_this{}, offset);
        }
    }

    LZ_CONSTEXPR_CXX_20 difference_type difference(const zip_longest_iterator& other) const {
        return minus(other, make_idx_sequence_for_this{});
    }

    LZ_CONSTEXPR_CXX_14 reference operator[](const difference_type offset) const {
        return *(*this + offset);
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const zip_longest_iterator& b) const {
        return eq(b, make_idx_sequence_for_this{});
    }
};
} // namespace detail
} // namespace lz
#endif // LZ_ZIP_LONGEST_ITERATOR_HPP