#pragma once

#ifndef LZ_CONCEPTS_HPP
#define LZ_CONCEPTS_HPP

#include <Lz/detail/compiler_checks.hpp>

#ifdef LZ_HAS_CONCEPTS

#include <iterator>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

template<class I>
concept iterable = requires(I i) {
    { std::begin(i) } -> std::input_or_output_iterator;
};

template<class I>
concept bidirectional_iterable =
    requires(std::remove_cvref_t<I> i) { requires std::bidirectional_iterator<decltype(std::begin(i))>; };

LZ_MODULE_EXPORT_SCOPE_END

} // End namespace lz

#define LZ_CONCEPT_ITERABLE lz::iterable
#define LZ_CONCEPT_BIDIRECTIONAL_ITERABLE lz::bidirectional_iterable

#else // ^^^ has concepts !has concepts vvv

#define LZ_CONCEPT_ITERABLE class
#define LZ_CONCEPT_BIDIRECTIONAL_ITERABLE class
#endif // LZ_HAS_CONCEPTS

#endif // LZ_CONCEPTS_HPP