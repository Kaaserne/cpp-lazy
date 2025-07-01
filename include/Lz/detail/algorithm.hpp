#pragma once

#ifndef LZ_DETAIL_ALGORITHM_HPP
#define LZ_DETAIL_ALGORITHM_HPP

#include <Lz/detail/procs.hpp>
#include <Lz/detail/traits.hpp>

#if defined(LZ_HAS_CXX_17)
#include <functional> // std::not_fn
#endif

#include <algorithm>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S>
constexpr diff_type<Iterator> distance(Iterator begin, S end);

/**
 * This value is returned when index_of(_if) does not find the value specified.
 */
constexpr static std::size_t npos = static_cast<std::size_t>(-1);

LZ_MODULE_EXPORT_SCOPE_END

namespace detail {
template<class Iterator, class S>
constexpr bool empty(Iterator begin, S end) {
    return begin == end;
}

template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14 bool has_one(Iterator begin, S end) {
    if (detail::empty(begin, end)) {
        return false;
    }
    ++begin;
    return begin == end;
}

template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14 bool has_many(Iterator begin, S end) {
    return !detail::empty(begin, end) && !detail::has_one(begin, end);
}

template<class Iterator, class S>
LZ_CONSTEXPR_CXX_14 Iterator unwrap_reverse_iterator(std::reverse_iterator<Iterator> it, Iterator begin, S end) {
    auto base = it.base();
    if (base == begin) {
        return end;
    }
    return --base;
}

template<class Iterator, class S, class T, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 T accumulate(Iterator begin, S end, T init, BinaryPredicate binary_op) {
    for (; begin != end; ++begin) {
        init = binary_op(std::move(init), *begin);
    }
    return init;
}

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator max_element(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if (begin == end) {
        return begin;
    }
    auto max = begin;
    for (++begin; begin != end; ++begin) {
        if (binary_predicate(*max, *begin)) {
            max = begin;
        }
    }
    return max;
}

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator min_element(Iterator begin, S end, BinaryPredicate binary_predicate) {
    using detail::max_element;
    using std::max_element;
#if defined(__cpp_lib_not_fn) && defined(LZ_HAS_CXX_17)

    return max_element(std::move(begin), std::move(end), std::not_fn(std::move(binary_predicate)));

#else

    using rt = ref_t<Iterator>;
    return max_element(std::move(begin), std::move(end), [&binary_predicate](rt a, rt b) { return !binary_predicate(a, b); });

#endif
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator find_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    for (; begin != end; ++begin) {
        if (unary_predicate(*begin)) {
            break;
        }
    }
    return begin;
}

template<class Iterator, class S, class T>
LZ_CONSTEXPR_CXX_14 Iterator find(Iterator begin, S end, const T& value) {
    using detail::find_if;
    using std::find_if;
    return find_if(std::move(begin), std::move(end), [&value](ref_t<Iterator> val) { return val == value; });
}

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class T>
constexpr Iterator find_last(Iterator begin, S end, const T& value) {
    if constexpr (std::is_same_v<Iterator, S>) {
        std::reverse_iterator<Iterator> rbegin(end);
        std::reverse_iterator<S> rend(begin);
        auto pos = std::find(std::move(rbegin), std::move(rend), value);
        return unwrap_reverse_iterator(std::move(pos), begin, end);
    }
    else {
        return find_last_if(std::move(begin), std::move(end), [&value](ref_t<Iterator> val) { return val == value; });
    }
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_17 Iterator find_last_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    if constexpr (std::is_same_v<Iterator, S>) {
        std::reverse_iterator<S> rbegin(end);
        std::reverse_iterator<Iterator> rend(begin);
        auto pos = std::find_if(std::move(rbegin), std::move(rend), std::move(unary_predicate));
        return unwrap_reverse_iterator(std::move(pos), begin, end);
    }
    else {
        auto current = begin;
        auto it = begin;
        for (; it != end; ++it) {
            if (unary_predicate(*it)) {
                current = it;
            }
        }
        // Check if current was never updated
        if (it == end && current == begin && !unary_predicate(*current)) {
            return it;
        }
        return current;
    }
}

template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate>
constexpr std::pair<Iterator, Iterator>
search(Iterator begin, S end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    if constexpr (!std::is_same_v<Iterator, S> || !std::is_same_v<Iterator2, S2>) {
        while (begin != end) {
            auto it = begin;
            auto it2 = begin2;
            while (it2 != end2 && it != end && binary_predicate(*it, *it2)) {
                ++it;
                ++it2;
            }
            if (it2 == end2) {
                return { begin, it };
            }
            ++begin;
        }
        return { begin, begin };
    }
    else {
        auto pos = std::search(std::move(begin), std::move(end), begin2, end2, std::move(binary_predicate));
        return pos == end ? std::make_pair(pos, pos) : std::make_pair(pos, std::next(pos, std::distance(begin2, end2)));
    }
}

#else

template<class Iterator, class S, class T>
LZ_CONSTEXPR_CXX_17 enable_if<std::is_same<Iterator, S>::value, Iterator> find_last(Iterator begin, S end, const T& value) {
    std::reverse_iterator<Iterator> rbegin(end);
    std::reverse_iterator<S> rend(begin);
    auto pos = std::find(std::move(rbegin), std::move(rend), value);
    return unwrap_reverse_iterator(std::move(pos), begin, end);
}

template<class Iterator, class S, class T>
LZ_CONSTEXPR_CXX_17 enable_if<!std::is_same<Iterator, S>::value, Iterator> find_last(Iterator begin, S end, const T& value) {
    return find_last_if(std::move(begin), std::move(end), [&value](ref_t<Iterator> val) { return val == value; });
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_17 enable_if<std::is_same<Iterator, S>::value, Iterator>
find_last_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    std::reverse_iterator<S> rbegin(end);
    std::reverse_iterator<Iterator> rend(begin);
    auto pos = std::find_if(std::move(rbegin), std::move(rend), std::move(unary_predicate));
    return unwrap_reverse_iterator(std::move(pos), begin, end);
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if<!std::is_same<Iterator, S>::value, Iterator>
find_last_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    auto current = begin;
    auto it = begin;
    for (; it != end; ++it) {
        if (unary_predicate(*it)) {
            current = it;
        }
    }
    // Check if current was never updated
    if (it == end && current == begin && !unary_predicate(*current)) {
        return it;
    }
    return current;
}

// clang-format off

template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14
enable_if<!std::is_same<Iterator, S>::value || !std::is_same<Iterator2, S2>::value, std::pair<Iterator, Iterator>>
search(Iterator begin, S end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    while (begin != end) {
        auto it = begin;
        auto it2 = begin2;
        while (it2 != end2 && it != end && binary_predicate(*it, *it2)) {
            ++it;
            ++it2;
        }
        if (it2 == end2) {
            return { begin, it };
        }
        ++begin;
    }
    return { begin, begin };
}

template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14
enable_if<std::is_same<Iterator, S>::value && std::is_same<Iterator2, S2>::value, std::pair<Iterator, Iterator>>
search(Iterator begin, S end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    auto pos = std::search(std::move(begin), std::move(end), begin2, end2, std::move(binary_predicate));
    return pos == end ? std::make_pair(pos, pos) : std::make_pair(pos, std::next(pos, std::distance(begin2, end2)));
}

// clang-format on

#endif

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator find_if_not(Iterator begin, S end, UnaryPredicate unary_predicate) {
    using detail::find_if;
    using std::find_if;
#if defined(__cpp_lib_not_fn) && defined(LZ_HAS_CXX_17)

    return find_if(std::move(begin), std::move(end), std::not_fn(std::move(unary_predicate)));
#else

    return find_if(std::move(begin), std::move(end),
                   [&unary_predicate](ref_t<Iterator> value) { return !unary_predicate(value); });
#endif
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator find_last_if_not(Iterator begin, S end, UnaryPredicate unary_predicate) {
#if defined(__cpp_lib_not_fn) && defined(LZ_HAS_CXX_17)

    return find_last_if(std::move(begin), std::move(end), std::not_fn(std::move(unary_predicate)));
#else

    return find_last_if(std::move(begin), std::move(end),
                        [&unary_predicate](ref_t<Iterator> value) { return !unary_predicate(value); });

#endif
}

template<class Iterator, class S, class T, class U>
LZ_CONSTEXPR_CXX_14 val_t<Iterator> find_or_default(Iterator begin, S end, T&& to_find, U&& default_value) {
    using detail::find;
    using std::find;
    return static_cast<val_t<Iterator>>(find(std::move(begin), end, to_find) == end ? std::forward<U>(default_value)
                                                                                    : std::forward<T>(to_find));
}

template<class Iterator, class S, class T, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 val_t<Iterator> find_or_default_if(Iterator begin, S end, UnaryPredicate unary_predicate, T&& default_value) {
    using detail::find_if;
    using std::find_if;
    auto pos = find_if(std::move(begin), end, std::move(unary_predicate));
    return static_cast<val_t<Iterator>>(pos == end ? std::forward<T>(default_value) : *pos);
}

template<class Iterator, class S, class T, class U>
LZ_CONSTEXPR_CXX_14 val_t<Iterator> find_last_or_default(Iterator begin, S end, T&& to_find, U&& default_value) {
    return static_cast<val_t<Iterator>>(find_last(std::move(begin), end, to_find) == end ? std::forward<U>(default_value)
                                                                                         : std::forward<T>(to_find));
}

template<class Iterator, class S, class UnaryPredicate, class U>
LZ_CONSTEXPR_CXX_14 val_t<Iterator>
find_last_or_default_if(Iterator begin, S end, UnaryPredicate unary_predicate, U&& default_value) {
    auto pos = find_last_if(std::move(begin), end, std::move(unary_predicate));
    return static_cast<val_t<Iterator>>(pos == end ? std::forward<U>(default_value) : *pos);
}

template<class Iterator, class S, class T, class U>
LZ_CONSTEXPR_CXX_14 val_t<Iterator> find_last_or_default_not(Iterator begin, S end, T&& value, U&& default_value) {
    return find_last_or_default_if(
        std::move(begin), std::move(end), [&value](ref_t<Iterator> val) { return !(val == value); },
        std::forward<U>(default_value));
}

template<class Iterator, class S, class UnaryPredicate, class U>
LZ_CONSTEXPR_CXX_14 val_t<Iterator>
find_last_or_default_if_not(Iterator begin, S end, UnaryPredicate unary_predicate, U&& default_value) {
#ifdef LZ_HAS_CXX_17

    return find_last_or_default_if(std::move(begin), std::move(end), std::not_fn(std::move(unary_predicate)),
                                   std::forward<U>(default_value));
#else

    return find_last_or_default_if(
        std::move(begin), std::move(end), [&unary_predicate](ref_t<Iterator> val) { return !unary_predicate(val); },
        std::forward<U>(default_value));

#endif
}

template<class Iterator, class S, class T>
LZ_CONSTEXPR_CXX_14 std::size_t index_of(Iterator begin, S end, const T& value) {
    return index_of_if(std::move(begin), std::move(end), [&value](ref_t<Iterator> val) { return val == value; });
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 std::size_t index_of_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    std::size_t index = 0;
    for (; begin != end; ++begin, ++index) {
        if (unary_predicate(*begin)) {
            return index;
        }
    }
    return begin == end ? npos : index;
}

template<class Iterator, class S, class T>
LZ_CONSTEXPR_CXX_14 bool contains(Iterator begin, S end, const T& value) {
    using detail::find;
    using std::find;
    return find(std::move(begin), std::move(end), value) != end;
}

template<class Iterator, class S, class Iter2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 bool equal(Iterator begin, S end, Iter2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    for (; begin != end && begin2 != end2; ++begin, ++begin2) {
        if (!binary_predicate(*begin, *begin2)) {
            return false;
        }
    }
    return begin == end && begin2 == end2;
}

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 bool starts_with(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    const bool is_end = begin == end;
    const bool is_end2 = begin2 == end2;

    if (is_end && is_end2) {
        return true;
    }
    if (is_end || is_end2) {
        return false;
    }

    while (begin != end && begin2 != end2 && binary_predicate(*begin, *begin2)) {
        if (begin == end) {
            return false;
        }
        ++begin;
        ++begin2;
    }
    return begin2 == end2;
}

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_17 bool ends_with(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    std::reverse_iterator<Iterator1> rbegin(std::move(end));
    std::reverse_iterator<S1> rend(std::move(begin));
    return starts_with(std::move(rbegin), std::move(rend), std::reverse_iterator<Iterator2>(std::move(end2)),
                       std::reverse_iterator<S2>(std::move(begin2)), std::move(binary_predicate));
}

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 bool ends_with(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate,
                                   std::size_t size_of_iterable1, std::size_t size_of_iterable2) {
    if (size_of_iterable1 < size_of_iterable2) {
        return false;
    }

    const auto distance = static_cast<diff_type<Iterator1>>(size_of_iterable1 - size_of_iterable2);
    begin = std::next(begin, distance);
    using detail::equal;
    using std::equal;
    return equal(std::move(begin), std::move(end), std::move(begin2), std::move(end2), std::move(binary_predicate));
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator partition(Iterator begin, S end, UnaryPredicate unary_predicate) {
    using detail::find_if_not;
    using std::find_if_not;
    begin = find_if_not(begin, end, unary_predicate);
    if (begin == end) {
        return begin;
    }

    for (auto i = std::next(begin); i != end; ++i) {
        if (!unary_predicate(*i)) {
            continue;
        }
        std::swap(*i, *begin);
        ++begin;
    }

    return begin;
}

template<class Iterator, class S, class BinaryOp>
LZ_CONSTEXPR_CXX_14 double mean(Iterator begin, S end, BinaryOp binary_op) {
    using T = decltype(binary_op(*begin, *begin));
    diff_type<Iterator> distance{ 0 };
    T sum{};

    for (; begin != end; ++begin, ++distance) {
        sum = binary_op(std::move(sum), *begin);
    }
    if (distance == 0) {
        return 0;
    }
    return static_cast<double>(sum) / static_cast<double>(distance);
}

template<class Iterator, class S, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void for_each(Iterator begin, S end, UnaryOp unary_op) {
    for (; begin != end; ++begin) {
        unary_op(*begin);
    }
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 void for_each_while(Iterator begin, S end, UnaryPredicate unary_predicate) {
    for (; begin != end; ++begin) {
        if (!unary_predicate(*begin)) {
            break;
        }
    }
}

template<class Iterator, class S, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void for_each_while_n(Iterator begin, S end, std::size_t n, UnaryOp unary_op) {
    for (std::size_t count = 0; begin != end && count < n; ++begin, ++count) {
        if (!unary_op(*begin)) {
            break;
        }
    }
}

template<class SourceIterator, class SourceS, class OutputIterator>
LZ_CONSTEXPR_CXX_14 void copy(SourceIterator begin, SourceS end, OutputIterator output) {
    for (; begin != end; ++begin, ++output) {
        *output = *begin;
    }
}

template<class SourceIterator, class SourceSentinel, class OutputIterator, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void transform(SourceIterator begin, SourceSentinel end, OutputIterator output, UnaryOp op) {
    for (; begin != end; ++begin, ++output) {
        *output = op(*begin);
    }
}

#ifdef LZ_HAS_CXX_17

template<class IterableA, class IterableB, class BinaryPredicate>
constexpr bool equal(IterableA&& a, IterableB&& b, BinaryPredicate&& binary_predicate) {
    if constexpr (sized<IterableA>::value && sized<IterableB>::value) {
        if (lz::size(a) != lz::size(b)) {
            return false;
        }
    }

    using detail::equal;
    using std::equal;

    return equal(detail::begin(std::forward<IterableA>(a)), detail::end(std::forward<IterableA>(a)),
                 detail::begin(std::forward<IterableB>(b)), detail::end(std::forward<IterableB>(b)),
                 std::forward<BinaryPredicate>(binary_predicate));
}

#else

template<class IterableA, class IterableB, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if<sized<IterableA>::value && sized<IterableB>::value, bool>
equal(IterableA&& a, IterableB&& b, BinaryPredicate&& binary_predicate) {
    using detail::equal;
    using std::equal;

    if (lz::size(a) != lz::size(b)) {
        return false;
    }

    return equal(detail::begin(std::forward<IterableA>(a)), detail::end(std::forward<IterableA>(a)),
                 detail::begin(std::forward<IterableB>(b)), detail::end(std::forward<IterableB>(b)),
                 std::forward<BinaryPredicate>(binary_predicate));
}

template<class IterableA, class IterableB, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if<!sized<IterableA>::value || !sized<IterableB>::value, bool>
equal(IterableA&& a, IterableB&& b, BinaryPredicate&& binary_predicate) {
    using detail::equal;
    using std::equal;

    return equal(detail::begin(std::forward<IterableA>(a)), detail::end(std::forward<IterableA>(a)),
                 detail::begin(std::forward<IterableB>(b)), detail::end(std::forward<IterableB>(b)),
                 std::forward<BinaryPredicate>(binary_predicate));
}

#endif

template<class Iterator, class T, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator sized_lower_bound(Iterator begin, const T& value, BinaryPredicate binary_predicate,
                                               diff_type<Iterator> count) {
    while (count > 0) {
        const auto step = count / 2;
        auto it = std::next(begin, step);
        if (binary_predicate(*it, value)) {
            begin = ++it;
            count -= step + 1;
        }
        else {
            count = step;
        }
    }
    return begin;
}

template<class Iterable, class T, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 iter_t<Iterable> lower_bound(Iterable&& iterable, const T& value, BinaryPredicate binary_predicate) {
    using diff = diff_type<iter_t<Iterable>>;

    auto begin = detail::begin(std::forward<Iterable>(iterable));
    return sized_lower_bound(std::move(begin), value, std::move(binary_predicate), static_cast<diff>(lz::eager_size(iterable)));
}

template<class Iterable, class T, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 iter_t<Iterable> upper_bound(Iterable&& iterable, const T& value, BinaryPredicate binary_predicate) {
    return detail::lower_bound(
        std::forward<Iterable>(iterable), value,
        [&binary_predicate](ref_iterable_t<Iterable> v1, const T& v2) { return !binary_predicate(v2, v1); });
}

template<class Iterable, class T, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 bool binary_search(Iterable&& iterable, const T& value, BinaryPredicate binary_predicate) {
    auto end = detail::end(std::forward<Iterable>(iterable));
    auto it = detail::lower_bound(std::forward<Iterable>(iterable), value, binary_predicate);
    return it != end && !binary_predicate(value, *it);
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 bool all_of(Iterator iterator, S s, UnaryPredicate unary_predicate) {
    using detail::find_if_not;
    using std::find_if_not;
    return find_if_not(std::move(iterator), s, std::move(unary_predicate)) == s;
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 bool any_of(Iterator iterator, S s, UnaryPredicate unary_predicate) {
    using detail::find_if;
    using std::find_if;
    return find_if(std::move(iterator), s, std::move(unary_predicate)) != s;
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 bool none_of(Iterator iterator, S s, UnaryPredicate unary_predicate) {
    using detail::any_of;
    using std::any_of;
    return !any_of(std::move(iterator), std::move(s), std::move(unary_predicate));
}

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator adjacent_find(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if (begin == end) {
        return begin;
    }
    auto next = begin;
    for (++next; next != end; ++begin, ++next) {
        if (binary_predicate(*begin, *next)) {
            return begin;
        }
    }
    return next;
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 diff_type<Iterator> count_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    diff_type<Iterator> count = 0;
    for (; begin != end; ++begin) {
        if (unary_predicate(*begin)) {
            ++count;
        }
    }
    return count;
}

template<class Iterator, class S, class T>
LZ_CONSTEXPR_CXX_14 diff_type<Iterator> count(Iterator begin, S end, const T& value) {
    using detail::count_if;
    using std::count_if;
    return count_if(std::move(begin), end, [&value](const T& val) { return val == value; });
}

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 bool is_sorted(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if (begin == end) {
        return true;
    }
    auto next = begin;
    for (++next; next != end; ++begin, ++next) {
        if (binary_predicate(*next, *begin)) {
            return false;
        }
    }
    return true;
}

} // namespace detail
} // namespace lz

#endif
