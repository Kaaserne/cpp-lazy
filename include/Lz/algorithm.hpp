#pragma once

#ifndef LZ_ALGORITHM_HPP
#define LZ_ALGORITHM_HPP

#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/traits.hpp>

// Only include if 20 is defined because std::accumulate pre CXX 20 doesn't use std::move
#ifdef LZ_HAS_CXX_20
#include <numeric> // std::accumulate
#endif             // LZ_HAS_CXX_20

LZ_MODULE_EXPORT namespace lz {

/**
 * Checks whether [begin, end) is empty.
 * @param begin The beginning of the sequence.
 * @param end The end of the sequence.
 * @return True if it is empty, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD constexpr bool empty(Iterable&& iterable) {
    return detail::empty(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)));
}

/**
 * Returns whether the sequence holds exactly one element.
 * @param iterable The sequence to check whether is has exactly one element.
 * @return True if it has one element exactly, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool has_one(Iterable&& iterable) {
    return detail::has_one(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)));
}

/**
 * Returns whether the sequence holds >= 2 elements.
 * @param iterable The sequence to check whether it has many (>= 2) elements.
 * @return True if the amount of values are >= 2, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool has_many(Iterable&& iterable) {
    return detail::has_many(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)));
}

/**
 * @brief Returns an optional reference(_wrapper (if the sequence has an actual reference)) to the next element in the sequence.
 * If the iterable is already at end, an empty optional is returned.
 *
 * @param iterable
 * @return A (std::)optional of reference wrapper if the sequence is a reference, value if it is not.
 * detail::end(std::forward<Iterable>(iterable))))
 */
template<class Iterator, class S>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 auto peek(Iterator begin, S end) -> decltype(detail::peek(begin, end)) {
    return detail::peek(begin, end);
}

/**
 * @brief Returns an optional reference(_wrapper (if the sequence has an actual reference)) to the next element in the sequence.
 * If the iterable is already at end, an empty optional is returned.
 * @param iterable
 * @return A (std::)optional of reference wrapper if the sequence is a reference, value if it is not.
 */
template<class Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 auto peek(Iterable&& iterable)
    -> decltype(lz::peek(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)))) {
    return lz::peek(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)));
}

