#pragma once

#ifndef LZ_BASIC_ITERATOR_VIEW_HPP
#    define LZ_BASIC_ITERATOR_VIEW_HPP

#    include <algorithm>
#    include <array>
#    include <map>
#    include <numeric>
#    include <string>
#    include <unordered_map>
#    include <vector>

#    if defined(LZ_STANDALONE)
#        ifdef LZ_HAS_FORMAT
#            include <format>
#        else
#            ifdef __cpp_lib_to_chars
#                include <charconv>
#            endif // __cpp_lib_to_chars
#            include <sstream>
#        endif // LZ_HAS_FORMAT
#    else
#        include <fmt/format.h>
#    endif // LZ_STANDALONE

#    include "LzTools.hpp"

namespace lz {
namespace internal {
#    ifdef __cpp_if_constexpr
template<class Iterator>
LZ_CONSTEXPR_CXX_20 RefType<Iterator> backImpl(Iterator, Iterator end) {
    if constexpr (IsBidirectional<Iterator>::value) {
        return *--end;
    }
    else {
        static_assert(internal::AlwaysFalse<Iterator>::value, "Iterator must at least be bidirectional to get last element");
    }
}

template<class Iterable>
LZ_CONSTEXPR_CXX_20 RefType<IterTypeFromIterable<Iterable>> backImpl(Iterable&& iterable) {
    using Iter = IterTypeFromIterable<Iterable>;
    return backImpl(std::begin(iterable), std::end(iterable));
}
#    else

template<class Iterator>
LZ_CONSTEXPR_CXX_20 RefType<Iterator> backImpl(Iterator /* begin */, Iterator end) {
    static_assert(internal::IsBidirectional<Iterator>::value, "Iterator must at least be bidirectional to get last element");
    return *--end;
}
#    endif // __cpp_if_constexpr
} // namespace internal

/**
 * Checks whether [begin, end) is empty.
 * @param begin The beginning of the sequence.
 * @param end The end of the sequence.
 * @return True if it is empty, false otherwise.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool empty(const Iterator begin, const Iterator end) {
    return begin == end;
}

/**
 * Checks whether [begin, end) is empty.
 * @param begin The beginning of the sequence.
 * @param end The end of the sequence.
 * @return True if it is empty, false otherwise.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool empty(const Iterable& iterable) {
    return lz::empty(std::begin(iterable), std::end(iterable));
}

/**
 * Returns the first element. Asserts if the sequence is empty.
 * @param begin The beginning of the sequence.
 * @param end The end of the sequence.
 * @return The first element of the sequence (by reference).
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::RefType<Iterator> front(Iterator begin, Iterator end) {
    LZ_ASSERT(!lz::empty(begin, end), "sequence cannot be empty in order to get the first element");
    static_cast<void>(end);
    return *begin;
}

/**
 * Returns the first element. Asserts if the sequence is empty.
 * @param iterable The sequence to get the first element of.
 * @return The first element of the sequence (by reference).
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::RefType<internal::IterTypeFromIterable<Iterable>> front(Iterable&& iterable) {
    LZ_ASSERT(!lz::empty(iterable), "sequence cannot be empty in order to get the last element");
    return front(std::begin(iterable), std::end(iterable));
}

/**
 * Returns the last element. Asserts if the sequence is empty.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @warning Static sserts if the iterator is not (at least) bidirectional
 * @return The last element of the sequence (by reference).
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::RefType<Iterator> back(Iterator begin, Iterator end) {
    LZ_ASSERT(!lz::empty(begin, end), "sequence cannot be empty in order to get the last element");
    return internal::backImpl(begin, end);
}

/**
 * Returns the last element. Asserts if the sequence is empty.
 * @warning Static sserts if the iterator is not (at least) bidirectional
 * @param iterable The sequence to get the last element of.
 * @return The last element of the sequence (by reference).
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::RefType<internal::IterTypeFromIterable<Iterable>> back(Iterable&& iterable) {
    LZ_ASSERT(!lz::empty(iterable), "sequence cannot be empty in order to get the last element");
    return lz::back(std::begin(iterable), std::end(iterable));
}

namespace internal {
#    if defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)
#        ifdef __cpp_if_constexpr
template<class T>
std::string makeString(const T value) {
#            ifdef __cpp_lib_to_chars
    char buff[std::numeric_limits<T>::digits10 + 1]{};
    std::to_chars(std::begin(buff), std::end(buff), value);
    return std::string(buff);
#            else
    using lz::internal::to_string;
    using std::to_string;
    return to_string(value);
#            endif // __cpp_lib_to_chars
}
#        else
template<class T>
std::string makeString(const T value) {
    using lz::internal::to_string;
    using std::to_string;
    return to_string(value);
}
#        endif // __cpp_if_constexpr

template<class Iterator>
EnableIf<std::is_arithmetic<ValueType<Iterator>>::value>
toStringImplSpecialized(std::string& result, Iterator begin, Iterator end, const StringView& delimiter) {
    std::for_each(begin, end, [&delimiter, &result](const ValueType<Iterator>& vt) {
        result += makeString(vt);
        result += delimiter;
    });
}

template<class Iterator>
EnableIf<!std::is_arithmetic<ValueType<Iterator>>::value>
toStringImplSpecialized(std::string& result, Iterator begin, Iterator end, const StringView& delimiter) {
    std::ostringstream oss;
    std::for_each(begin, end, [&oss, &delimiter](const ValueType<Iterator>& t) { oss << t << delimiter; });
    result = oss.str();
}
#    endif // defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)

template<class Iterator>
LZ_CONSTEXPR_CXX_20 void
#    if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
toStringImpl(std::string& result, const Iterator& begin, const Iterator& end, const StringView delimiter, const StringView fmt) {
#    else
toStringImpl(std::string& result, const Iterator& begin, const Iterator& end, const StringView& delimiter) {
#    endif // LZ_HAS_FORMAT
    if (begin == end) {
        return;
    }
#    if !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)
    auto backInserter = std::back_inserter(result);
    // Dynamic format merge with the `fmt` and the "{}" for the delimiter
    std::string format;
    format.reserve(3 + fmt.size());
    format.append(fmt.data(), fmt.size());
    format += "{}";
#    endif // !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)
#    if !defined(LZ_STANDALONE)
    std::for_each(begin, end, [&delimiter, backInserter, &format](const ValueType<Iterator>& v) {
        fmt::format_to(backInserter, format.c_str(), v, delimiter);
    });
#    elif defined(LZ_HAS_FORMAT)
    std::for_each(begin, end, [&delimiter, backInserter, &format](const ValueType<Iterator>& v) {
        std::format_to(backInserter, format.c_str(), v, delimiter);
    });
    // clang-format off
#else
    toStringImplSpecialized(result, begin, end, delimiter);
#endif // LZ_STANDALONE
    // clang-format on
    const auto resultEnd = result.end();
    result.erase(resultEnd - static_cast<std::ptrdiff_t>(delimiter.size()), resultEnd);
}

template<class Iterator>
LZ_CONSTEXPR_CXX_20 internal::EnableIf<std::is_same<char, ValueType<Iterator>>::value, std::string>
#    if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
doMakeString(const Iterator& b, const Iterator& e, const StringView delimiter, const StringView fmt) {
#    else
doMakeString(const Iterator& b, const Iterator& e, const StringView& delimiter) {
#    endif // LZ_HAS_FORMAT
    if (delimiter.size() == 0) {
        return std::string(b, e);
    }
    std::string result;

    const auto len = static_cast<std::size_t>(sizeHint(b, e));
    if (len != 0) {
        result.reserve(len + delimiter.size() * len + 1);
    }

#    if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
    toStringImpl(result, b, e, delimiter, fmt);
#    else
    toStringImpl(result, b, e, delimiter);
#    endif
    return result;
}

template<class Iterator>
LZ_CONSTEXPR_CXX_20 internal::EnableIf<!std::is_same<char, ValueType<Iterator>>::value, std::string>
#    if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
doMakeString(const Iterator& b, const Iterator& e, const StringView delimiter, const StringView fmt) {
#    else
doMakeString(const Iterator& b, const Iterator& e, const StringView& delimiter) {
#    endif // LZ_HAS_FORMAT
    std::string result;
#    if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
    toStringImpl(result, b, e, delimiter, fmt);
#    else
    toStringImpl(result, b, e, delimiter);
#    endif
    return result;
}

template<class T, class = int>
struct HasResize : std::false_type {};

template<class T>
struct HasResize<T, decltype((void)std::declval<T&>().resize(1), 0)> : std::true_type {};

template<class T, class = int>
struct HasReserve : std::false_type {};

template<class T>
struct HasReserve<T, decltype((void)std::declval<T&>().reserve(1), 0)> : std::true_type {};

template<class It>
class BasicIteratorView {
protected:
    It _begin{};
    It _end{};

public:
    using value_type = ValueType<It>;
    using iterator = It;
    using reference = decltype(*_begin);
    using const_reference = typename std::add_const<reference>::type;
    using const_iterator = iterator;

private:
    template<class KeySelectorFunc>
    using KeyType = FunctionReturnType<KeySelectorFunc, RefType<It>>;

#    ifndef __cpp_if_constexpr
    template<class Container>
    EnableIf<!HasReserve<Container>::value, void> tryReserve(Container&) const {
    }

    template<class Container>
    EnableIf<HasReserve<Container>::value, void> tryReserve(Container& container) const {
        container.reserve(sizeHint(_begin, _end));
    }
#    else
    template<class Container>
    LZ_CONSTEXPR_CXX_20 void tryReserve(Container& container) const {
        if constexpr (HasReserve<Container>::value) {
            container.reserve(sizeHint(_begin, _end));
        }
    }
#    endif // __cpp_if_constexpr
    template<class MapType, class KeySelectorFunc>
    LZ_CONSTEXPR_CXX_20 void createMap(MapType& map, const KeySelectorFunc keyGen) const {
        transformTo(std::inserter(map, map.end()),
                    [keyGen](internal::RefType<It> value) { return std::make_pair(keyGen(value), value); });
    }

public:
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 It begin() LZ_CONST_REF_QUALIFIER noexcept {
        return _begin;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 It end() LZ_CONST_REF_QUALIFIER noexcept {
        return _end;
    }

#    ifdef LZ_HAS_REF_QUALIFIER
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 It begin() && noexcept {
        return std::move(_begin);
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 It end() && noexcept {
        return std::move(_end);
    }
#    endif // LZ_HAS_REF_QUALIFIER

    constexpr BasicIteratorView() = default;

    constexpr BasicIteratorView(It&& begin, It&& end) noexcept : _begin(std::move(begin)), _end(std::move(end)) {
    }

    constexpr BasicIteratorView(const It& begin, const It& end) noexcept : _begin(begin), _end(end) {
    }

#    ifdef LZ_HAS_EXECUTION
    /**
     * @brief Returns an arbitrary container type, of which its constructor signature looks like:
     * `Container(Iterator, Iterator[, args...])`. The args may be left empty. The type of the vector is equal to
     * the typedef `value_type`.
     * @attention This function only works properly if the template types matches the container. Example:
     * `to<std::vector, std::allocator<int>>() // fails`
     * `to<std::vector, std::allocator<int>>(std::execution::seq, std::allocator<int>{}); // ok`
     * `to<std::vector>(std::execution::seq, std::allocator<int>{}); // also ok, able to deduce type`
     * `to<std::vector, std::allocator<int>>(std::execution::seq, {}); // also ok`
     * @details Use this function to convert the iterator to a container. Example:
     * ```cpp
     * auto list = lazyIterator.to<std::list>();
     * auto allocator = std::allocator<int>();
     * auto set = lazyIterator.to<std::set>(allocator);
     * ```
     * @param execution The execution policy. Must be one of `std::execution`'s tags.
     * @tparam Args Additional arguments, automatically deduced.
     * @param args Additional arguments, for e.g. an allocator.
     * @return An arbitrary container specified by the entered template parameter.
     */
    template<template<class, class...> class Container, class... Args, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Container<value_type, Decay<Args>...>
    to(Execution execution = std::execution::seq, Args&&... args) const {
        using Cont = Container<value_type, Decay<Args>...>;
        return to<Cont>(execution, std::forward<Args>(args)...);
    }

