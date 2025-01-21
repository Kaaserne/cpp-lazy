// #pragma once

// #ifndef LZ_EXCEPT_ITERATOR_HPP
// #define LZ_EXCEPT_ITERATOR_HPP

// #include <Lz/detail/fake_ptr_proxy.hpp>
// #include <Lz/detail/func_container.hpp>
// #include <Lz/iterator_base.hpp>
// #include <algorithm>

// namespace lz {
// namespace detail {
// template<class Iterator, class S, class IteratorToExcept, class S2, class BinaryPredicate>
// class except_iterator
//     : public iter_base<except_iterator<Iterator, S, IteratorToExcept, S2, BinaryPredicate>, ref_t<Iterator>,
//                        fake_ptr_proxy<ref_t<Iterator>>, diff_type<Iterator>, std::forward_iterator_tag, default_sentinel> {

//     using iter_traits = std::iterator_traits<Iterator>;

// public:
//     using value_type = typename iter_traits::value_type;
//     using difference_type = typename iter_traits::difference_type;
//     using reference = typename iter_traits::reference;
//     using pointer = fake_ptr_proxy<reference>;

// private:
//     Iterator _iterator;
//     IteratorToExcept _to_except_begin;
//     S2 _to_except_end;
//     S _end;
//     mutable func_container<BinaryPredicate> _predicate;

//     LZ_CONSTEXPR_CXX_14 void find_next() {
//         using detail::binary_search;
//         using detail::find_if;
//         using std::binary_search;
//         using std::find_if;

//         _iterator = find_if(std::move(_iterator), _end, [this](ref_t<Iterator> value) {
//             return !binary_search(_to_except_begin, _to_except_end, value, _predicate);
//         });
//     }

// public:
//     constexpr except_iterator() = default;

//     LZ_CONSTEXPR_CXX_14
//     except_iterator(Iterator begin, S end, IteratorToExcept except_begin, S2 except_end, BinaryPredicate compare) :
//         _iterator(std::move(begin)),
//         _to_except_begin(std::move(except_begin)),
//         _to_except_end(std::move(except_end)),
//         _end(std::move(end)),
//         _predicate(std::move(compare)) {
//         if (_to_except_begin == _to_except_end) {
//             return;
//         }
//         find_next();
//     }

//     constexpr reference dereference() const {
//         return *_iterator;
//     }

//     LZ_CONSTEXPR_CXX_17 pointer arrow() const {
//         return fake_ptr_proxy<decltype(**this)>(**this);
//     }

//     LZ_CONSTEXPR_CXX_14 void increment() {
//         ++_iterator;
//         find_next();
//     }

//     constexpr bool eq(const except_iterator& b) const {
//         return _iterator == b._iterator;
//     }

//     constexpr bool eq(default_sentinel) const {
//         return _iterator == _end;
//     }
// };
// } // namespace detail
// } // namespace lz

// #endif