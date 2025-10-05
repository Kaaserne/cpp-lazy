#pragma once

#ifndef LZ_STREAM_HPP
#define LZ_STREAM_HPP

#include <Lz/basic_iterable.hpp> // for operator|
#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <ostream>
#include <string>

#if FMT_VERSION >= 80000
#endif

// clang-format off
#if !defined(LZ_STANDALONE)
  #ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Weffc++"
    #pragma GCC diagnostic ignored "-Wswitch-default"
  #endif
  #include <fmt/format.h>
  #include <fmt/ostream.h>
  #include <fmt/ranges.h>
  #ifdef __GNUC__
    #pragma GCC diagnostic pop
  #endif
#elif defined(LZ_HAS_FORMAT)
  #include <format>
#endif // !defined(LZ_STANDALONE)

// clang-format on

namespace lz {
namespace detail {

struct iterable_formatter {
    using adaptor = iterable_formatter;

#if !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)

    /**
     * @brief Function that can be used to format an iterable to an output
     * stream. Only defined if c++ 20 or if using `{fmt}`. Example:
     * ```cpp
     * std::vector<int> vec = { 2, 4 };
     * lz::format(vec, std::cout, ", ", "{}"); // prints: 2, 4
     * lz::format(vec, std::cout, ","); // prints: 2,4
     * std::stringstream oss;
     * lz::format(vec, oss); // oss contains: 2, 4
     * ```
     *
     * @param iterable Any iterable. May be a container or another iterable.
     * @param stream The output stream to write to
     * @param separator The separator to use between elements. Default is ", "
     * @param format The format to use for each element. Default is "{}"
     */
    template<class Iterable>
    void
    operator()(const Iterable& iterable, std::ostream& stream, const char* separator = ", ", const char* format = "{}") const {
        auto it = iterable.begin();
        auto end = iterable.end();
        if (it == end) {
            return;
        }

#ifndef LZ_STANDALONE

#if FMT_VERSION >= 80000

        fmt::print(stream, fmt::runtime(format), *it);
        for (++it; it != end; ++it) {
            fmt::print(stream, "{}", separator);
            fmt::print(stream, fmt::runtime(format), *it);
        }

#else

        fmt::print(stream, format, *it);
        for (++it; it != end; ++it) {
            fmt::print(stream, "{}", separator);
            fmt::print(stream, format, *it);
        }

#endif // FMT_VERSION >= 80000

#else // ^^ LZ_STANDALONE - vv !LZ_STANDALONE

        std::ostreambuf_iterator<char> out_it(stream);
        std::vformat_to(out_it, format, std::make_format_args(*it));

        for (++it; it != end; ++it) {
            std::vformat_to(out_it, "{}", std::make_format_args(separator));
            std::vformat_to(out_it, format, std::make_format_args(*it));
        }

#endif // !LZ_STANDALONE
    }

#else

    /**
     * @brief Function that can be used to format an iterable to an output
     * stream. Only defined if c++ 20 is not defined or not using `{fmt}`.
     * Example:
     * ```cpp
     * std::vector<int> vec = { 2, 4 };
     * lz::format(vec, std::cout, ", "); // prints: 2, 4
     * lz::format(vec, std::cout, ","); // prints: 2,4
     *
     * @param separator The separator to use between elements. Default is ", "
     */
    template<class Iterable>
    void operator()(const Iterable& iterable, std::ostream& stream, const char* separator = ", ") const {
        auto it = iterable.begin();
        auto end = iterable.end();
        if (it == end) {
            return;
        }
        stream << *it;
        for (++it; it != end; ++it) {
            stream << separator << *it;
        }
    }

#endif // !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)

#if !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)