    /**
     * @brief This function can be used to create a new container from the current view. The template parameter `Container`
     * must be specified along with its value type: `view.to<std::vector<int>>()`. One could also use `view.to<std::vector>()`.
     * See the other `to` function overload for documentation.
     * @example `lzView.to<std::vector<int>>(std::execution::seq, 100); // This will create a vec of size 100 with containing the
     * contents of lzView`
     * @tparam Container The container along with its value type.
     * @param execution The execution policy. Must be one of `std::execution`'s tags.
     * @param args Additional container args. Must be compatible with the constructor of `Container`
     * @return The container.
     */
    template<class Container, class... Args, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Container to(Execution execution = std::execution::seq, Args&&... args) const {
        Container container(std::forward<Args>(args)...);
        tryReserve(container);
        if constexpr (internal::IsSequencedPolicyV<Execution>) {
            tryReserve(container);
            copyTo(std::inserter(container, container.begin()), execution);
        }
        else {
            static_assert(HasResize<Container>::value, "Container needs to have a method resize() in order to use parallel"
                                                       " algorithms. Use std::execution::seq instead");
            static_assert(internal::IsRandomAccess<It>::value,
                          "The iterator needs to be random access. Please use copyTo if you want to parallize this function");
            container.resize(_end - _begin);
            copyTo(container.begin(), execution);
        }
        return container;
    }

