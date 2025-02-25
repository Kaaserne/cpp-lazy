#pragma once

#ifndef LZ_SPLIT_ADAPTOR_HPP
#define LZ_SPLIT_ADAPTOR_HPP

#include <Lz/c_string.hpp>
#include <Lz/detail/iterables/split.hpp>

namespace lz {
namespace detail {
template<class ValueType>
struct split_adaptor {
    using adaptor = split_adaptor<ValueType>;

    template<LZ_CONCEPT_ITERABLE Iterable, class CharT>
    constexpr split_iterable<ValueType, Iterable, c_string_iterable<const CharT*>>
    operator()(Iterable&& iterable, const CharT* delimiter) const {
        return (*this)(std::forward<Iterable>(iterable), c_string(delimiter));
    }

    template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE Iterable2>
    constexpr split_iterable<ValueType, Iterable, Iterable2> operator()(Iterable&& iterable, Iterable2&& delimiter) const {
        return { std::forward<Iterable>(iterable), std::forward<Iterable2>(delimiter) };
    }

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, Iterable> operator()(Iterable&& iterable) const {
        return { std::forward<Iterable>(iterable) };
    }

    template<LZ_CONCEPT_ITERABLE Iterable, class CharT>
    LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, c_string_iterable<const CharT*>> operator()(const CharT* delimiter) const {
        return (*this)(c_string(delimiter));
    }
};

template<>
struct split_adaptor<void> {
    using adaptor = split_adaptor<void>;

    template<LZ_CONCEPT_ITERABLE Iterable, class CharT>
    constexpr split_iterable<basic_iterable<iter_t<Iterable>>, Iterable, c_string_iterable<const CharT*>>
    operator()(Iterable&& iterable, const CharT* delimiter) const {
        return (*this)(std::forward<Iterable>(iterable), c_string(delimiter));
    }

    template<LZ_CONCEPT_ITERABLE Iterable, LZ_CONCEPT_ITERABLE Iterable2>
    constexpr split_iterable<basic_iterable<iter_t<Iterable>>, Iterable, Iterable2>
    operator()(Iterable&& iterable, Iterable2&& delimiter) const {
        constexpr split_adaptor<basic_iterable<iter_t<Iterable>>> adaptor{};
        return adaptor(std::forward<Iterable>(iterable), std::forward<Iterable2>(delimiter));
    }

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, Iterable> operator()(Iterable&& iterable) const {
        constexpr split_adaptor<basic_iterable<iter_t<Iterable>>> adaptor{};
        return adaptor(std::forward<Iterable>(iterable));
    }

    template<LZ_CONCEPT_ITERABLE Iterable, class CharT>
    LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, c_string_iterable<const CharT*>> operator()(const CharT* delimiter) const {
        return (*this)(c_string(delimiter));
    }
};

} // namespace detail
} // namespace lz

#endif