/**
 * @brief Gets the first element of the sequence. The sequence cannot be empty.
 *
 * @param iterable The iterable to get the first value of.
 * @return (A reference to) the first element of the sequence.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 ref_t<iter_t<Iterable>> front(Iterable&& iterable) {
    LZ_ASSERT(!lz::empty(iterable), "Cannot get the front element of an empty iterable.");
    return *std::begin(iterable);
}

/**
 * @brief Gets the end element of the sequence. The sequence cannot be empty. `iterable.begin()` must be (at least) a
 * bidirectional iterator.
 *
 * @param iterable The iterable to get the end value of.
 * @return (A reference to) the end element of the sequence.
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 ref_t<iter_t<Iterable>> back(Iterable&& iterable) {
    static_assert(lz::detail::has_sentinel<Iterable>::value ? lz::detail::is_ra<iter_t<Iterable>>::value
                                                            : lz::detail::is_bidi<iter_t<Iterable>>::value,
                  "If iterable has sentinels, it must be random access, otherwise it must be bidirectional.");
    LZ_ASSERT(!lz::empty(iterable), "Cannot get the back element of an empty iterable.");
    return detail::back(std::forward<Iterable>(iterable));
}

/**
 * Gets the first element of the sequence, or `default_value` if the sequence is empty.
 * @param iterable The iterable to get the first value of, or `value` in case it is empty.
 * @param default_value The value to return if `iterable` is empty.
 * @return Either the first element of `iterable` or `value` if the sequence is empty.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T>
LZ_NODISCARD constexpr val_iterable_t<Iterable> front_or(Iterable&& iterable, T&& default_value) {
    return lz::empty(iterable) ? std::forward<T>(default_value) : lz::front(iterable);
}

/**
 * This function returns the end element. If the sequence is empty, it returns `value`.
 * @param iterable The iterable to get the end value of, or `value` in case it is empty.
 * @param default_value The value to return if `iterable` is empty.
 * @return Either the end element of `iterable` or `value` if the sequence is empty.
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable, class T>
LZ_NODISCARD constexpr val_iterable_t<Iterable> back_or(Iterable&& iterable, T&& default_value) {
    return lz::empty(iterable) ? std::forward<T>(default_value) : lz::back(iterable);
}

/**
 * @brief Accumulates the values in the range [begin, end) using the binary operator @p binary_op
 *
 * @param iterable The iterable to accumulate
 * @param init The initial value to start accumulating with
 * @param binary_op The binary operator to accumulate the values with
 * @return The accumulated value
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T, class BinaryOp = MAKE_BIN_PRED(plus)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 T accumulate(Iterable&& iterable, T&& init, BinaryOp&& binary_op = {}) {
#ifdef LZ_HAS_CXX_20

    using detail::accumulate;
    using std::accumulate;
    return accumulate(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                      std::forward<T>(init), std::forward<BinaryOp>(binary_op));
#else

    // CXX20 uses std::move, but everything below 20 does not.
    return detail::accumulate(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                              std::forward<T>(init), std::forward<BinaryOp>(binary_op));

#endif // LZ_HAS_CXX_20
}

/**
 * @brief Gets the maximum element in the range [begin, end) using the binary binary_predicate @p binary_predicate
 *
 * @param iterable The iterable to find the maximum element in
 * @param binary_predicate The binary operator to find the maximum element with
 * @return The maximum element in the range, if the range is empty, the return value is `end`
 */
template<LZ_CONCEPT_ITERABLE Iterable, class BinaryPredicate = MAKE_BIN_PRED(less)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> max_element(Iterable&& iterable, BinaryPredicate&& binary_predicate = {}) {
    using detail::max_element;
    using std::max_element;
    return max_element(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                       std::forward<BinaryPredicate>(binary_predicate));
}

/**
 * @brief Finds the minimum element in the range [begin, end) using the binary binary_predicate @p binary_predicate
 *
 * @param iterable The iterable to find the minimum element in
 * @param binary_predicate The binary operator to find the minimum element with
 * @return The minimum element in the range or `end` if the range is empty
 */
template<LZ_CONCEPT_ITERABLE Iterable, class BinaryPredicate = MAKE_BIN_PRED(less)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> min_element(Iterable&& iterable, BinaryPredicate&& binary_predicate = {}) {
    using detail::min_element;
    using std::min_element;
    return min_element(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                       std::forward<BinaryPredicate>(binary_predicate));
}

/**
 * @brief Finds the first element in the range [begin, end) that satisfies the binary_predicate @p binary_predicate
 *
 * @param iterable The iterable to find the element in
 * @param unary_predicate The binary_predicate to find the element with
 * @return Iterator to the first element that satisfies the binary_predicate or `end(iterable)` if the element is not found
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> find_if(Iterable&& iterable, UnaryPredicate&& unary_predicate) {
    using detail::find_if;
    using std::find_if;
    return find_if(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                   std::forward<UnaryPredicate>(unary_predicate));
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the value @p value
 *
 * @param iterable The iterable to find the element in
 * @param value The value to find
 * @return The iterator to the first element that satisfies the value or `end(iterable)` if the element is not found
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> find(Iterable&& iterable, const T& value) {
    using detail::find;
    using std::find;
    return find(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), value);
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the value @p value.
 * Note that when @p iterable has a sentinel, the entire range is searched. Does not use `lz::cached_reverse` to
 * reverse the elements.
 * @param iterable The iterable to find the element in
 * @param value The value to find
 * @return An iterator to the first element that satisfies the unary_predicate or `end(iterable)` if the element is not found
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> find_last(Iterable&& iterable, const T& value) {
    return detail::find_last(std::forward<Iterable>(iterable), value);
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the unary_predicate @p
 * unary_predicate. Note that when @p iterable has a sentinel, the entire range is searched. Does not use `lz::cached_reverse` to
 * reverse the elements.
 * @param iterable The iterable to find the element in
 * @param unary_predicate The unary_predicate to find the element with
 * @return An iterator to the first element that satisfies the unary_predicate or `end(iterable)` if the element is not found
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> find_last_if(Iterable&& iterable, UnaryPredicate&& unary_predicate) {
    return detail::find_last_if(std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(unary_predicate));
}

/**
 * @brief Searches for the first occurrence of the range [begin(iterable2), end(iterable2)) in the range [begin(iterable),
 * end(iterable))
 *
 * @param iterable The iterable to search in
 * @param iterable2 The iterable to search for
 * @param binary_predicate The binary predicate to search with
 * @return The iterator to the first occurrence of the range [begin(iterable2), end(iterable2)) in the range [begin(iterable),
 * end(iterable)) or `end(iterable)` if the range is not found
 */
