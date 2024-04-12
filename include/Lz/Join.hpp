#pragma once

#ifndef LZ_JOIN_HPP
#define LZ_JOIN_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/JoinIterator.hpp"

// clang-format off
#if defined(LZ_STANDALONE) && defined(LZ_HAS_FORMAT)
#include <format>
#elif defined(LZ_STANDALONE)
#include <ostream>
#else
#include <fmt/ostream.h>
#endif
// clang-format on

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator>
class Join final : public internal::BasicIteratorView<internal::JoinIterator<Iterator>> {
public:
    using iterator = internal::JoinIterator<Iterator>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#if defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)
    LZ_CONSTEXPR_CXX_20 Join(Iterator begin, Iterator end, std::string delimiter) :
        internal::BasicIteratorView<iterator>(iterator(std::move(begin), delimiter, true),
                                              iterator(std::move(end), delimiter, false)) {
    }
#else
    LZ_CONSTEXPR_CXX_20
    Join(Iterator begin, Iterator end, std::string delimiter, std::string fmt) :
        internal::BasicIteratorView<iterator>(iterator(std::move(begin), delimiter, fmt, true),
                                              iterator(std::move(end), delimiter, fmt, false)) {
    }
#endif // has format

    Join() = default;

    friend std::ostream& operator<<(std::ostream& o, const Join<Iterator>& joinIter) {
        if (joinIter.empty()) {
            return o;
        }
        auto begin = joinIter.begin();
        auto iter = begin._getIterator();
        auto end = joinIter.end()._getIterator();
        const auto& delimiter = begin._getDelimiter();
// clang-format off
#if defined(LZ_STANDALONE) && defined(LZ_HAS_FORMAT) 
        std::format_to(std::ostreambuf_iterator<char>(o), "{}", *iter);
#elif defined(LZ_STANDALONE)
        o << *iter;
#else
        fmt::print(o, "{}", *iter);     
#endif
        // clang-format on
        for (++iter; iter != end; ++iter) {
// clang-format off
#if defined(LZ_STANDALONE) && defined(LZ_HAS_FORMAT)
            std::format_to(std::ostreambuf_iterator<char>(o), "{}{}", delimiter, *iter);
#elif defined(LZ_STANDALONE)
            o << delimiter << *iter;
#else
            fmt::print(o, "{}{}", delimiter, *iter);
#endif
            // clang-format on
        }
        return o;
    }
};

/**
 * @addtogroup ItFns
 * @{
 */

#if defined(LZ_STANDALONE) && !defined(LZ_HAS_FORMAT)
/**
 * @brief Creates a Join object.
 * @note If you're going to call .toString() on this, it is better to use `strJoin` for performance reasons.
 * @details Combines the iterator values followed by the delimiter. It is evaluated in a
 * `"[value][delimiter][value][delimiter]..."`-like fashion.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param delimiter The delimiter to separate the previous and the next values in the sequence.
 * @return A Join iterator view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Join<Iterator> joinRange(Iterator begin, Iterator end, std::string delimiter) {
    return { std::move(begin), std::move(end), std::move(delimiter) };
}

/**
 * @brief Creates a Join object.
 * @note If you're going to call .toString() on this, it is better to use `strJoin` for performance reasons.
 * @details Combines the iterator values followed by the delimiter. It is evaluated in a
 * `"[value][delimiter][value][delimiter]..."`-like fashion.
 * @param iterable The iterable to join with the delimiter.
 * @param delimiter The delimiter to separate the previous and the next values in the sequence.
 * @return A Join iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Join<internal::IterTypeFromIterable<Iterable>> join(Iterable&& iterable, std::string delimiter) {
    return { std::begin(iterable), std::end(iterable), std::move(delimiter) };
}

/**
 * Converts a sequence to a  `std::string` without creating an iterator Join object.
 * @param begin The beginning of the sequence
 * @param end The ending of the sequence
 * @param delimiter The delimiter to separate each value from the sequence.
 * @return A string where each item in `iterable` is appended to a string, separated by `delimiter`.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
std::string strJoinRange(Iterator begin, Iterator end, const StringView delimiter = "") {
    return internal::BasicIteratorView<Iterator>(std::move(begin), std::move(end)).toString(delimiter);
}

/**
 * Converts a sequence to a  `std::string` without creating an iterator Join object.
 * @param iterable The iterable to convert to string
 * @param delimiter The delimiter to separate each value from the sequence.
 * @return A string where each item in `iterable` is appended to a string, separated by `delimiter`.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
std::string strJoin(Iterable&& iterable, const StringView delimiter = "") {
    return strJoinRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                        delimiter);
}

#else

/**
 * @brief Creates a Join object.
 * @note If you're going to call .toString() on this, it is better to use `strJoin` for performance reasons.
 * @details Combines the iterator values followed by the delimiter. It is evaluated in a
 * `"[value][delimiter][value][delimiter]..."`-like fashion.
 * @param begin The beginning of the sequence.
 * @param end The ending of the sequence.
 * @param delimiter The delimiter to separate the previous and the next values in the sequence.
 * @param fmt The std:: or fmt:: formatting args (`"{}"` is default).
 * @return A Join iterator view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Join<Iterator>
joinRange(Iterator begin, Iterator end, std::string delimiter, std::string fmt = "{}") {
    return { std::move(begin), std::move(end), std::move(delimiter), std::move(fmt) };
}

/**
 * @brief Creates a Join object.
 * @note If you're going to call .toString() on this, it is better to use `strJoin` for performance reasons.
 * @details Combines the iterator values followed by the delimiter. It is evaluated in a
 * `"[value][delimiter][value][delimiter]..."`-like fashion.
 * @param iterable The iterable to join with the delimiter.
 * @param delimiter The delimiter to separate the previous and the next values in the sequence.
 * @param fmt The std:: or fmt:: formatting args (`"{}"` is default).
 * @return A Join iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Join<internal::IterTypeFromIterable<Iterable>>
join(Iterable&& iterable, std::string delimiter, std::string fmt = "{}") {
    return { std::begin(iterable), std::end(iterable), std::move(delimiter), std::move(fmt) };
}

/**
 * Converts a sequence to a  `std::string` without creating an iterator Join object.
 * @param begin The beginning of the sequence
 * @param end The ending of the sequence
 * @param delimiter The delimiter to separate each value from the sequence.
 * @param fmt The format args. (`{}` is default, not applicable if std::format isn't available or LZ_STANDALONE is defined)
 * @return A string where each item in `iterable` is appended to a string, separated by `delimiter`.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
std::string strJoinRange(Iterator begin, Iterator end, const StringView delimiter = "", const StringView fmt = "{}") {
    return internal::BasicIteratorView<Iterator>(std::move(begin), std::move(end)).toString(delimiter, fmt);
}

/**
 * Converts a sequence to a `std::string` without creating an iterator Join object.
 * @param iterable The iterable to convert to string
 * @param delimiter The delimiter to separate each value from the sequence.
 * @param fmt The format args. (`{}` is default, not applicable if std::format isn't available or LZ_STANDALONE is defined)
 * @return A string where each item in `iterable` is appended to a string, separated by `delimiter`.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
std::string strJoin(Iterable&& iterable, const StringView delimiter = "", const StringView fmt = "{}") {
    return strJoinRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)),
                        delimiter, fmt);
}
#endif // has format

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif