#pragma once

#ifndef LZ_STREAM_HPP
#define LZ_STREAM_HPP

#include <Lz/basic_iterable.hpp> // for operator|
#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/traits.hpp>
#include <string>

namespace lz {
namespace detail {

struct iterable_formatter {
    using adaptor = iterable_formatter;

#if !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)

    template<LZ_CONCEPT_ITERABLE Iterable>
    std::ostream&
    operator()(const Iterable& iterable, std::ostream& stream, const char* separator = ", ", const char* format = "{}") const {
        return stream << (*this)(iterable, separator, format);
    }

#else

    template<LZ_CONCEPT_ITERABLE Iterable>
    std::ostream& operator()(const Iterable& iterable, std::ostream& stream, const char* separator = ", ") const {
        auto begin = std::begin(iterable);
        auto end = std::end(iterable);
        if (begin == end) {
            return stream;
        }
        stream << *begin++;
        for (; begin != end; ++begin) {
            stream << separator << *begin;
        }

        return stream;
    }

#endif

#if !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, const char*, const char*>
    operator()(const char* separator = ", ", const char* format = "{}") const {
        return { separator, format };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, std::ostream&, const char*, const char*>
    operator()(std::ostream& stream, const char* separator = ", ", const char* format = "{}") const {
        return { stream, separator, format };
    }

#else

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, const char*> operator()(const char* separator = ", ") const {
        return { separator };
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, std::ostream&, const char*>
    operator()(std::ostream& stream, const char* separator = ", ") const {
        return { stream, separator };
    }

#endif

#if !defined(LZ_STANDALONE)

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD std::string operator()(const Iterable& iterable, const char* separator = ", ", const char* format = "{}") const {
#if FMT_VERSION >= 80000

        return fmt::format(fmt::runtime(format), fmt::join(iterable, separator));

#else

        return fmt::format(format, fmt::join(iterable, separator));

#endif
    }

#elif defined(LZ_HAS_FORMAT)

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD std::string operator()(const Iterable& iterable, const char* separator = ", ", const char* format = "{}") const {
        auto begin = std::begin(iterable);
        auto end = std::end(iterable);
        if (begin == end) {
            return "";
        }

        std::string result;
        auto back_inserter = std::back_inserter(result);

        std::vformat_to(back_inserter, format, std::make_format_args(*begin));
        ++begin;

        std::string_view fmt{ format };
        std::string_view sep{ separator };

        const auto empty_fmt_args = std::make_format_args();
        for (; begin != end; ++begin) {
            std::vformat_to(back_inserter, sep, empty_fmt_args);
            std::vformat_to(back_inserter, fmt, std::make_format_args(*begin));
        }

        return result;
    }

#else

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD std::string operator()(const Iterable& iterable, const char* separator = ", ") const {
        std::ostringstream oss;
        (*this)(iterable, oss, separator);
        return oss.str();
    }

#endif
};

struct iterable_printer {
    using adaptor = iterable_printer;

#if !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, const char*, const char*>
    operator()(const char* separator = ", ", const char* format = "{}") const {
        return { separator, format };
    }

#else

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, const char*> operator()(const char* separator = ", ") const {
        return { separator };
    }

#endif

#if !defined(LZ_STANDALONE)

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD void operator()(const Iterable& iterable, const char* separator = ", ", const char* format = "{}") const {
#if FMT_VERSION >= 80000

        fmt::print(fmt::runtime(format), fmt::join(iterable, separator));

#else

        fmt::print(format, fmt::join(iterable, separator));

#endif
    }

#elif defined(LZ_HAS_FORMAT)

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD void operator()(const Iterable& iterable, const char* separator = ", ", const char* format = "{}") const {
        constexpr iterable_formatter iter_formatter{};
        std::string result = iter_formatter(iterable, separator, format);
        std::fputs(result.c_str(), stdout);
    }

#else

    template<LZ_CONCEPT_ITERABLE Iterable>
    LZ_NODISCARD void operator()(const Iterable& iterable, const char* separator = ", ") const {
        constexpr iterable_formatter formatter{};
        formatter(iterable, std::cout, separator);
    }

#endif
};
} // namespace detail

LZ_MODULE_EXPORT_SCOPE_BEGIN

// TODO docs

#ifdef LZ_HAS_CXX_11

constexpr detail::iterable_printer print{};

constexpr detail::iterable_formatter format{};

#else

LZ_INLINE_VAR constexpr detail::iterable_printer print{};

LZ_INLINE_VAR constexpr detail::iterable_formatter format{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Streams a `lz` iterable to an output stream. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto filter = lz::filter(vec, [](int i) { return i % 2 == 0; });
 * std::cout << filter; // 2, 4
 * ```
 *
 * @param stream The stream to output to
 * @param iterable The `lz` iterable to output
 */
template<LZ_CONCEPT_ITERABLE Iterable>
lz::detail::enable_if<std::is_base_of<lz::lazy_view, Iterable>::value, std::ostream&>
operator<<(std::ostream& stream, const Iterable& iterable) {
    return lz::format(iterable, stream);
}

LZ_MODULE_EXPORT_SCOPE_END

#endif
