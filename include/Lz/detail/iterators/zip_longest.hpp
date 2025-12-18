#pragma once

#ifndef LZ_ZIP_LONGEST_ITERATOR_HPP
#define LZ_ZIP_LONGEST_ITERATOR_HPP

#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/detail/iterator.hpp>
#include <Lz/detail/procs/assert.hpp>
#include <Lz/detail/procs/min_max.hpp>
#include <Lz/detail/traits/index_sequence.hpp>
#include <Lz/detail/tuple_helpers.hpp>
#include <Lz/util/default_sentinel.hpp>
#include <Lz/util/optional.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/procs/decompose.hpp>
#endif

namespace lz {
namespace detail {

template<class>
struct optional_iter_tuple_value_type;

template<class... Iterators>
struct optional_iter_tuple_value_type<std::tuple<Iterators...>> {
    using type = std::tuple<optional<val_t<Iterators>>...>;
};

template<class IterTuple>
using optional_value_type_iter_tuple_t = typename optional_iter_tuple_value_type<IterTuple>::type;

template<bool>
struct reference_or_value_type;

template<>
struct reference_or_value_type<true /* is lvalue reference */> {
    template<class Ref, class>
    using type = std::reference_wrapper<remove_ref_t<Ref>>;
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

template<class IterTuple>
using optional_iter_tuple_ref_type_t = typename optional_iter_tuple_ref_type<IterTuple>::type;

template<class, class...>
class zip_longest_iterator;

template<class... Iterables>
class zip_longest_iterator<std::input_iterator_tag, Iterables...>
    : public iterator<zip_longest_iterator<std::input_iterator_tag, Iterables...>,
                      optional_iter_tuple_ref_type_t<std::tuple<iter_t<Iterables>...>>,
                      fake_ptr_proxy<optional_iter_tuple_ref_type_t<std::tuple<iter_t<Iterables>...>>>,
                      iter_tuple_diff_type_t<std::tuple<iter_t<Iterables>...>>,
                      iter_tuple_iter_cat_t<std::tuple<iter_t<Iterables>...>>, default_sentinel_t> {

    using iters = std::tuple<iter_t<Iterables>...>;
    using sentinels = std::tuple<sentinel_t<Iterables>...>;

public:
    using iterator_category = iter_tuple_iter_cat_t<iters>;
    using value_type = optional_value_type_iter_tuple_t<iters>;
    using difference_type = iter_tuple_diff_type_t<iters>;
    using reference = optional_iter_tuple_ref_type_t<iters>;
    using pointer = fake_ptr_proxy<value_type>;

private:
    using is = make_index_sequence<tuple_size<iters>::value>;

    iters _iterators{};
    sentinels _end{};

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 auto deref_one() const
        -> optional<reference_or_value_type_t<ref_t<remove_cvref_t<decltype(std::get<I>(_iterators))>>,
                                              val_t<remove_cvref_t<decltype(std::get<I>(_iterators))>>>> {

        using iter_type = remove_cvref_t<decltype(std::get<I>(_iterators))>;
        using ref_or_value_type = optional<reference_or_value_type_t<ref_t<iter_type>, val_t<iter_type>>>;

        if (std::get<I>(_iterators) == std::get<I>(_end)) {
            return lz::nullopt;
        }

        return ref_or_value_type{ *std::get<I>(_iterators) };
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 reference dereference(index_sequence<I...>) const {
        return reference{ deref_one<I>()... };
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence<I...>) {
#ifdef LZ_HAS_CXX_17
        ((std::get<I>(_iterators) != std::get<I>(_end) ? static_cast<void>(++std::get<I>(_iterators)) : (void)(0)), ...);
#else
        decompose(
            (std::get<I>(_iterators) != std::get<I>(_end) ? static_cast<void>(++std::get<I>(_iterators)) : (void)(0), 0)...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_17 bool eq(const zip_longest_iterator& other, index_sequence<I...>) const {
#ifdef LZ_HAS_CXX_17
        return ((std::get<I>(_iterators) == std::get<I>(other._iterators)) && ...);
#else
        return min_variadic(static_cast<bool>(std::get<I>(_iterators) == std::get<I>(other._iterators))...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_17 bool eq(index_sequence<I...>) const {
#ifdef LZ_HAS_CXX_17
        return ((std::get<I>(_iterators) == std::get<I>(_end)) && ...);
#else
        return min_variadic(static_cast<bool>(std::get<I>(_iterators) == std::get<I>(_end))...);
#endif
    }

    template<size_t... I>
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
        requires(std::default_initializable<iters> && std::default_initializable<sentinels>)
    = default;

#else

    template<class I = iters,
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<sentinels>::value>>
    constexpr zip_longest_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                              std::is_nothrow_default_constructible<sentinels>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14 zip_longest_iterator(iters iterators, sentinels end) :
        _iterators{ std::move(iterators) },
        _end{ std::move(end) } {
        static_assert(tuple_size<iters>::value > 1, "Cannot concat one/zero iterables");
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

template<class... Iterables>
class zip_longest_iterator<std::forward_iterator_tag, Iterables...>
    : public zip_longest_iterator<std::input_iterator_tag, Iterables...> {
    using base = zip_longest_iterator<std::input_iterator_tag, Iterables...>;

public:
    using base::base;
    using base::operator=;
};

template<class... Iterables>
class zip_longest_iterator<std::bidirectional_iterator_tag, Iterables...>
    : public iterator<zip_longest_iterator<std::bidirectional_iterator_tag, Iterables...>,
                      optional_iter_tuple_ref_type_t<std::tuple<iter_t<Iterables>...>>,
                      fake_ptr_proxy<optional_iter_tuple_ref_type_t<std::tuple<iter_t<Iterables>...>>>,
                      iter_tuple_diff_type_t<std::tuple<iter_t<Iterables>...>>,
                      iter_tuple_iter_cat_t<std::tuple<iter_t<Iterables>...>>, default_sentinel_t> {

    using iters = std::tuple<iter_t<Iterables>...>;
    using sentinels = std::tuple<sentinel_t<Iterables>...>;

public:
    using iterator_category = iter_tuple_iter_cat_t<iters>;
    using value_type = optional_value_type_iter_tuple_t<iters>;
    using difference_type = iter_tuple_diff_type_t<iters>;
    using reference = optional_iter_tuple_ref_type_t<iters>;
    using pointer = fake_ptr_proxy<value_type>;

private:
    static constexpr size_t tup_size = tuple_size<iters>::value;
    using is = make_index_sequence<tup_size>;
    using difference_tuple = decltype(tuple_of<difference_type>(is{}));

    iters _iterators{};
    std::tuple<Iterables...> _iterables{};
    difference_tuple _distances{};

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 auto deref_one() const
        -> optional<reference_or_value_type_t<ref_t<remove_cvref_t<decltype(std::get<I>(_iterators))>>,
                                              val_t<remove_cvref_t<decltype(std::get<I>(_iterators))>>>> {

        using iter_type = remove_cvref_t<decltype(std::get<I>(_iterators))>;
        using ref_or_value_type = optional<reference_or_value_type_t<ref_t<iter_type>, val_t<iter_type>>>;

        if (std::get<I>(_iterators) == std::get<I>(_iterables).end()) {
            return lz::nullopt;
        }

        return ref_or_value_type{ *std::get<I>(_iterators) };
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 reference dereference(index_sequence<I...>) const {
        return reference{ deref_one<I>()... };
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void increment(index_sequence<I...>) {
#ifdef LZ_HAS_CXX_17
        ((std::get<I>(_iterators) != std::get<I>(_iterables).end()
              ? static_cast<void>(++std::get<I>(_iterators), ++std::get<I>(_distances))
              : (void)(0)),
         ...);
#else
        decompose((std::get<I>(_iterators) != std::get<I>(_iterables).end()
                       ? static_cast<void>(++std::get<I>(_iterators), ++std::get<I>(_distances))
                       : (void)(0),
                   0)...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void decrement(index_sequence<I...>) {
        const auto longest = max_variadic(std::get<I>(_distances)...);
        LZ_ASSERT_DECREMENTABLE(longest > 0);
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

    template<size_t I>
    LZ_CONSTEXPR_CXX_14 void plus_is_one(const difference_type offset) {
        auto& it = std::get<I>(_iterators);
        const auto end = std::get<I>(_iterables).end();
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

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void min_is(const difference_type offset, index_sequence<I...>) {
        const auto longest = max_variadic(std::get<I>(_distances)...);
        LZ_ASSERT_SUBTRACTABLE(offset <= longest);
#ifdef LZ_HAS_CXX_17
        (min_is_one<I>(longest, offset), ...);
#else
        decompose((min_is_one<I>(longest, offset), 0)...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 difference_type minus(const zip_longest_iterator& other, index_sequence<I...>) const {
        const auto max = max_variadic(static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(other._iterators))...);
        if (max > 0) {
            return max;
        }
        return min_variadic(static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(other._iterators))...);
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 difference_type minus(index_sequence<I...>) const {
        return min_variadic(static_cast<difference_type>(std::get<I>(_iterators) - std::get<I>(_iterables).end())...);
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 bool eq(index_sequence<I...>) const {
#ifdef LZ_HAS_CXX_17
        return ((std::get<I>(_iterators) == std::get<I>(_iterables).end()) && ...);
#else
        return min_variadic(static_cast<bool>(std::get<I>(_iterators) == std::get<I>(_iterables).end())...);
#endif
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 bool eq(const zip_longest_iterator& other, index_sequence<I...>) const {
        LZ_ASSERT_COMPATIBLE(
            min_variadic(static_cast<bool>(std::get<I>(_iterables).end() == std::get<I>(other._iterables).end())...) &&
            min_variadic(static_cast<bool>(std::get<I>(_iterables).begin() == std::get<I>(other._iterables).begin())...));
        return _iterators == other._iterators;
    }

    template<size_t... I>
    LZ_CONSTEXPR_CXX_14 void assign_sentinels(index_sequence<I...>) {
#ifdef LZ_HAS_CXX_17
        ((std::get<I>(_iterators) = std::get<I>(_iterables).end()), ...);
        ((std::get<I>(_distances) = lz::eager_ssize(std::get<I>(_iterables))), ...);
#else
        decompose(std::get<I>(_iterators) = std::get<I>(_iterables).end()...);
        decompose(std::get<I>(_distances) = lz::eager_ssize(std::get<I>(_iterables))...);
#endif
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr zip_longest_iterator()
        requires(std::default_initializable<iters> && std::default_initializable<sentinels>)
    = default;

#else

    template<class I = iters,
             class = enable_if_t<std::is_default_constructible<I>::value && std::is_default_constructible<sentinels>::value>>
    constexpr zip_longest_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                              std::is_nothrow_default_constructible<sentinels>::value) {
    }

#endif

    template<class I>
    constexpr zip_longest_iterator(I&& iterables, iters it, difference_tuple distances) :
        _iterators{ std::move(it) },
        _iterables{ std::forward<I>(iterables) },
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
        return minus(other, is{});
    }

    LZ_CONSTEXPR_CXX_14 difference_type difference(default_sentinel_t) const {
        return minus(is{});
    }

    LZ_CONSTEXPR_CXX_14 bool eq(const zip_longest_iterator& other) const {
        return eq(other, is{});
    }

    LZ_CONSTEXPR_CXX_20 bool eq(default_sentinel_t) const {
        return eq(is{});
    }
};

template<class... Iterables>
class zip_longest_iterator<std::random_access_iterator_tag, Iterables...>
    : public zip_longest_iterator<std::bidirectional_iterator_tag, Iterables...> {
    using base = zip_longest_iterator<std::bidirectional_iterator_tag, Iterables...>;

public:
    using base::base;
    using base::operator=;
};

#ifdef LZ_HAS_CXX_20

template<class... Iterables>
class zip_longest_iterator<std::contiguous_iterator_tag, Iterables...>
    : public zip_longest_iterator<std::random_access_iterator_tag, Iterables...> {
    using base = zip_longest_iterator<std::random_access_iterator_tag, Iterables...>;

public:
    using base::base;
    using base::operator=;
};

#endif

} // namespace detail
} // namespace lz
#endif // LZ_ZIP_LONGEST_ITERATOR_HPP