template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE Iterable2, class BinaryPredicate = MAKE_BIN_PRED(equal_to)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::pair<iter_t<Iterable>, iter_t<Iterable>>
search(Iterable&& iterable, Iterable2&& iterable2, BinaryPredicate&& binary_predicate = {}) {
    return detail::search(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                          detail::begin(std::forward<Iterable>(iterable2)), detail::end(std::forward<Iterable>(iterable2)),
                          std::forward<BinaryPredicate>(binary_predicate));
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that does not satisfy the unary_predicate @p
 * unary_predicate
 *
 * @param iterable The iterable to find the element in
 * @param unary_predicate The unary_predicate to find the element with
 * @return Iterator to the first element that does not satisfy the unary_predicate or `end(iterable)` if the element is not
 * found
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> find_if_not(Iterable&& iterable, UnaryPredicate&& unary_predicate) {
    using detail::find_if_not;
    using std::find_if_not;
    return find_if_not(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                       std::forward<UnaryPredicate>(unary_predicate));
}

/**
 * @brief Finds the last element in the range [begin(iterable), end(iterable)) that does not satisfy the unary_predicate @p
 * unary_predicate. Note that when @p iterable has a sentinel, the entire range is searched. Does not use `lz::cached_reverse` to
 * reverse the elements.
 *
 * @param iterable The iterable to find the element in
 * @param unary_predicate The unary_predicate to find the element with
 * @return Iterator to the last element that does not satisfy the unary_predicate or `end(iterable)` if the element is not
 * found
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> find_last_if_not(Iterable&& iterable, UnaryPredicate&& unary_predicate) {
    return detail::find_last_if_not(std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(unary_predicate));
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the value @p to_find. If the
 * element is found, it returns the value, otherwise it returns @p default_value
 * @param iterable The iterable to find the element in
 * @param to_find The value to find
 * @param default_value The value to return if the element is not found, defaults to `T()`
 * @return The value @p to_find if it is found, otherwise @p default_value
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T, class U = detail::remove_cvref<T>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 val_iterable_t<Iterable>
find_or_default(Iterable&& iterable, T&& to_find, U&& default_value = {}) {
    return detail::find_or_default(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                                   std::forward<T>(to_find), std::forward<U>(default_value));
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that satisfies the unary_predicate @p
 * unary_predicate. If the element is found, it returns the value, otherwise it returns @p default_value
 * @param iterable The iterable to search.
 * @param unary_predicate The search unary_predicate in [begin(iterable), end(iterable)). Must return bool.
 * @param default_value The value to return when no element matches unary_predicate @p unary_predicate in [begin(iterable),
 * end(iterable)). Defaults to `Iterable::value_type()`
 * @return The value if it is found, otherwise @p default_value
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate, class U = val_iterable_t<Iterable>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 val_iterable_t<Iterable>
find_or_default_if(Iterable&& iterable, UnaryPredicate&& unary_predicate, U&& default_value = {}) {
    return detail::find_or_default_if(detail::begin(std::forward<Iterable>(iterable)),
                                      detail::end(std::forward<Iterable>(iterable)),
                                      std::forward<UnaryPredicate>(unary_predicate), std::forward<U>(default_value));
}

/**
 * @brief Finds the last element in the range [begin(iterable), end(iterable)) that satisfies the value @p to_find. If the
 * element is found, it returns the value, otherwise it returns @p default_value. Note that when @p iterable has a sentinel,
 * the entire range is searched. Does not use `lz::cached_reverse` to reverse the elements.
 * @param iterable The iterable to find the element in
 * @param to_find The value to find
 * @param default_value The value to return when no element matches @p to_find in [begin(iterable), end(iterable)). Defaults to
 * `T()`
 * @return The value if it is found, otherwise @p default_value
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T, class U = T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 val_iterable_t<Iterable>
find_last_or_default(Iterable&& iterable, T&& to_find, U&& default_value = {}) {
    return detail::find_last_or_default(std::forward<Iterable>(iterable), std::forward<T>(to_find),
                                        std::forward<U>(default_value));
}

/**
 * @brief Finds the last element in the range [begin(iterable), end(iterable)) that satisfies the unary_predicate @p
 * unary_predicate. If the element is found, it returns the value, otherwise it returns @p default_value. Note that when @p
 * iterable has a sentinel, the entire range is searched. Does not use `lz::cached_reverse` to reverse the elements.
 * @param iterable The iterable to search.
 * @param unary_predicate The search unary_predicate in [begin(iterable), end(iterable)). Must return bool.
 * @param default_value The value to return when no element matches unary_predicate @p unary_predicate in [begin(iterable),
 * end(iterable))`. Defaults to `Iterable::value_type()`
 * @return The value if it is found, otherwise @p default_value
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate, class U = val_iterable_t<Iterable>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 val_iterable_t<Iterable>
find_last_or_default_if(Iterable&& iterable, UnaryPredicate&& unary_predicate, U&& default_value = {}) {
    return detail::find_last_or_default_if(std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(unary_predicate),
                                           std::forward<U>(default_value));
}

/**
 * @brief Finds the last element in the range [begin(iterable), end(iterable)) that does not satisfy the value @p value. If
 * the element is found, it returns the value, otherwise it returns @p default_value. Does not use `lz::cached_reverse` to reverse
 * the elements.
 * @param iterable The iterable to search in
 * @param value The value to search for
 * @param default_value The value to return when no element matches @p value in [begin(iterable), end(iterable)). Defaults to `T()
 * @return The value if it is found, otherwise @p default_value
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T, class U = T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 val_iterable_t<Iterable>
find_last_or_default_not(Iterable&& iterable, T&& value, U&& default_value = {}) {
    return detail::find_last_or_default_not(std::forward<Iterable>(iterable), std::forward<T>(value),
                                            std::forward<U>(default_value));
}

/**
 * @brief Finds the last element in the range [begin(iterable), end(iterable)) that does not satisfy the unary_predicate @p
 * unary_predicate. If the element is found, it returns the value, otherwise it returns @p default_value. Note that when @p
 * iterable has a sentinel, the entire range is searched. Does not use `lz::cached_reverse` to reverse the elements.
 * @param iterable The iterable to search in
 * @param unary_predicate The search unary_predicate in [begin(iterable), end(iterable)). Must return bool-like.
 * @param default_value The value to return when no element matches unary_predicate @p unary_predicate in [begin(iterable),
 * end(iterable))`. Defaults to `Iterable::value_type()`
 * @return The value if it is found, otherwise @p default_value
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate, class U = val_iterable_t<Iterable>>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 val_iterable_t<Iterable>
find_last_or_default_if_not(Iterable&& iterable, UnaryPredicate&& unary_predicate, U&& default_value = {}) {
    return detail::find_last_or_default_if_not(std::forward<Iterable>(iterable), std::forward<UnaryPredicate>(unary_predicate),
                                               std::forward<U>(default_value));
}

/**
 * @brief Gets the index of the first occurrence of the value @p value in the range [begin(iterable), end(iterable))
 *
 * @param iterable The iterable to search in
 * @param value The value to search for
 * @return The index of the value in the range [begin(iterable), end(iterable)) or lz::npos if the value is not found
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::size_t index_of(Iterable&& iterable, const T& value) {
    return detail::index_of(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                            value);
}

/**
 * @brief Gets the index of the first element in the range [begin(iterable), end(iterable)) that satisfies the unary_predicate
 * @p unary_predicate
 * @param iterable The iterable to search in
 * @param unary_predicate The unary_predicate to search with
 * @return The index of the first element that satisfies the unary_predicate in the range [begin(iterable), end(iterable)) or
 * lz::npos if the element is not found
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::size_t index_of_if(Iterable&& iterable, UnaryPredicate&& unary_predicate) {
    return detail::index_of_if(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                               std::forward<UnaryPredicate>(unary_predicate));
}

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) contains the value @p value
 *
 * @param iterable The iterable to check
 * @param value The value to check for
 * @return `true` if the value is found, `false` otherwise
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool contains(Iterable&& iterable, const T& value) {
    return detail::contains(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                            value);
}

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) starts with the range [begin(iterable2), end(iterable2))
 *
 * @param iterable The iterable to check
 * @param iterable2 The iterable to check for
 * @param binary_predicate The binary binary_predicate to check the values with
 * @return `true` if the value is found, `false` otherwise
 */
template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE Iterable2, class BinaryPredicate = MAKE_BIN_PRED(equal_to)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool
starts_with(Iterable&& iterable, Iterable2&& iterable2, BinaryPredicate&& binary_predicate = {}) {
    return detail::starts_with(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                               detail::begin(std::forward<Iterable>(iterable2)), detail::end(std::forward<Iterable>(iterable2)),
                               std::forward<BinaryPredicate>(binary_predicate));
}

#ifdef LZ_HAS_CXX_17

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) ends with the bidirectional range [begin(iterable2),
 * end(iterable2)). Needs to know the size of the range if its input iterables aren't bidirectional
 * so it may be worth your while to use `lz::cache_size` if the input iterable @p iterable and @p iterable2 aren't sized and going
 * to use this function multiple times. Does not use `lz::cached_reverse` to reverse the elements.
 *
 * @param iterable The iterable to check
 * @param iterable2 The bidirectional iterable to check for
 * @param binary_predicate The binary binary_predicate to check the values with
 * @return `true` if the value is found, `false` otherwise
 */