    /**
     * Fills destination output iterator `outputIterator` with current contents of [`begin()`, `end()`).
     * @param outputIterator The output to fill into. Essentially the same as:
     * `std::copy(lzView.begin(), lzView.end(), myContainer.begin());`
     * @param execution The execution policy. Must be one of `std::execution`'s tags.
     */
    template<class OutputIterator, class Execution = std::execution::sequenced_policy>
    LZ_CONSTEXPR_CXX_20 void copyTo(OutputIterator outputIterator, Execution execution = std::execution::seq) const {
        if constexpr (internal::checkForwardAndPolicies<Execution, OutputIterator>()) {
            std::copy(_begin, _end, outputIterator);
        }
        else {
            static_assert(IsForward<It>::value,
                          "The iterator type must be forward iterator or stronger. Prefer using std::execution::seq");
            std::copy(execution, _begin, _end, outputIterator);
        }
    }

    /**
     * Fills destination output iterator `outputIterator` with current contents of [`begin()`, `end()`), using `transformFunc`.
     * @param outputIterator The output to fill into.
     * @param transformFunc The transform function. Must be a callable object that has a parameter of the current value type.
     * Essentially the same as: `std::transform(lzView.begin(), lzView.end(), myContainer.begin(), [](T value) { ... });`
     * @param execution The execution policy. Must be one of `std::execution`'s tags.
     */
    template<class OutputIterator, class TransformFunc, class Execution = std::execution::sequenced_policy>
    LZ_CONSTEXPR_CXX_20 void
    transformTo(OutputIterator outputIterator, TransformFunc transformFunc, Execution execution = std::execution::seq) const {
        if constexpr (internal::IsSequencedPolicyV<Execution>) {
            std::transform(_begin, _end, outputIterator, transformFunc);
        }
        else {
            static_assert(IsForward<It>::value, "Iterator type must be at least forward to use parallel execution");
            static_assert(IsForward<OutputIterator>::value,
                          "Output iterator type must be at least forward to use parallel execution");
            std::transform(execution, _begin, _end, outputIterator, transformFunc);
        }
    }

