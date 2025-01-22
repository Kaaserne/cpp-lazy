// #pragma once

// #ifndef LZ_TAKE_WHILE_HPP
// #define LZ_TAKE_WHILE_HPP

// #include <Lz/basic_iterable.hpp>
// #include <Lz/detail/iterators/take_while.hpp>

// namespace lz {
// LZ_MODULE_EXPORT_SCOPE_BEGIN

// template<class Iterator, class S, class UnaryPredicate>
// class take_while_iterable : public detail::basic_iterable<detail::take_while_iterator<Iterator, S, UnaryPredicate>,
//                                                           typename detail::take_while_iterator<Iterator, S,
//                                                           UnaryPredicate>::sentinel> {
// private:
//     LZ_CONSTEXPR_CXX_14
//     take_while_iterable(Iterator begin, S end, UnaryPredicate predicate, std::forward_iterator_tag /* is_bidi */) :
//         detail::basic_iterable<iterator, default_sentinel>(iterator(std::move(begin), end, predicate)) {
//     }

//     LZ_CONSTEXPR_CXX_14
//     take_while_iterable(Iterator begin, Iterator end, UnaryPredicate predicate, std::bidirectional_iterator_tag /* is_bidi */)
//     :
//         detail::basic_iterable<iterator>(iterator(begin, begin, end, predicate), iterator(end, begin, end, predicate)) {
//     }

// public:
//     using iterator = detail::take_while_iterator<Iterator, S, UnaryPredicate>;
//     using const_iterator = iterator;
//     using value_type = typename iterator::value_type;

//     LZ_CONSTEXPR_CXX_14 take_while_iterable() = default;

//     LZ_CONSTEXPR_CXX_14 take_while_iterable(Iterator begin, S end, UnaryPredicate predicate) :
//         take_while_iterable(std::move(begin), std::move(end), std::move(predicate), iter_cat_t<Iterator>{}) {
//     }
// };

// /**
//  * @brief This iterator can be used to select elements while a predicate @p predicate is true.
//  * @param iterable An object that has methods `begin()` and `end()`.
//  * @param predicate A function that returns a bool and passes a value type in its argument. If the function returns
//  * false, the iterator stops.
//  * @return A Take object that can be converted to an arbitrary container or can be iterated over using
//  * `for (auto... lz::take_while(...))`.
//  */
// template<LZ_CONCEPT_ITERABLE Iterable, class UnaryPredicate>
// LZ_NODISCARD LZ_CONSTEXPR_CXX_14 take_while_iterable<iter_t<Iterable>, sentinel_t<Iterable>, UnaryPredicate>
// take_while(Iterable&& iterable, UnaryPredicate predicate) {
//     return { std::forward<Iterable>(iterable).begin()), std::forward<Iterable>(iterable).end(),
//              std::move(predicate) };
// }

// LZ_MODULE_EXPORT_SCOPE_END

// } // namespace lz

// #endif // LZ_TAKE_WHILE_HPP