template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE Iterable2, class BinaryPredicate = std::equal_to<>>
[[nodiscard]] constexpr bool ends_with(Iterable&& iterable, Iterable2&& iterable2, BinaryPredicate&& binary_predicate = {}) {
    if constexpr (detail::is_bidi<iter_t<Iterable>>::value) {
        return detail::ends_with(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                                 std::begin(iterable2), std::end(iterable2), std::forward<BinaryPredicate>(binary_predicate));
    }
    else {
        return detail::ends_with(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                                 std::begin(iterable2), std::end(iterable2), std::forward<BinaryPredicate>(binary_predicate),
                                 lz::eager_size(iterable), lz::eager_size(iterable2));
    }
}

#else

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) ends with the bidirectional range [begin(iterable2),
 * end(iterable2))
 *
 * @param iterable The iterable to check
 * @param iterable2 The bidirectional iterable to check for
 * @param binary_predicate The binary binary_predicate to check the values with
 * @return `true` if the value is found, `false` otherwise
 */
template<class Iterable, class Iterable2, class BinaryPredicate = MAKE_BIN_PRED(equal_to)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::enable_if<detail::is_bidi<iter_t<Iterable>>::value, bool>
ends_with(Iterable&& iterable, Iterable2&& iterable2, BinaryPredicate&& binary_predicate = {}) {
    return detail::ends_with(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                             std::begin(iterable2), std::end(iterable2), std::forward<BinaryPredicate>(binary_predicate));
}

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) ends with the forward iterable range [begin(iterable2),
 * end(iterable2)). Needs to know the size of the range if its input iterables aren't bidirectional so it may be worth your while
 * to use `lz::cache_size` if the input iterable @p iterable and @p iterable2 aren't sized and going to use this function multiple
 * times.
 *
 * @param iterable The iterable to check
 * @param iterable2 The forward iterable to check for
 * @param binary_predicate The binary binary_predicate to check the values with
 * @return `true` if the value is found, `false` otherwise
 */
