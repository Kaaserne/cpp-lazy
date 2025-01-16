#pragma once

#ifndef LZ_UNIQUE_HPP
#define LZ_UNIQUE_HPP

#include <Lz/detail/basic_iterable.hpp>
#include <Lz/detail/iterators/unique.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<LZ_CONCEPT_ITERATOR Iterator, class S, class BinaryPredicate>
class unique_iterable final
    : public detail::basic_iterable<detail::unique_iterator<Iterator, S, BinaryPredicate>,
                                    typename detail::unique_iterator<Iterator, S, BinaryPredicate>::sentinel> {
public:
    using iterator = detail::unique_iterator<Iterator, S, BinaryPredicate>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    constexpr unique_iterable(Iterator begin, S end, BinaryPredicate compare, std::forward_iterator_tag) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(begin), std::move(end), std::move(compare))) {
    }

    constexpr unique_iterable(Iterator begin, Iterator end, BinaryPredicate compare, std::bidirectional_iterator_tag) :
        detail::basic_iterable<iterator>(iterator(begin, begin, end, compare), iterator(end, begin, end, compare)) {
    }

public:
    constexpr unique_iterable(Iterator begin, S end, BinaryPredicate compare) :
        unique_iterable(std::move(begin), std::move(end), std::move(compare), iter_cat_t<Iterator>{}) {
    }

    constexpr unique_iterable() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief Returns an unique_iterable iterator view object.
 * @attention `iterable` must be sorted in order to work correctly.
 * @details Use this iterator view to eventually get an iterator of unique_iterable values.
 * @param iterable The iterable sequence.
 * @param binary_predicate The comparer. operator< is assumed by default.
 * @return An unique_iterable iterator view object, which can be used to iterate over in a `(for ... : unqiue(...))`
 * fashion.
 */
template<class Iterable, class BinaryPredicate = MAKE_BIN_PRED(std::less, ref_iterable_t<Iterable>)>
LZ_NODISCARD constexpr unique_iterable<iter_t<Iterable>, sentinel_t<Iterable>, BinaryPredicate>
unique(Iterable&& iterable, BinaryPredicate binary_predicate = {}) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             std::move(binary_predicate) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // end namespace lz

#endif // end LZ_UNIQUE_HPP