    /**
     * @brief Function that can be used with the pipe operator. This overload can be used with C++20's std::format or {fmt}.
     *
     * @param separator The separator to use between elements. Default is ", "
     * @param format The format to use for each element. Default is "{}"
     * @return A function object that can be used with the pipe operator
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, const char*, const char*>
    operator()(const char* separator = ", ", const char* format = "{}") const {
        return { separator, format };
    }

    /**
     * @brief Function that can be used with the pipe operator. This overload can be used with C++20's std::format or {fmt}.
     * Example:
     * ```cpp
     * std::vector<int> vec = { 2, 4 };
     * std::string output = vec | lz::format(std::cout, ", ", "{}"); // prints: 2, 4
     * std::string output = vec | lz::format(std::cout, ","); // prints: 2,4
     * std::string output = vec | lz::format(std::cout); // prints: 2, 4
     * ```
     *
     * @param stream The output stream to write to
     * @param separator The separator to use between elements. Default is ", "
     * @return A function object that can be used with the pipe operator
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, std::ostream&, const char*, const char*>
    operator()(std::ostream& stream, const char* separator = ", ", const char* format = "{}") const {
        return { stream, separator, format };
    }

#else

    /**
     * @brief Function that can be used with the pipe operator. It takes a separator. Only defined if c++ 20
     * is not defined or not using `{fmt}`. Example:
     * ```cpp
     * std::vector<int> vec = { 2, 4 };
     * std::string output = vec | lz::format(", "); // prints: 2, 4
     * std::string output = vec | lz::format; // prints: 2, 4
     * ```
     *
     * @param separator The separator to use between elements. Default is ", "
     * @return A function object that can be used with the pipe operator
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, const char*> operator()(const char* separator = ", ") const {
        return { separator };
    }

    /**
     * @brief Function that can be used with the pipe operator. It takes an output stream and a separator. Only defined if c++ 20
     * is not defined or not using `{fmt}`. Example:
     * ```cpp
     * std::vector<int> vec = { 2, 4 };
     * std::string output = vec | lz::format(std::cout, ", "); // prints: 2, 4
     * std::string output = vec | lz::format(std::cout); // prints: 2, 4
     * ```
     *
     * @param stream The output stream to write to
     * @param separator The separator to use between elements. Default is ", "
     * @return A function object that can be used with the pipe operator
     */
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, std::ostream&, const char*>
    operator()(std::ostream& stream, const char* separator = ", ") const {
        return { stream, separator };
    }

#endif // !defined(LZ_STANDALONE) || defined(LZ_HAS_FORMAT)

#if !defined(LZ_STANDALONE)

    /**
     * @brief Converts an iterable to a string, using the given separator and format. This overload can be used with C++20's
     * std::format or {fmt}. Example:
     * ```cpp
     * std::vector<int> vec = { 2, 4 };
     * std::string output = lz::format(vec, ", ", "{}"); // 2, 4
     * std::string output = lz::format(vec, ","); // 2,4
     * std::string output = lz::format(vec); // 2, 4
     * ```
     *
     * @param iterable Any iterable. May be a container or another iterable.
     * @param separator The separator to use between elements. Default is ", "
     * @param format The format to use for each element. Default is "{}"
     * @return The string representation of the iterable, with the given separator and format.
     */
    template<class Iterable>
    LZ_NODISCARD std::string operator()(const Iterable& iterable, const char* separator = ", ", const char* format = "{}") const {
#if FMT_VERSION >= 80000

        return fmt::format(fmt::runtime(format), fmt::join(iterable, separator));

#else

        return fmt::format(format, fmt::join(iterable, separator));

#endif // FMT_VERSION >= 80000
    }

#elif defined(LZ_HAS_FORMAT)

    /**
     * @brief Converts an iterable to a string, using the given separator and format. This overload can be used with C++20's
     * std::format or {fmt}. Example:
     * ```cpp
     * std::vector<int> vec = { 2, 4 };
     * std::string output = lz::format(vec, ", ", "{}"); // 2, 4
     * std::string output = lz::format(vec, ","); // 2,4
     * std::string output = lz::format(vec); // 2, 4
     * ```
     *
     * @param iterable Any iterable. May be a container or another iterable.
     * @param separator The separator to use between elements. Default is ", "
     * @param format The format to use for each element. Default is "{}"
     * @return The string representation of the iterable, with the given separator and format.
     */
    template<class Iterable>
    LZ_NODISCARD std::string operator()(const Iterable& iterable, const char* separator = ", ", const char* format = "{}") const {
        auto it = iterable.begin();
        auto end = iterable.end();
        if (it == end) {
            return "";
        }

        std::string result;
        auto back_inserter = std::back_inserter(result);

        std::vformat_to(back_inserter, format, std::make_format_args(*it));
        auto sep = std::make_format_args(separator);

        for (++it; it != end; ++it) {
            std::vformat_to(back_inserter, "{}", sep);
            std::vformat_to(back_inserter, format, std::make_format_args(*it));
        }

        return result;
    }

#else

