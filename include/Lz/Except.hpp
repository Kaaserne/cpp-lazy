#pragma once

#ifndef LZ_EXCEPT_HPP
#define LZ_EXCEPT_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/iterators/ExceptIterator.hpp"

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_EXECUTION
template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept, class Comparer, class Execution>
class Except final : public detail::BasicIteratorView<detail::ExceptIterator<Iterator, IteratorToExcept, Comparer, Execution>> {
#else
template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept, class Comparer>
class Except final : public detail::BasicIteratorView<detail::ExceptIterator<Iterator, IteratorToExcept, Comparer>> {
#endif
public:
#ifdef LZ_HAS_EXECUTION
    using iterator = detail::ExceptIterator<Iterator, IteratorToExcept, Comparer, Execution>;
#else
    using iterator = detail::ExceptIterator<Iterator, IteratorToExcept, Comparer>;
#endif
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

#ifdef LZ_HAS_EXECUTION
    LZ_CONSTEXPR_CXX_20 Except(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd,
                               Comparer comparer, Execution execPolicy) :
        detail::BasicIteratorView<iterator>(iterator(std::move(begin), end, toExceptBegin, toExceptEnd, comparer, execPolicy),
                                            iterator(end, end, toExceptBegin, toExceptEnd, comparer, execPolicy)) {
    }
#else  // ^^^ has execution vvv ! has execution
    Except(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd, Comparer comparer) :
        detail::BasicIteratorView<iterator>(iterator(std::move(begin), end, std::move(toExceptBegin), toExceptEnd, comparer),
                                            iterator(end, end, toExceptEnd, toExceptEnd, comparer)) {
    }
#endif // LZ_HAS_EXECUTION

    constexpr Except() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

#ifdef LZ_HAS_EXECUTION
/**
 * @brief Skips elements in [begin, end) that is contained by [toExceptBegin, toExceptEnd). [toExceptBegin, toExceptEnd) must be
 * sorted manually before creating this view.
 * @attention [toExceptBegin, toExceptEnd) must be sorted  manually before creating this view.
 * @param execPolicy The std::execution::* policy.
 * @param begin The beginning of the sequence to skip elements in.
 * @param end The ending of the sequence to skip elements in.
 * @param toExceptBegin The beginning of the sequence that may not be contained in [begin, end).
 * @param toExceptEnd The ending of the sequence that may not be contained in [begin, end).
 * @param comparer Comparer for binary search (operator < is default) in IteratorToExcept.
 * @return An Except view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator, LZ_CONCEPT_ITERATOR IteratorToExcept, class Comparer = std::less<>,
         class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 Except<Iterator, IteratorToExcept, Comparer, Execution>
exceptRange(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd, Comparer comparer = {},
            Execution execPolicy = std::execution::seq) {
    return {
        std::move(begin), std::move(end), std::move(toExceptBegin), std::move(toExceptEnd), std::move(comparer), execPolicy
    };
}

/**
 * @brief Skips elements iterable that is contained by toExcept. ToExcept must be sorted manually before creating this view.
 * @attention ToExcept must be sorted manually before creating this view.
 * @param execPolicy The std::execution::* policy.
 * @param iterable Sequence to iterate over.
 * @param toExcept Sequence that contains items that must be skipped in `iterable`.
 * @param comparer Comparer for binary search (operator < is default) in IterableToExcept
 * @return An Except view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE IterableToExcept, class Comparer = std::less<>,
         class Execution = std::execution::sequenced_policy>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20
    Except<detail::IterTypeFromIterable<Iterable>, detail::IterTypeFromIterable<IterableToExcept>, Comparer, Execution>
    except(Iterable&& iterable, IterableToExcept&& toExcept, Comparer comparer = {}, Execution execPolicy = std::execution::seq) {
    return exceptRange(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                       detail::begin(std::forward<IterableToExcept>(toExcept)),
                       detail::end(std::forward<IterableToExcept>(toExcept)), std::move(comparer), execPolicy);
}

#else // ^^^ has execution vvv ! has execution
/**
 * @brief Skips elements in [begin, end) that is contained by [toExceptBegin, toExceptEnd). [toExceptBegin, toExceptEnd) must be
 * sorted manually before creating this view.
 * @attention [toExceptBegin, toExceptEnd) must be sorted  manually before creating this view.
 * @param begin The beginning of the sequence to skip elements in.
 * @param end The ending of the sequence to skip elements in.
 * @param toExceptBegin The beginning of the sequence that may not be contained in [begin, end).
 * @param toExceptEnd The ending of the sequence that may not be contained in [begin, end).
 * @param comparer Comparer for binary search (operator < is default) in IteratorToExcept.
 * @return An Except view object.
 */
#ifdef LZ_HAS_CXX_11
template<class Iterator, class IteratorToExcept, class Comparer = std::less<detail::ValueType<Iterator>>>
#else
template<class Iterator, class IteratorToExcept, class Comparer = std::less<>>
#endif // LZ_HAS_CXX_11
Except<Iterator, IteratorToExcept, Comparer>
exceptRange(Iterator begin, Iterator end, IteratorToExcept toExceptBegin, IteratorToExcept toExceptEnd, Comparer comparer = {}) {
    return { std::move(begin), std::move(end), std::move(toExceptBegin), std::move(toExceptEnd), std::move(comparer) };
}

/**
 * @brief Skips elements iterable that is contained by toExcept. ToExcept must be sorted manually before creating this view.
 * @attention ToExcept must be sorted manually before creating this view.
 * @param iterable Sequence to iterate over.
 * @param toExcept Sequence that contains items that must be skipped in `iterable`.
 * @param comparer Comparer for binary search (operator < is default) in IterableToExcept
 * @return An Except view object.
 */
#ifdef LZ_HAS_CXX_11
template<class Iterable, class IterableToExcept, class Comparer = std::less<detail::ValueTypeIterable<Iterable>>>
#else
template<class Iterable, class IterableToExcept, class Comparer = std::less<>>
#endif // LZ_HAS_CXX_11
Except<detail::IterTypeFromIterable<Iterable>, detail::IterTypeFromIterable<IterableToExcept>, Comparer>
except(Iterable&& iterable, IterableToExcept&& toExcept, Comparer comparer = {}) {
    return exceptRange(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                       detail::begin(std::forward<IterableToExcept>(toExcept)),
                       detail::end(std::forward<IterableToExcept>(toExcept)), std::move(comparer));
}
#endif // LZ_HAS_EXECUTION

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // end namespace lz


#endif // end LZ_EXCEPT_HPP