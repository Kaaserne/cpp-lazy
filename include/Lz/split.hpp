#pragma once

#ifndef LZ_STRING_SPLITTER_HPP
#define LZ_STRING_SPLITTER_HPP

#include <Lz/c_string.hpp>
#include <Lz/detail/iterators/split.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class Iterator, class S, class Iterator2, class S2>
class split_iterable final : public detail::basic_iterable<detail::split_iterator<Iterator, S, Iterator2, S2>, default_sentinel> {
public:
    using const_iterator = detail::split_iterator<Iterator, S, Iterator2, S2>;
    using iterator = const_iterator;

public:
    using value_type = basic_iterable<iterator>;

    LZ_CONSTEXPR_CXX_20 split_iterable(Iterator begin, S end, Iterator2 begin2, S2 end2) :
        detail::basic_iterable<iterator, default_sentinel>(
            iterator(std::move(begin), std::move(end), std::move(begin2), std::move(end2))) {
    }

    split_iterable() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

template<class Iterable, class Iterable2>
using regular_splitter = split_iterable<iter_t<Iterable>, sentinel_t<Iterable>, iter_t<Iterable2>, sentinel_t<Iterable2>>;

template<class Iterable, class Iterable2>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 regular_splitter<iter_t<Iterable>, iter_t<Iterable2>>
split(Iterable&& iterable, Iterable2&& delimiter) {
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             detail::begin(std::forward<Iterable>(delimiter)), detail::end(std::forward<Iterable>(delimiter)) };
}

template<class Iterable, class CharT>
using string_splitter = split_iterable<iter_t<Iterable>, sentinel_t<Iterable>,
                                       detail::c_string_iterator<const CharT*, std::forward_iterator_tag>, default_sentinel>;

template<class Iterable, class CharT>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 string_splitter<Iterable, CharT> split(Iterable&& iterable, const CharT* delimiter) {
    auto string = lz::c_string(delimiter);
    return { detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
             detail::begin(std::move(string)), detail::end(std::move(string)) };
}

template<class Iterator, class S>
LZ_NODISCARD LZ_CONSTEXPR_CXX_20 basic_string_view<val_t<Iterator>> to_string_view(const basic_iterable<Iterator, S>& view) {
    static_assert(detail::is_ra<Iterator>::value, "Iterator must be at least a random access iterator");
    return basic_string_view<val_t<Iterator>>(std::addressof(*std::begin(view)), static_cast<std::size_t>(view.distance()));
}

// End of group
/**
 * @}
 */

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_STRING_SPLITTER_HPP