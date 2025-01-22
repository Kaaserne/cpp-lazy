// #pragma once

// #ifndef LZ_CARTESIAN_PRODUCT_HPP
// #define LZ_CARTESIAN_PRODUCT_HPP

// #include <Lz/basic_iterable.hpp>
// #include <Lz/detail/iterators/cartesian_product.hpp>

// namespace lz {

// LZ_MODULE_EXPORT_SCOPE_BEGIN

// using detail::cartesian_product_iterable;

// template<LZ_CONCEPT_ITERABLE... Iterables>
// struct cartesian_product_adaptor {
//     std::tuple<Iterables...> iterables;

//     constexpr cartesian_product_adaptor() = default;

//     LZ_CONSTEXPR_CXX_14 cartesian_product_adaptor(const Iterables&... iterables) : iterables(iterables...) {
//     }

//     template<class Iterable, std::size_t... Is>
//     LZ_NODISCARD LZ_CONSTEXPR_CXX_14 cartesian_product_iterable<Iterable, Iterables...>
//     construct(Iterable&& lhs, detail::index_sequence_helper<Is...>) const {
//         return { std::forward<Iterable>(lhs), std::get<Is>(std::forward<Iterables>(iterables))... };
//     }

//     template<class Iterable>
//     LZ_NODISCARD LZ_CONSTEXPR_CXX_14 cartesian_product_iterable<Iterable, Iterables...> operator()(Iterable&& lhs) const {
//         return construct(std::forward<Iterable>(lhs), detail::make_index_sequence<sizeof...(Iterables)>());
//     }

//     auto begin() const {
//         return (*this)(std::get<0>(iterables)).begin();
//     }

//     auto end() const {
//         return (*this)(std::get<0>(iterables)).end();
//     };
// };

// template<>
// struct cartesian_product_adaptor<void> {
//     template<LZ_CONCEPT_ITERABLE... Iterables>
//     LZ_NODISCARD LZ_CONSTEXPR_CXX_14 cartesian_product_adaptor<Iterables...> operator()(Iterables&&... iterables) const {
//         return cartesian_product_adaptor<Iterables...>{ std::forward<Iterables>(iterables)... };
//     }
// };

// /**
//  * @addtogroup ItFns
//  * @{
//  */

// /**
//  * Creates an iterator view object that, when iterated over, gets all possible combinations of all its values of the iterables.
//  * @param iterables The iterables to make all of the possible combinations with.
//  * @return A cartesian_product product view object.
//  */
// LZ_INLINE_VAR constexpr cartesian_product_adaptor<void> cartesian_product{};

// // End of group
// /**
//  * @}
//  */

// LZ_MODULE_EXPORT_SCOPE_END

// } // namespace lz

// #endif // LZ_CARTESIAN_PRODUCT_HPP