template<class Iterable, class Iterable2, class BinaryPredicate = MAKE_BIN_PRED(equal_to)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 detail::enable_if<!detail::is_bidi<iter_t<Iterable>>::value, bool>
ends_with(Iterable&& iterable, Iterable2&& iterable2, BinaryPredicate&& binary_predicate = {}) {
    return detail::ends_with(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                             std::begin(iterable2), std::end(iterable2), std::forward<BinaryPredicate>(binary_predicate),
                             lz::eager_size(iterable), lz::eager_size(iterable2));
}

#endif // LZ_HAS_CXX_17

/**
 * @brief Reorders the elements in the range [begin(iterable), end(iterable)) in such a way that all elements for which the
 * unary_predicate @p unary_predicate returns true precede all elements for which unary_predicate @p unary_predicate returns
 * false. Relative order of the elements is not preserved
 *
 * @param iterable The iterable to partition
 * @param unary_predicate The unary_predicate to partition the elements with
 * @return Iterator to the first element of the second group.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17 iter_t<Iterable> partition(Iterable&& iterable, UnaryPredicate&& unary_predicate) {
    using std::partition;
    return partition(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                     std::forward<UnaryPredicate>(unary_predicate));
}

/**
 * Gets the mean of a sequence.
 * @param iterable The iterable to calculate the mean of.
 * @param binary_op The binary operator to calculate the mean with.
 * @return The mean of the container.
 */
