#pragma once

#ifndef LZ_ZIP_ITERATOR_HPP
#define LZ_ZIP_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/traits.hpp>
#include <Lz/iterator_base.hpp>
#include <algorithm>

namespace lz {
namespace detail {

template<class IterTuple, class SentinelTuple>
class zip_iterator
    : public iterator<
          zip_iterator<IterTuple, SentinelTuple>, iter_tuple_ref_type_t<IterTuple>,
          fake_ptr_proxy<iter_tuple_ref_type_t<IterTuple>>, iter_tuple_diff_type_t<IterTuple>, iter_tuple_iter_cat_t<IterTuple>,
          sentinel_selector<iter_tuple_iter_cat_t<IterTuple>, zip_iterator<IterTuple, SentinelTuple>, SentinelTuple>> {

public:
    using iterator_category = iter_tuple_iter_cat_t<IterTuple>;
    using value_type = iter_tuple_value_type_t<IterTuple>;
    using difference_type = iter_tuple_diff_type_t<IterTuple>;
    using reference = iter_tuple_ref_type_t<IterTuple>;
    using pointer = fake_ptr_proxy<reference>;

private:
    using make_idx_sequence_for_this = make_index_sequence<std::tuple_size<IterTuple>::value>;
    IterTuple _iterators;

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 reference dereference(index_sequence_helper<I...>) const {
        return reference{ *std::get<I>(_iterators)... };
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence_helper<I...>) {
        decompose((++std::get<I>(_iterators), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void decrement(index_sequence_helper<I...>) {
        decompose((--std::get<I>(_iterators), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_14 void plus_is(index_sequence_helper<I...>, const difference_type offset) {
        decompose(((std::get<I>(_iterators) += offset), 0)...);
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 difference_type minus(const zip_iterator& other, index_sequence_helper<I...>) const {
        const difference_type expand[] = { static_cast<difference_type>(
            (std::get<I>(_iterators) - std::get<I>(other._iterators)))... };
        const auto min_max = std::minmax_element(std::begin(expand), std::end(expand));
        return *min_max.second < 0 ? *min_max.second : *min_max.first;
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool eq(const zip_iterator& other, index_sequence_helper<I...>) const {
        return eq(other._iterators, index_sequence_helper<I...>{});
    }

    template<std::size_t... I>
    LZ_CONSTEXPR_CXX_20 bool eq(const SentinelTuple& other, index_sequence_helper<I...>) const {
        return std::max({ (std::get<I>(_iterators) == std::get<I>(other))... });
    }

public:
    LZ_CONSTEXPR_CXX_14 zip_iterator(IterTuple iterators) : _iterators{ std::move(iterators) } {
        static_assert(std::tuple_size<IterTuple>::value > 1, "Cannot concat one/zero iterables");
    }

    constexpr zip_iterator() = default;

    LZ_CONSTEXPR_CXX_14 reference dereference() const {
        return dereference(make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<decltype(**this)>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        increment(make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        decrement(make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_14 void plus_is(const difference_type offset) {
        plus_is(make_idx_sequence_for_this(), offset);
    }

    LZ_CONSTEXPR_CXX_20 difference_type difference(const zip_iterator& other) const {
        return minus(other, make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const zip_iterator& b) const {
        return eq(b, make_idx_sequence_for_this());
    }

    LZ_CONSTEXPR_CXX_20 bool eq(const SentinelTuple& b) const {
        return eq(b, make_idx_sequence_for_this());
    }
};
} // namespace detail
} // namespace lz

#endif