    /**
     * @brief Creates a new `std::vector<value_type>` of the sequence.
     * @details Creates a new vector of the sequence. A default `std::allocator<value_type>`.
     * @param execution The execution policy. Must be one of `std::execution`'s tags.
     * @return A `std::vector<value_type>` with the sequence.
     */
    template<class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::vector<value_type> toVector(Execution execution = std::execution::seq) const {
        return to<std::vector>(execution);
    }

    /**
     * @brief Creates a new `std::vector<value_type, Allocator>`.
     * @details Creates a new `std::vector<value_type, Allocator>` with a specified allocator which can be passed
     * by this function.
     * @param execution The execution policy. Must be one of `std::execution`'s tags.
     * @param alloc The allocator.
     * @return A new `std::vector<value_type, Allocator>`.
     */
    template<class Allocator, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::vector<value_type, Allocator>
    toVector(const Allocator& alloc, Execution execution) const {
        return to<std::vector>(execution, alloc);
    }

    /**
     * @brief Creates a new `std::vector<value_type, N>`.
     * @tparam N The size of the array.
     * @param execution The execution policy. Must be one of `std::execution`'s tags.
     * @return A new `std::array<value_type, N>`.
     * @throws `std::out_of_range` if the size of the iterator is bigger than `N`.
     */
    template<std::size_t N, class Execution = std::execution::sequenced_policy>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::array<value_type, N> toArray(Execution execution = std::execution::seq) const {
        std::array<value_type, N> container{};
        copyTo(container.begin(), execution);
        return container;
    }
#    else
    /**
     * @brief Returns an arbitrary container type, of which its constructor signature looks like:
     * `Container(Iterator, Iterator[, args...])`. The args may be left empty. The type of the sequence is equal to
     * the typedef `value_type`.
     * @attention This function only works properly if the template types matches the container. Example:
     * `to<std::vector, std::allocator<int>>() // fails, no args given`
     * `to<std::vector, std::allocator<int>>(std::allocator<int>{}); // ok`
     * `to<std::vector>(std::allocator<int>{}); // also ok, able to deduce type`
     * `to<std::vector, std::allocator<int>>({}); // also ok`
     * @details Use this function to convert the iterator to a container. Example:
     * ```cpp
     * auto list = lazyIterator.to<std::list>();
     * auto allocator = std::allocator<int>();
     * auto set = lazyIterator.to<std::set>(allocator);
     * ```
     * @tparam Args Additional arguments, automatically deduced
     * @param args Additional arguments, for e.g. an allocator.
     * @return An arbitrary container specified by the entered template parameter.
     */
    template<template<class, class...> class Container, class... Args>
    Container<value_type, Decay<Args>...> to(Args&&... args) const {
        using Cont = Container<value_type, Decay<Args>...>;
        return to<Cont>(std::forward<Args>(args)...);
    }

