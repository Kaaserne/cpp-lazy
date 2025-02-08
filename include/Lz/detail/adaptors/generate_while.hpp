#pragma once

#ifndef LZ_GENERATE_WHILE_ADAPTOR_HPP
#define LZ_GENERATE_WHILE_ADAPTOR_HPP

#include <Lz/detail/iterables/generate_while.hpp>

namespace lz {
namespace detail {
struct generate_while_adaptor {
    using adaptor = generate_while_adaptor;

    template<class GeneratorFunc>
    LZ_NODISCARD constexpr generate_while_iterable<GeneratorFunc> operator()(GeneratorFunc generator_func) const {
        using pair = decltype(generator_func());
        using pair_first = decltype(std::get<0>(std::declval<pair>()));
        static_assert(std::is_convertible<detail::decay_t<pair_first>, bool>::value,
                      "Function must return a std::pair compatible object (i.e. object::first, object::second), where "
                      "object::first"
                      "returns a bool like object.");
        return { std::move(generator_func) };
    }
};
} // namespace detail
} // namespace lz

#endif