template<LZ_CONCEPT_ITERABLE Iterable, class BinaryOp = MAKE_BIN_PRED(plus)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 double mean(Iterable&& iterable, BinaryOp binary_op = {}) {
    return detail::mean(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                        std::move(binary_op));
}

/**
 * @brief Iterates over the range [begin(iterable), end(iterable)) and applies the unary binary_predicate @p unary_op to
 * each
 *
 * @param iterable The iterable to iterate over
 * @param unary_op The unary binary_predicate to apply to each element
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void for_each(Iterable&& iterable, UnaryOp&& unary_op) {
    using detail::for_each;
    using std::for_each;
    for_each(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             std::forward<UnaryOp>(unary_op));
}

/**
 * @brief Keeps iterating over @p iterable while @p binary_predicate returns true. Essentially the same as lz::filter, however
 * filter is not generally used to alter variables in a range
 *
 * @param iterable A range
 * @param unary_op Predicate that must either return true or false
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void for_each_while(Iterable&& iterable, UnaryOp&& unary_op) {
    detail::for_each_while(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                           std::forward<UnaryOp>(unary_op));
}

/**
 * @brief Keeps iterating over @p iterable while @p binary_predicate returns true and @p n < distance(begin(iterable),
 * end(iterable)).
 *
 * @param iterable A range
 * @param n The amount of elements to iterate over
 * @param unary_op Predicate that must either return true or false
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void for_each_while_n(Iterable&& iterable, std::size_t n, UnaryOp&& unary_op) {
    detail::for_each_while_n(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), n,
                             std::forward<UnaryOp>(unary_op));
}

/**
 * @brief Copies the range [begin(iterable), end(iterable)) to the range [output, output + (end - begin))
 *
 * @param iterable The iterable to copy
 * @param output The output iterator to copy the values to
 */
