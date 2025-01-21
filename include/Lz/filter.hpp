// #pragma once

// #ifndef LZ_FILTER_HPP
// #define LZ_FILTER_HPP

// #include <Lz/basic_iterable.hpp>
// #include <Lz/detail/iterators/filter.hpp>

// namespace lz {

// LZ_MODULE_EXPORT_SCOPE_BEGIN

// template<LZ_CONCEPT_ITERATOR Iterator, class S, class UnaryPredicate>
// class filter_iterable final
//     : public detail::basic_iterable<detail::filter_iterator<Iterator, S, UnaryPredicate>,
//                                     typename detail::filter_iterator<Iterator, S, UnaryPredicate>::sentinel> {
// public:
//     using iterator = detail::filter_iterator<Iterator, S, UnaryPredicate>;
//     using const_iterator = iterator;
//     using value_type = typename iterator::value_type;

// private:
//     using s = typename detail::filter_iterator<Iterator, S, UnaryPredicate>::sentinel;

//     LZ_CONSTEXPR_CXX_14 filter_iterable(Iterator begin, S end, UnaryPredicate function, std::forward_iterator_tag /* unused */)
//     :
//         detail::basic_iterable<iterator, s>(iterator(begin, begin, end, function)) {
//     }

//     LZ_CONSTEXPR_CXX_14
//     filter_iterable(Iterator begin, Iterator end, UnaryPredicate function, std::bidirectional_iterator_tag /* unused */) :
//         detail::basic_iterable<iterator, iterator>(iterator(begin, begin, end, function), iterator(end, begin, end, function))
//         {
//     }

// public:
//     LZ_CONSTEXPR_CXX_14 filter_iterable(Iterator begin, S end, UnaryPredicate function) :
//         filter_iterable(std::move(begin), std::move(end), std::move(function), iter_cat_t<Iterator>{}) {
//     }

//     constexpr filter_iterable() = default;
// };

// /**
//  * @addtogroup ItFns
//  * @{
//  */

// /**
//  * Returns a forward filter iterator. If the `predicate` returns false, the value it is excluded.
//  * I.e. `lz::filter({1, 2, 3, 4, 5}, [](int i){ return i % 2 == 0; });` will eventually remove all
//  * elements that are not even.
//  * @param iterable An iterable, e.g. a container / object with `begin()` and `end()` methods.
//  * @param predicate A function that must return a bool, and needs a value type of the container as parameter.
//  * @return A filter iterator that can be converted to an arbitrary container or can be iterated
//  * over using `for (auto... lz::filter(...))`.
//  */
// template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
// LZ_NODISCARD LZ_CONSTEXPR_CXX_14 filter_iterable<iter_t<Iterable>, sentinel_t<Iterable>, UnaryPredicate>
// filter(Iterable&& iterable, UnaryPredicate predicate) {
//     return { std::forward<Iterable>(iterable).begin()), std::forward<Iterable>(iterable).end(),
//              std::move(predicate) };
// }

// // End of group
// /**
//  * @}
//  */

// LZ_MODULE_EXPORT_SCOPE_END

// } // namespace lz

// #endif // end LZ_FILTER_HPP