    /**
     * @brief Converts an iterable to a string, using the given separator, using std::stringstream if c++ 20 is not defined or not
     * using `{fmt}`. Example:
     * ```cpp
     * std::vector<int> vec = { 2, 4 };
     * std::string output = lz::format(vec, ","); // 2,4
     * std::string output = lz::format(vec); // 2, 4
     * ```
     *
     * @param iterable Any iterable. May be a container or another iterable.
     * @param separator The separator to use between elements. Default is ", "
     * @return The string representation of the iterable, with the given separator.
     */
    template<class Iterable>
    LZ_NODISCARD std::string operator()(const Iterable& iterable, const char* separator = ", ") const {
        std::ostringstream oss;
        (*this)(iterable, oss, separator);
        return oss.str();
    }

#endif // !defined(LZ_STANDALONE)
};
} // namespace detail
} // namespace lz

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Streams any iterable to an output stream. For printing, `std::cout << <lz_iterable>` can also be used. Example:
 * ```cpp
 * std::vector<int> vec = { 2, 4 };
 *
 * // Requires C++20 or {fmt}. If using {fmt}, LZ_STANDALONE must *not* be defined.
 * lz::format(vec, std::cout, ", ", "{}"); // prints: 2, 4
 * lz::format(vec, std::cout, ","); // prints: 2,4
 * lz::format(vec, std::cout); // prints: 2, 4
 *
 * std::string output = lz::format(vec, ", ", "{}"); // 2, 4
 * std::string output = lz::format(vec, ","); // 2,4
 * std::string output = lz::format(vec); // 2, 4
 *
 * vec | lz::format(std::cout, ", ", "{}"); // prints: 2, 4
 * vec | lz::format(std::cout, ","); // prints: 2,4
 * vec | lz::format(std::cout); // prints: 2, 4
 * std::string output = vec | lz::format(", ", "{}"); // 2, 4
 * std::string output = vec | lz::format; // 2, 4
 * std::string output = vec | lz::format(","); // 2,4
 *
 * // Otherwise, if not using {fmt} or C++20, use the following:
 * lz::format(vec, std::cout, ", "); // prints: 2, 4
 * lz::format(vec, std::cout); // prints: 2, 4
 * std::string output = lz::format(vec, ", "); // 2, 4
 * std::string output = lz::format(vec); // 2,4
 *
 * // Or use the pipe operator:
 * vec | lz::format(std::cout, ", "); // prints: 2, 4
 * vec | lz::format(std::cout); // prints: 2, 4
 * std::string output = vec | lz::format(","); // 2,4
 * std::string output = vec | lz::format; // 2, 4
 * ```
 */
LZ_INLINE_VAR constexpr detail::iterable_formatter format{};

} // namespace lz

#ifdef LZ_HAS_CONCEPTS

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
LZ_MODULE_EXPORT template<class Iterable>
std::ostream& operator<<(std::ostream& stream, const Iterable& iterable)
    requires(std::is_base_of_v<lz::lazy_view, Iterable>)
{
    lz::format(iterable, stream);
    return stream;
}

#else

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
LZ_MODULE_EXPORT template<class Iterable>
lz::detail::enable_if_t<std::is_base_of<lz::lazy_view, Iterable>::value, std::ostream&>
operator<<(std::ostream& stream, const Iterable& iterable) {
    lz::format(iterable, stream);
    return stream;
}

#endif

#endif
