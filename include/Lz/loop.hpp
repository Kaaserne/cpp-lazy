#pragma once

#ifndef LZ_LOOP_HPP
#define LZ_LOOP_HPP

#include <Lz/detail/basic_iterable.hpp>
#include <Lz/detail/iterators/loop.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S>
class loop_iterable final
    : public detail::basic_iterable<detail::loop_iterator<Iterator, S>, typename detail::loop_iterator<Iterator, S>::sentinel> {

    constexpr loop_iterable(Iterator begin, S end, std::forward_iterator_tag) :
        detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(begin), std::move(begin), std::move(end))) {
    }

    constexpr loop_iterable(Iterator begin, S end, std::bidirectional_iterator_tag) :
        detail::basic_iterable<iterator>(iterator(begin, begin, end), iterator(end, begin, end)) {
    }

public:
    using iterator = detail::loop_iterator<Iterator, S>;
    using const_iterator = iterator;
    using value_type = typename iterator::value_type;

    constexpr loop_iterable(Iterator begin, S end) : loop_iterable(std::move(begin), std::move(end), iter_cat_t<Iterator>{}) {
    }

    loop_iterable() = default;
};

/**
 * @addtogroup ItFns
 * @{
 */

/**
 * Loops over iterable indefinitely
 *
 * @param iterable The sequence to loop over.
 * @return A loop iterator object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
LZ_NODISCARD constexpr loop_iterable<iter_t<Iterable>, sentinel_t<Iterable>> loop(Iterable&& iterable) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)) };
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_LOOP_HPP