    /**
     * @brief This function can be used to create a new container from the current view. The template parameter `Container`
     * must be specified along with its value type: `view.to<std::vector<int>>()`. One could also use `view.to<std::vector>()`.
     * See the other `to` function overload for documentation.
     * @tparam Container The container along with its value type.
     * @example `lzView.to<std::vector<int>>(100); // This will create a vec of size 100 with containing the contents of lzView`
     * @param args Additional container args. Must be compatible with the constructor of `Container`
     * @return The container.
     */
    template<class Container, class... Args>
    Container to(Args&&... args) const {
        Container cont(std::forward<Args>(args)...);
        tryReserve(cont);
        copyTo(std::inserter(cont, cont.begin()));
        return cont;
    }

    /**
     * Fills destination output iterator `outputIterator` with current contents of [`begin()`, `end()`)
     * @param outputIterator The output to fill into. Essentially the same as:
     * `std::copy(lzView.begin(), lzView.end(), myContainer.begin());`
     */
    template<class OutputIterator>
    void copyTo(OutputIterator outputIterator) const {
        std::copy(_begin, _end, outputIterator);
    }

    /**
     * Fills destination output iterator `outputIterator` with current contents of [`begin()`, `end()`), using `transformFunc`.
     * @param outputIterator The output to fill into.
     * @param transformFunc The transform function. Must be a callable object that has a parameter of the current value type.
     * Essentially the same as: `std::transform(lzView.begin(), lzView.end(), myContainer.begin(), [](T value) { ... });`
     */
    template<class OutputIterator, class TransformFunc>
    void transformTo(OutputIterator outputIterator, TransformFunc transformFunc) const {
        std::transform(_begin, _end, outputIterator, transformFunc);
    }

    /**
     * @brief Creates a new `std::vector<value_type>` of the sequence.
     * @details Creates a new vector of the sequence. A default `std::allocator<value_type>`.
     * @return A `std::vector<value_type>` with the sequence.
     */
    std::vector<value_type> toVector() const {
        return to<std::vector>();
    }

    /**
     * @brief Creates a new `std::vector<value_type, Allocator>`.
     * @details Creates a new `std::vector<value_type, Allocator>` with a specified allocator which can be passed
     * by this function.
     * @param alloc The allocator
     * @return A new `std::vector<value_type, Allocator>`.
     */
    template<class Allocator>
    std::vector<value_type, Allocator> toVector(const Allocator& alloc = Allocator()) const {
        return to<std::vector, Allocator>(alloc);
    }

