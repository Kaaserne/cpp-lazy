#pragma once

#ifndef LZ_JOIN_ADAPTOR_HPP
#define LZ_JOIN_ADAPTOR_HPP

#include <Lz/detail/iterables/join.hpp>
#include <sstream>

#ifdef LZ_HAS_CHARCONV
#include <charconv>
#endif

namespace lz {
namespace detail {

#ifdef LZ_HAS_FORMAT

template<class T>
struct formatter {
    std::string operator()(const T value) const {
        return std::format("{}", value);
    }
};

#else

template<class T, class = void>
struct formatter;

template<class T>
struct formatter<T, enable_if<std::is_arithmetic<T>::value>> {
    std::string operator()(const T value) const {
        return std::to_string(value);
    }
};

template<class T>
struct formatter<T, enable_if<!std::is_same<std::string, T>::value>> {
private:
    std::ostringstream _oss;

public:
    std::string operator()(const T& value) const {
        _oss.str("");
        _oss << value;
        return _oss.str();
    }
};

template<class T>
struct formatter<T, enable_if<std::is_same<std::string, T>::value>> {
    const std::string& operator()(const std::string& value) const noexcept {
        return value;
    }

    std::string& operator()(T& value) const noexcept {
        return value;
    }
};

#endif

class join_adaptor {
    template<class Iterable, class UnaryFormatFn = formatter<val_iterable_t<Iterable>>>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_20 join_iterable<Iterable, UnaryFormatFn>
    operator()(Iterable&& iterable, std::string separator, UnaryFormatFn format_fn = {}) const {
        return { std::forward<Iterable>(iterable), std::move(separator), std::move(format_fn) };
    }
};

} // namespace detail
} // namespace lz

#endif // LZ_JOIN_ADAPTOR_HPP