#pragma once

#ifndef LZ_AS_ITERATOR_ADAPTOR_HPP
#define LZ_AS_ITERATOR_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/as_iterator.hpp>

namespace lz {
namespace detail {

template<class IterTag>
using is_input_tag = std::is_convertible<IterTag, std::input_iterator_tag>;

template<class IterTag>
using is_output_tag = std::is_convertible<IterTag, std::output_iterator_tag>;

struct as_iterator_adaptor {
    using adaptor = as_iterator_adaptor;

    /**
     * @brief Creates an `as_iterator_iterable` from an iterable. This means it will return its underling iterators instead of
     * using the `operator*`. This can be useful for when you need the iterator and the deref operator. The iterable has a size
     * method if the underlying iterable is sized. Returns the same sentinel as its underlying iterable. Example:
     * ```cpp
     * std::vector<int> vec = {1, 2, 3, 4, 5};
     * auto iter = lz::as_iterator(vec);
     * for (const auto vector_iterator : iter) {
     *     std::cout << *vector_iterator << " "; // prints: 1 2 3 4 5
     * }
     * ```
     * @param iterable
     * @return An `as_iterator_iterable` that contains the iterators of the iterable.
     */
    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD constexpr as_iterator_iterable<remove_ref<Iterable>, iter_cat_iterable_t<Iterable>>
    operator()(Iterable&& iterable) const {
        return { std::forward<Iterable>(iterable) };
    }

    /**
     * @brief Creates an `as_iterator_iterable` from an iterable and decays into the iterator category `IterCat`. This means it
     * will return its underling iterators instead of using the `operator*` and only contains the operations associated with that
     * iterator category. This can be useful for when you need the iterator and the deref operator and want to decay it to a lower
     * iterator type. The iterable has a size method if the underlying iterable is sized. Returns the same sentinel as its
     * underlying iterable. An example for random access iterators:
     * ```cpp
     * std::vector<int> vec = {1, 2, 3, 4, 5};
     * auto iter = lz::as_iterator(vec, std::forward_iterator_tag{}); // decays to forward iterator
     * for (const auto vector_iterator : iter) {
     *     std::cout << *vector_iterator << " "; // prints: 1 2 3 4 5
     * }
     * ```
     * @param iterable The iterable to get its iterators from and decay to the iterator category `IterCat`.
     * @param IterCat The iterator category to decay the iterable to
     * @return An `as_iterator_iterable` that contains the iterators of the iterable decayed to the iterator category `IterCat`.
     */
    template<LZ_CONCEPT_ITERABLE Iterable, class IterCat>
    LZ_NODISCARD constexpr as_iterator_iterable<remove_ref<Iterable>, IterCat> operator()(Iterable&& iterable, IterCat) const {
        return { std::forward<Iterable>(iterable) };
    }

    // clang-format off

    /**
     * @brief Creates an `as_iterator_iterable` from an iterable and decays into the iterator category `IterCat`. This means it
     * will return its underling iterators instead of using the `operator*` and only contains the operations associated with that
     * iterator category. This can be useful for when you need the iterator and the deref operator and want to decay it to a lower
     * iterator type. The iterable has a size method if the underlying iterable is sized. Returns the same sentinel as its
     * underlying iterable. An example for random access iterators:
     * ```cpp
     * std::vector<int> vec = {1, 2, 3, 4, 5};
     * auto iter = vec | lz::as_iterator(std::forward_iterator_tag{}); // decays to forward iterator
     * for (const auto vector_iterator : iter) {
     *     std::cout << *vector_iterator << " "; // prints: 1 2 3 4 5
     * }
     * ```
     * @param IterCat The iterator category to decay the iterable to
     * @return An an adaptor that can be used in a pipe expression
     */
    template<class IterCat>
    LZ_NODISCARD constexpr 
    enable_if<is_input_tag<IterCat>::value || is_output_tag<IterCat>::value, fn_args_holder<adaptor, IterCat>>
    operator()(IterCat) const {
        return {};
    }

    // clang-format on
};

} // namespace detail
} // namespace lz

#endif // LZ_AS_ITERATOR_ADAPTOR_HPP
