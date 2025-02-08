#pragma once

#ifndef LZ_GENERATE_ADAPTOR_HPP
#define LZ_GENERATE_ADAPTOR_HPP

#include <Lz/detail/iterables/generate.hpp>

namespace lz {
namespace detail {
struct generate_adaptor {
    using adaptor = generate_adaptor;

    template<class GeneratorFunc>
    LZ_NODISCARD constexpr generate_iterable<GeneratorFunc>
    operator()(GeneratorFunc generatorFunc, const std::size_t amount = (std::numeric_limits<std::size_t>::max)()) const {
        return { std::move(generatorFunc), amount };
    }
};
} // namespace detail
} // namespace lz

#endif