template<LZ_CONCEPT_ITERABLE Iterable, class OutputIterator>
LZ_CONSTEXPR_CXX_14 void copy(Iterable&& iterable, OutputIterator output) {
    using detail::copy;
    using std::copy;
    copy(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), output);
}

/**
 * @brief Transforms the range [begin(iterable), end(iterable)) to the range [output, output + (end - begin))
 *
 * @param iterable The iterable to transform
 * @param output The output iterator to transform the values to
 * @param unary_op The operation to transform the values with
 */
template<LZ_CONCEPT_ITERABLE Iterable, class OutputIterator, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void transform(Iterable&& iterable, OutputIterator output, UnaryOp&& unary_op) {
    using detail::transform;
    using std::transform;
    transform(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), std::move(output),
              std::forward<UnaryOp>(unary_op));
}

/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @return true if both are equal, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE IterableA, LZ_CONCEPT_ITERABLE IterableB, class BinaryPredicate = MAKE_BIN_PRED(equal_to)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool equal(IterableA&& a, IterableB&& b, BinaryPredicate&& binary_predicate = {}) {
    return detail::equal(std::forward<IterableA>(a), std::forward<IterableB>(b), std::forward<BinaryPredicate>(binary_predicate));
}

/**
 * @brief Searches for the first element in the partitioned range [begin(iterable), end(iterable)) which is not ordered before
 * @p value.
 *
 * @param iterable The iterable to check
 * @param value The value to check for
 * @param binary_predicate The to use when comparing the values
 * @return Iteartor to the first element that satisfies the value or `end(iterable)` if the element is not found
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T, class BinaryPredicate = MAKE_BIN_PRED(less)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17 iter_t<Iterable>
lower_bound(Iterable&& iterable, const T& value, BinaryPredicate&& binary_predicate = {}) {
    return detail::lower_bound(std::forward<Iterable>(iterable), value, std::forward<BinaryPredicate>(binary_predicate));
}

/**
 * @brief Searches for the first element in the partitioned range [begin(iterable), end(iterable)) which is not ordered before
 * @p value.
 *
 * @param iterable The iterable to check
 * @param value The value to check for
 * @param binary_predicate The to use when comparing the values
 * @return Iteartor to the first element that satisfies the value or `end(iterable)` if the element is not found
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T, class BinaryPredicate = MAKE_BIN_PRED(less)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17 iter_t<Iterable>
upper_bound(Iterable&& iterable, const T& value, BinaryPredicate&& binary_predicate = {}) {
    return detail::upper_bound(std::forward<Iterable>(iterable), value, std::forward<BinaryPredicate>(binary_predicate));
}

/**
 * @brief Searches for the first occurrence of the value @p value in the range [begin(iterable), end(iterable)) using the
 * binary search algorithm. @p iterable must be sorted beforehand.
 * @param iterable The iterable to search in
 * @param value The value to search for
 * @param binary_predicate Predicate to search the value with
 * @return true if the value is found, false otherwise
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T, class BinaryPredicate = MAKE_BIN_PRED(less)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17 bool
binary_search(Iterable&& iterable, const T& value, BinaryPredicate&& binary_predicate = {}) {
    return detail::binary_search(std::forward<Iterable>(iterable), value, std::forward<BinaryPredicate>(binary_predicate));
}

/**
 * @brief Checks if all elements in the range [begin(iterable), end(iterable)) satisfy the unary_predicate @p unary_predicate
 *
 * @param iterable The iterable to check
 * @param unary_predicate The unary_predicate to call whether all elements satisfy this condition
 * @return true if all elements satisfy the unary_predicate, false otherwise
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool all_of(Iterable&& iterable, UnaryPredicate&& unary_predicate) {
    using detail::all_of;
    using std::all_of;
    return all_of(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                  std::forward<UnaryPredicate>(unary_predicate));
}

/**
 * @brief Checks if any element in the range [begin(iterable), end(iterable)) satisfies the unary_predicate @p unary_predicate
 *
 * @param iterable The iterable to check
 * @param unary_predicate The unary_predicate to call whether any element satisfies this condition
 * @return true if any element satisfies the unary_predicate, false otherwise
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool any_of(Iterable&& iterable, UnaryPredicate&& unary_predicate) {
    using detail::any_of;
    using std::any_of;
    return any_of(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                  std::forward<UnaryPredicate>(unary_predicate));
}

/**
 * @brief Checks if none of the elements in the range [begin(iterable), end(iterable)) satisfy the unary_predicate @p
 * unary_predicate
 *
 * @param iterable The iterable to check
 * @param unary_predicate The unary_predicate to call whether none of the elements satisfy this condition
 * @return true if none of the elements satisfy the unary_predicate, false otherwise
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool none_of(Iterable&& iterable, UnaryPredicate&& unary_predicate) {
    using detail::none_of;
    using std::none_of;
    return none_of(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                   std::forward<UnaryPredicate>(unary_predicate));
}

/**
 * @brief Finds the first occurrence of two adjacent elements in the range [begin(iterable), end(iterable)) that satisfy the
 * binary binary_predicate @p binary_predicate
 *
 * @param iterable The iterable to search in
 * @param binary_predicate The binary binary_predicate to search with
 * @return Iterator to the first occurrence of two adjacent elements that satisfy the binary binary_predicate or
 * `end(iterable)` if the elements are not found
 */
