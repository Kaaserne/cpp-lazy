#ifndef LZ_DUPLICATES_ITERATOR_HPP
#define LZ_DUPLICATES_ITERATOR_HPP

#include <Lz/detail/algorithm.hpp>
#include <Lz/detail/fake_ptr_proxy.hpp>
#include <Lz/iterator_base.hpp>
#include <utility>

namespace lz {
namespace detail {
template<class Iterator, class S, class BinaryPredicate, class = void>
class duplicates_iterator;

template<class Iterator, class S, class BinaryPredicate>
class duplicates_iterator<Iterator, S, BinaryPredicate, enable_if<is_ra<Iterator>::value>>
    : public lz::iterator<duplicates_iterator<Iterator, S, BinaryPredicate>, std::pair<ref_t<Iterator>, std::size_t>,
                          fake_ptr_proxy<std::pair<ref_t<Iterator>, std::size_t>>, diff_type<Iterator>,
                          std::bidirectional_iterator_tag, duplicates_iterator<Iterator, S, BinaryPredicate>> {

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = std::pair<typename traits::value_type, std::size_t>;
    using reference = std::pair<typename traits::reference, std::size_t>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;
    using iterator_category = std::bidirectional_iterator_tag;

private:
    Iterator _begin;
    Iterator _last;
    Iterator _first;
    S _end;
    BinaryPredicate _compare;

    LZ_CONSTEXPR_CXX_14 void next() {
        using detail::find_if;
        using std::find_if;

        _last = find_if(_first, _end, [this](typename traits::reference val) { return _compare(*_first, val); });
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr duplicates_iterator()
        requires std::default_initializable<Iterator> && std::default_initializable<Iterable> &&
                     std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = Iterator,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<S>::value &&
                               std::is_default_constructible<BinaryPredicate>::value>>
    constexpr duplicates_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<S>::value &&
                                             std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14 duplicates_iterator(Iterator it, Iterator begin, S end, BinaryPredicate compare) :
        _begin{ std::move(begin) },
        _last{ std::move(it) },
        _first{ std::move(_last) },
        _end{ std::move(end) },
        _compare{ std::move(compare) } {
        next();
    }

    LZ_CONSTEXPR_CXX_14 duplicates_iterator& operator=(default_sentinel) {
        _first = _end;
        return *this;
    }

    constexpr reference dereference() const {
        return { *_first, static_cast<std::size_t>(_last - _first) };
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<reference>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        _first = std::move(_last);
        next();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        _last = _first;

        while (_first != _begin) {
            --_first;
            auto prev = std::prev(_first);
            if (_compare(*prev, *_first)) {
                return;
            }
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(const duplicates_iterator& other) const {
        LZ_ASSERT(_begin == other._begin && _end == other._end, "Incompatible iterators");
        return _first == other._first;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(default_sentinel) const {
        return _first == _end;
    }
};

template<class Iterator, class S, class BinaryPredicate>
class duplicates_iterator<Iterator, S, BinaryPredicate, enable_if<!is_ra<Iterator>::value && is_bidi<Iterator>::value>>
    : public lz::iterator<duplicates_iterator<Iterator, S, BinaryPredicate>, std::pair<ref_t<Iterator>, std::size_t>,
                          fake_ptr_proxy<std::pair<ref_t<Iterator>, std::size_t>>, diff_type<Iterator>,
                          std::bidirectional_iterator_tag, duplicates_iterator<Iterator, S, BinaryPredicate>> {

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = std::pair<typename traits::value_type, std::size_t>;
    using reference = std::pair<typename traits::reference, std::size_t>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

private:
    Iterator _begin;
    Iterator _last;
    Iterator _first;
    std::size_t _last_distance;
    S _end;
    BinaryPredicate _compare;

    LZ_CONSTEXPR_CXX_14 void next() {
        using detail::find_if;
        using std::find_if;

        _last_distance = 0;
        _last = find_if(_first, _end, [this](typename traits::reference val) {
            const auto condition = _compare(*_first, val);
            if (!condition) {
                ++_last_distance;
            }
            return condition;
        });
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr duplicates_iterator()
        requires std::default_initializable<Iterator> && std::default_initializable<S> &&
                     std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = Iterator,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<S>::value &&
                               std::is_default_constructible<BinaryPredicate>::value>>
    constexpr duplicates_iterator() noexcept(std::is_nothrow_default_constructible<I>::value &&
                                             std::is_nothrow_default_constructible<S>::value &&
                                             std::is_nothrow_default_constructible<BinaryPredicate>::value) {
    }

#endif

    LZ_CONSTEXPR_CXX_14 duplicates_iterator(Iterator it, Iterator begin, S end, BinaryPredicate compare) :
        _begin{ std::move(begin) },
        _last{ std::move(it) },
        _first{ std::move(_last) },
        _end{ std::move(end) },
        _compare{ std::move(compare) } {
        next();
    }

    LZ_CONSTEXPR_CXX_14 duplicates_iterator& operator=(default_sentinel) {
        _first = _end;
        return *this;
    }

    constexpr reference dereference() const {
        return { *_first, _last_distance };
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<reference>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        _first = std::move(_last);
        next();
    }

    LZ_CONSTEXPR_CXX_14 void decrement() {
        _last_distance = 0;
        _last = _first;

        while (_first != _begin) {
            --_first;
            auto prev = std::prev(_first);
            ++_last_distance;
            if (_compare(*prev, *_first)) {
                return;
            }
        }
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(const duplicates_iterator& other) const {
        LZ_ASSERT(_begin == other._begin && _end == other._end, "Incompatible iterators");
        return _first == other._first;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(default_sentinel) const {
        return _first == _end;
    }
};

template<class Iterator, class S, class BinaryPredicate>
class duplicates_iterator<Iterator, S, BinaryPredicate, enable_if<!is_bidi<Iterator>::value>>
    : public lz::iterator<duplicates_iterator<Iterator, S, BinaryPredicate>, std::pair<ref_t<Iterator>, std::size_t>,
                          fake_ptr_proxy<std::pair<ref_t<Iterator>, std::size_t>>, diff_type<Iterator>, iter_cat_t<Iterator>,
                          default_sentinel> {

    using traits = std::iterator_traits<Iterator>;

public:
    using value_type = std::pair<typename traits::value_type, std::size_t>;
    using reference = std::pair<typename traits::reference, std::size_t>;
    using pointer = fake_ptr_proxy<reference>;
    using difference_type = typename traits::difference_type;

private:
    Iterator _last;
    Iterator _first;
    std::size_t _last_distance;
    S _end;
    BinaryPredicate _compare;

    LZ_CONSTEXPR_CXX_14 void next() {
        using detail::find_if;
        using std::find_if;

        _last_distance = 0;
        _last = find_if(_first, _end, [this](typename traits::reference val) {
            const auto condition = _compare(*_first, val);
            if (!condition) {
                ++_last_distance;
            }
            return condition;
        });
    }

public:
#ifdef LZ_HAS_CONCEPTS

    constexpr duplicates_iterator()
        requires std::default_initializable<Iterator> && std::default_initializable<S> &&
                     std::default_initializable<BinaryPredicate>
    = default;

#else

    template<class I = Iterator,
             class = enable_if<std::is_default_constructible<I>::value && std::is_default_constructible<S>::value &&
                               std::is_default_constructible<BinaryPredicate>::value>>
    constexpr duplicates_iterator() {
    }

#endif

    LZ_CONSTEXPR_CXX_14 duplicates_iterator(Iterator begin, S end, BinaryPredicate compare) :
        _last{ std::move(begin) },
        _first{ std::move(_last) },
        _end{ std::move(end) },
        _compare{ std::move(compare) } {
        next();
    }

    LZ_CONSTEXPR_CXX_14 duplicates_iterator& operator=(default_sentinel) {
        _first = _end;
        return *this;
    }

    constexpr reference dereference() const {
        return { *_first, static_cast<std::size_t>(_last_distance) };
    }

    LZ_CONSTEXPR_CXX_17 pointer arrow() const {
        return fake_ptr_proxy<reference>(**this);
    }

    LZ_CONSTEXPR_CXX_14 void increment() {
        _first = std::move(_last);
        next();
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(const duplicates_iterator& other) const {
        LZ_ASSERT(_end == other._end, "Incompatible iterators");
        return _first == other._first;
    }

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool eq(default_sentinel) const {
        return _first == _end;
    }
};
} // namespace detail
} // namespace lz

#endif
