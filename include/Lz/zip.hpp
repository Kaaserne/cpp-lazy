#pragma once

#ifndef LZ_ZIP_HPP
#define LZ_ZIP_HPP

#include <Lz/detail/basic_iterable.hpp>
#include <Lz/detail/iterators/zip.hpp>

namespace lz {
template<class IterTuple, class SentinelTuple>
class zip_iterable;

namespace detail {
template<class Tuple, class SentinelTuple, std::size_t... Is>
LZ_CONSTEXPR_CXX_14 Tuple create_end_smallest_iterator(const Tuple& begin, const SentinelTuple& end,
                                                       index_sequence_helper<Is...>) {
    const std::ptrdiff_t lengths[] = { static_cast<std::ptrdiff_t>(std::get<Is>(end) - std::get<Is>(begin))... };
    const auto smallest_length = *std::min_element(std::begin(lengths), std::end(lengths));
    // If we use begin + smallest_length, we get compile errors for non random access iterators. However, we know that we are
    // dealing with a random access iterator, so std::next does a + internally. It is implemented this way to prevent more
    // enable_if's from appearing
    return Tuple{ (std::get<Is>(begin) + smallest_length)... };
}

template<class Tuple, class SentinelTuple>
LZ_CONSTEXPR_CXX_14 zip_iterable<decay_t<Tuple>, decay_t<SentinelTuple>>
construct_zip_iterable(std::true_type /* is_ra */, Tuple&& begin, SentinelTuple&& end) {
    using is = make_index_sequence<std::tuple_size<decay_t<Tuple>>::value>;
    end = create_end_smallest_iterator(begin, std::forward<SentinelTuple>(end), is{});
    return { std::forward<Tuple>(begin), std::move(end) };
}

template<class Tuple, class SentinelTuple>
constexpr zip_iterable<decay_t<Tuple>, decay_t<SentinelTuple>>
construct_zip_iterable(std::false_type /* is_ra */, Tuple&& begin, SentinelTuple&& end) {
    return { std::forward<Tuple>(begin), std::forward<SentinelTuple>(end) };
}
} // namespace detail

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class IterTuple, class SentinelTuple>
class zip_iterable final : public detail::basic_iterable<detail::zip_iterator<IterTuple, SentinelTuple>,
                                                         typename detail::zip_iterator<IterTuple, SentinelTuple>::sentinel> {

    constexpr zip_iterable(IterTuple begin, IterTuple end, std::bidirectional_iterator_tag /* unused */) :
        detail::basic_iterable<iterator, iterator>(iterator(std::move(begin)), iterator(std::move(end))) {
    }

    constexpr zip_iterable(IterTuple begin, SentinelTuple end, std::forward_iterator_tag /* unused */) :
        detail::basic_iterable<iterator, SentinelTuple>(iterator(std::move(begin)), std::move(end)) {
    }

public:
    using iterator = detail::zip_iterator<IterTuple, SentinelTuple>;
    using const_iterator = iterator;

    using value_type = typename iterator::value_type;

    constexpr zip_iterable(IterTuple begin, SentinelTuple end) :
        zip_iterable(std::move(begin), std::move(end), detail::iter_tuple_iter_cat_t<IterTuple>{}) {
    }

    constexpr zip_iterable() = default;
};

// Start of group
/**
 * @addtogroup ItFns
 * @{
 */

/**
 * @brief This function can be used to iterate over multiple containers. It stops at its smallest container.
 * Its `begin()` function returns an iterator. The operators `<, <=, >, >=` will return true
 * if one of the containers returns true with its corresponding `operator<`/`operator<=`/`operator>`/`operator>=`.
 * @details The tuple that is returned by `operator*` returns a `std::tuple` by value and its elements by
 * reference e.g. `std::tuple<Args&...>`. So it is possible to alter the values in the container/iterable),
 * unless the iterator is const, making it a const reference.
 * @param iterables The iterables to iterate simultaneously over.
 * @return A Take object that can be converted to an arbitrary container or can be iterated over using
 * `for (auto tuple :  lz::zip(...))`.
 */
template<LZ_CONCEPT_ITERABLE... Iterables>
LZ_NODISCARD constexpr zip_iterable<std::tuple<iter_t<Iterables>...>, std::tuple<sentinel_t<Iterables>...>>
zip(Iterables&&... iterables) {
    static_assert(sizeof...(Iterables) > 0, "Cannot create zip object with 0 iterators");
    using common_tag = detail::common_type<iter_cat_t<iter_t<Iterables>>...>;
    auto begin = std::make_tuple(std::begin(iterables)...);
    auto end = std::make_tuple(std::end(iterables)...);
    return detail::construct_zip_iterable(detail::is_ra_tag<common_tag>{}, std::move(begin), std::move(end));
}
// End of group
/**
 * @}
 */
} // namespace lz

LZ_MODULE_EXPORT_SCOPE_END

#endif