template<LZ_CONCEPT_ITERABLE Iterable, class BinaryPredicate = MAKE_BIN_PRED(equal_to)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> adjacent_find(Iterable&& iterable, BinaryPredicate&& binary_predicate = {}) {
    using detail::adjacent_find;
    using std::adjacent_find;
    return adjacent_find(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                         std::forward<BinaryPredicate>(binary_predicate));
}

/**
 * @brief Counts the amount of elements in the range [begin(iterable), end(iterable)) that satisfy the unary_predicate @p
 * unary_predicate
 *
 * @param iterable The iterable to count
 * @param unary_predicate The unary_predicate to count the elements with
 * @return The amount of elements that satisfy the unary_predicate
 */
template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 diff_iterable_t<Iterable> count_if(Iterable&& iterable, UnaryPredicate&& unary_predicate) {
    using detail::count_if;
    using std::count_if;
    return count_if(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                    std::forward<UnaryPredicate>(unary_predicate));
}

/**
 * @brief Counts the amount of elements in the range [begin(iterable), end(iterable)) that are equal to the value @p value
 *
 * @param iterable The iterable to count in
 * @param value The value to count
 * @return The amount of elements that are equal to @p value
 */
template<LZ_CONCEPT_ITERABLE Iterable, class T>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 diff_iterable_t<Iterable> count(Iterable&& iterable, const T& value) {
    using detail::count;
    using std::count;
    return count(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)), value);
}

/**
 * @brief Checks if the range [begin(iterable), end(iterable)) is sorted using the binary binary_predicate @p binary_predicate
 *
 * @param iterable The iterable to check
 * @param binary_predicate The binary binary_predicate to check the range with
 * @return true if the range is sorted, false otherwise
 */
template<LZ_CONCEPT_ITERABLE Iterable, class BinaryPredicate = MAKE_BIN_PRED(less)>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool is_sorted(Iterable&& iterable, BinaryPredicate&& binary_predicate = {}) {
    using detail::is_sorted;
    using std::is_sorted;
    return is_sorted(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                     std::forward<BinaryPredicate>(binary_predicate));
}

} // namespace lz

#endif // LZ_ALGORITHM_HPP