    /**
     * @brief Creates a new `std::vector<value_type, N>`.
     * @tparam N The size of the array
     * @return A new `std::array<value_type, N>`.
     * @throws `std::out_of_range` if the size of the iterator is bigger than `N`.
     */
    template<std::size_t N>
    std::array<value_type, N> toArray() const {
        std::array<value_type, N> cont{};
        copyTo(cont.begin());
        return cont;
    }
#    endif // LZ_HAS_EXECUTION

    /**
     * Creates a `std::map<<keyGen return type, value_type[, Compare[, Allocator]]>`. The keyGen function generates the keys
     * for the `std::map`. The value type is the current type this view contains. (`typename decltype(view)::value_type`).
     * @param keyGen Function generates the keys for the `std::map`. Must contains 1 arg that is equal to `typename
     * decltype(view)::value_type`
     * @param allocator Optional, a custom allocator. `std::allocator<decltype(func(*begin()))>` is default.
     * @param cmp Optional, a custom key comparer. `std::less<decltype(func(*begin()))>` is default.
     * @return A `std::map` with as key type the return type of `keyGen`, and as value the current values contained by this view.
     */
    template<class KeySelectorFunc, class Compare = std::less<KeyType<KeySelectorFunc>>,
             class Allocator = std::allocator<std::pair<const KeyType<KeySelectorFunc>, value_type>>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::map<KeyType<KeySelectorFunc>, value_type, Compare, Allocator>
    toMap(const KeySelectorFunc keyGen, const Allocator& allocator = {}, const Compare& cmp = {}) const {
        using Map = std::map<KeyType<KeySelectorFunc>, value_type, Compare, Allocator>;
        Map m(cmp, allocator);
        createMap(m, keyGen);
        return m;
    }

    /**
     * Creates a `std::unordered_map<<keyGen return type, value_type[, Hasher[, KeyEquality[, Allocator]]]>`. The keyGen function
     * generates the keys for the `std::unordered_map`. The value type is the current type this view contains. (`typename
     * decltype(view)::value_type`).
     * @param keyGen Function generates the keys for the `std::unordered_map`. Must contains 1 arg that is equal to `typename
     * decltype(view)::value_type`
     * @param allocator Optional, a custom allocator. `std::allocator<decltype(func(*begin()))>` is default.
     * @param cmp Optional, a custom key comparer. `std::less<decltype(func(*begin()))>` is default.
     * @param cmp Optional, the key comparer. `std::equal_to<decltype(func(*begin()))>` is default.
     * @param h Hash function. `std::hash<decltype(func(*begin()))>` is default.
     * @return A `std::map` with as key type the return type of `keyGen`, and as value the current values contained by this view.
     */
    template<class KeySelectorFunc, class Hasher = std::hash<KeyType<KeySelectorFunc>>,
             class KeyEquality = std::equal_to<KeyType<KeySelectorFunc>>,
             class Allocator = std::allocator<std::pair<const KeyType<KeySelectorFunc>, value_type>>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::unordered_map<KeyType<KeySelectorFunc>, value_type, Hasher, KeyEquality, Allocator>
    toUnorderedMap(const KeySelectorFunc keyGen, const Allocator& alloc = {}, const KeyEquality& cmp = {},
                   const Hasher& h = {}) const {
        using UnorderedMap = std::unordered_map<KeyType<KeySelectorFunc>, value_type, Hasher, KeyEquality, Allocator>;
        UnorderedMap um(static_cast<std::size_t>(internal::sizeHint(_begin, _end)), h, cmp, alloc);
        createMap(um, keyGen);
        return um;
    }

