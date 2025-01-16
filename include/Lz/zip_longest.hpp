#pragma once

#ifndef LZ_ZIP_LONGEST_HPP
#define LZ_ZIP_LONGEST_HPP

#include <Lz/detail/basic_iterable.hpp>
#include <Lz/detail/iterators/zip_longest.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class IsRa, class IterTuple, class SentinelTuple>
class zip_longest_iterable final
    : public detail::basic_iterable<detail::zip_longest_iterator<IsRa::value, IterTuple, SentinelTuple>,
                                    typename detail::zip_longest_iterator<IsRa::value, IterTuple, SentinelTuple>::sentinel> {
public:
    using iterator = detail::zip_longest_iterator<IsRa::value, IterTuple, SentinelTuple>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

private:
    static_assert(std::tuple_size<IterTuple>::value > 0, "Cannot create zip longest object with 0 iterators");

    LZ_CONSTEXPR_CXX_14 zip_longest_iterable(IterTuple begin, IterTuple end, std::true_type /* is bidirectional access */) :
        detail::basic_iterable<iterator>(iterator(begin, begin, end), iterator(begin, end, end)) {
    }

    LZ_CONSTEXPR_CXX_14
    zip_longest_iterable(IterTuple begin, SentinelTuple end, std::false_type /* is not bidirectional access */) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(begin), std::move(end))) {
    }

public:
    using iterator = detail::zip_longest_iterator<IsRa::value, IterTuple, SentinelTuple>;
    using const_iterator = iterator;

    using value_type = typename iterator::value_type;

    LZ_CONSTEXPR_CXX_14 zip_longest_iterable(IterTuple begin, SentinelTuple end) :
        zip_longest_iterable(std::move(begin), std::move(end), IsRa{}) {
    }

    constexpr zip_longest_iterable() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

template<class... Iterables>
using zip_longest_iterable_t =
    zip_longest_iterable<detail::is_bidi_tag<detail::iter_tuple_iter_cat_t<std::tuple<iter_t<Iterables>...>>>,
                         std::tuple<iter_t<Iterables>...>, std::tuple<sentinel_t<Iterables>...>>;

/**
 * @brief Zip iterator that zips multiple iterators.
 *
 * @details Zip iterator that zips multiple iterators, except the difference beteween zip_longest and zip is that zip longest stops
 * when the longest iterator is at its end. Other iterators that have been 'exhausted' will be filled with an empty optional
 * object, rather than a valid optional object. This is std::optional (if post C++ 17) or lz::detail::optional (if pre C++).
 * lz::detail::optional contains most of the basic funcitonality std::optional does. This iterator is a forward iterator if one of
 * the iterators passed is bidirectional or lower, random access otherwise.
 *
 * @note All values retrieved from the iterator are by value.
 * @param iterables The iterables objects
 * @return zip_longest_iterable object that contains the iterator.
 */
template<LZ_CONCEPT_ITERABLE... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 zip_longest_iterable_t<Iterables...> zip_longest(Iterables&&... iterables) {
    return { std::make_tuple(detail::begin(std::forward<Iterables>(iterables))...),
             std::make_tuple(detail::end(std::forward<Iterables>(iterables))...) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_ZIP_LONGEST_HPP