    /**
     * Converts an iterator to a string, with a given delimiter. Example: lz::range(4).toString() yields 0123, while
     * lz::range(4).toString(" ") yields 0 1 2 3 4 and lz::range(4).toString(", ") yields 0, 1, 2, 3, 4.
     * @param delimiter The delimiter between the previous value and the next.
     * @param fmt The format args. (`{}` is default, not applicable if std::format isn't available or LZ_STANDALONE is defined)
     * @return The converted iterator in string format.
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 std::string
#    if defined(LZ_HAS_FORMAT) || !defined(LZ_STANDALONE)
    toString(const StringView delimiter = "", const StringView fmt = "{}") const {
        return internal::doMakeString(_begin, _end, delimiter, fmt);
#    else
    toString(const StringView delimiter = "") const {
        return internal::doMakeString(_begin, _end, delimiter);
#    endif
        // clang-format off
    }

    /**
     * Function to stream the iterator to an output stream e.g. `std::cout`.
     * @param o The stream object.
     * @param it The iterator to print.
     * @return The stream object by reference.
     */
    friend std::ostream& operator<<(std::ostream& o, const BasicIteratorView<It>& it) {
        return o << it.toString(" ");
    }

    /**
     * Returns the length of the view.
     * @note Please note that this traverses the whole sequence.
     * @return The length of the view.
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 internal::DiffType<It> distance() const {
        return std::distance(_begin, _end);
    }

    /**
     * Gets the nth position of the iterator from this sequence.
     * @param n The offset.
     * @return The element referred to by `begin() + n`
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 It next(const internal::DiffType<It> n = 1) const {
        return std::next(_begin, n);
    }

    /**
     * Checks is the sequence is empty.
     * @return True if it is empty, false otherwise.
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool empty() const {
        return lz::empty(_begin, _end);
    }

    //! See `lz::front(Iterable)` for details
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference front() const {
        return lz::front(*this);
    }

    /**
     * @brief See `lz::back` for details
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 reference back() const {
        return lz::back(*this);
    }
}; // namespace internal
// clang-format on
} // namespace internal

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 lz::internal::BasicIteratorView<internal::IterTypeFromIterable<Iterable>>
view(Iterable&& iterable) {
    return { internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)) };
}

#    ifndef LZ_HAS_EXECUTION
/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB,
         class BinaryPredicate = MAKE_BIN_OP(std::equal_to, internal::ValueTypeIterable<IterableA>)>
bool equal(const IterableA& a, const IterableB& b, BinaryPredicate predicate = {}) {
    return std::equal(std::begin(a), std::end(a), std::begin(b), std::end(b), std::move(predicate));
}
#    else  // ^^^ !LZ_HAS_EXECUTION vvv LZ_HAS_EXECUTION
/**
 * Use this function to check if two lz iterators are the same.
 * @param a An iterable, its underlying value type should have an operator== with `b`
 * @param b An iterable, its underlying value type should have an operator== with `a`
 * @return true if both are equal, false otherwise.
 */
template<class IterableA, class IterableB, class BinaryPredicate = std::equal_to<>,
         class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 bool
equal(const IterableA& a, const IterableB& b, BinaryPredicate predicate = {}, Execution execution = std::execution::seq) {
    if constexpr (internal::checkForwardAndPolicies<Execution, internal::IterTypeFromIterable<IterableA>>() &&
                  internal::checkForwardAndPolicies<Execution, internal::IterTypeFromIterable<IterableB>>()) {
        static_cast<void>(execution);
        return std::equal(std::begin(a), std::end(a), std::begin(b), std::end(b), std::move(predicate));
    }
    else {
        return std::equal(execution, std::begin(a), std::end(a), std::begin(b), std::end(b), std::move(predicate));
    }
}
#    endif // LZ_HAS_EXECUTION
} // Namespace lz

#    if !defined(LZ_STANDALONE)
template<class Iterable>
struct fmt::formatter<
    Iterable,
    lz::internal::EnableIf<
        std::is_base_of<lz::internal::BasicIteratorView<lz::internal::IterTypeFromIterable<Iterable>>, Iterable>::value, char>>
    : fmt::formatter<std::string> {
    using InnerIter = lz::internal::BasicIteratorView<lz::internal::IterTypeFromIterable<Iterable>>;

    template<class FormatCtx>
    auto format(const InnerIter& it, FormatCtx& ctx) const {
        return fmt::formatter<std::string>::format(it.toString(), ctx);
    }
};
#    endif

// End of group
/**
 * @}
 */

#endif // LZ_BASIC_ITERATOR_VIEW_HPP