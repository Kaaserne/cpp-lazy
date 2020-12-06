#pragma once

#ifndef LZ_LEFT_JOIN_HPP
#define LZ_LEFT_JOIN_HPP

#include "detail/BasicIteratorView.hpp"
#include "detail/LeftJoinIterator.hpp"

namespace lz {
#ifdef LZ_HAS_CXX_17
    template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector, class Execution>
    class LeftJoin :
        public internal::BasicIteratorView<internal::LeftJoinIterator<IterA, IterB, SelectorA, SelectorB, ResultSelector,Execution>> {

    public:
        using iterator = internal::LeftJoinIterator<IterA, IterB, SelectorA, SelectorB, ResultSelector, Execution>;
#else
    template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector>
    class LeftJoin : public internal::BasicIteratorView<internal::LeftJoinIterator<IterA, IterB, SelectorA, SelectorB, ResultSelector>> {

    public:
        using iterator = internal::LeftJoinIterator<IterA, IterB, SelectorA, SelectorB, ResultSelector>;
#endif
        using const_iterator = iterator;
        using value_type = typename iterator::value_type;

    private:
        iterator _begin{};
        iterator _end{};

    public:
        LeftJoin() = default;

#ifdef LZ_HAS_CXX_17
        LeftJoin(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b, ResultSelector resultSelector,
                 Execution execution):
            internal::BasicIteratorView<iterator>(iterator(iterA, endA, iterB, endB, a, b, resultSelector, execution),
                                                  iterator(endA, endA, endB, endB, a, b, resultSelector, execution))
        {}
#else
        LeftJoin(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b, ResultSelector resultSelector):
            internal::BasicIteratorView<iterator>(iterator(iterA, endA, iterB, endB, a, b, resultSelector),
                                                  iterator(endA, endA, endB, endB, a, b, resultSelector))
        {}
#endif
    };


#ifdef LZ_HAS_CXX_17
    template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector,
        class Execution = std::execution::sequenced_policy>
    LeftJoin<IterA, IterB, SelectorA, SelectorB, ResultSelector, Execution>
    leftJoin(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b, ResultSelector resultSelector,
             Execution execution = std::execution::seq) {
        return LeftJoin<IterA, IterB, SelectorA, SelectorB, ResultSelector, Execution>(
            std::move(iterA), std::move(endA), std::move(iterB), std::move(endB), std::move(a), std::move(b), std::move(resultSelector),
            execution);
    }

    template<class IterableA, class IterableB, class SelectorA, class SelectorB, class ResultSelector,
        class Execution = std::execution::sequenced_policy>
    LeftJoin<internal::IterTypeFromIterable<IterableA>, internal::IterTypeFromIterable<IterableB>,
        SelectorA, SelectorB, ResultSelector, Execution>
    leftJoin(IterableA&& iterableA, IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector,
             Execution execution = std::execution::seq) {
        return leftJoin(internal::begin(std::forward<IterableA>(iterableA)), internal::end(std::forward<IterableA>(iterableA)),
                        internal::begin(std::forward<IterableB>(iterableB)), internal::end(std::forward<IterableB>(iterableB)),
                        std::move(a), std::move(b), std::move(resultSelector), execution);
    }
#else // ^^^ lz has cxx 17 vvv ! has cxx 17
    template<class IterA, class IterB, class SelectorA, class SelectorB, class ResultSelector>
    LeftJoin<IterA, IterB, SelectorA, SelectorB, ResultSelector>
    leftJoin(IterA iterA, IterA endA, IterB iterB, IterB endB, SelectorA a, SelectorB b, ResultSelector resultSelector) {
        return LeftJoin<IterA, IterB, SelectorA, SelectorB, ResultSelector>(
            std::move(iterA), std::move(endA), std::move(iterB), std::move(endB), std::move(a), std::move(b), std::move(resultSelector));
    }

    template<class IterableA, class IterableB, class SelectorA, class SelectorB, class ResultSelector>
    LeftJoin<internal::IterTypeFromIterable<IterableA>, internal::IterTypeFromIterable<IterableB>, SelectorA, SelectorB, ResultSelector>
    leftJoin(IterableA&& iterableA, IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector) {
        return leftJoin(internal::begin(std::forward<IterableA>(iterableA)), internal::end(std::forward<IterableA>(iterableA)),
                        internal::begin(std::forward<IterableB>(iterableB)), internal::end(std::forward<IterableB>(iterableB)),
                        std::move(a), std::move(b), std::move(resultSelector));
    }
#endif

}
#endif // LZ_LEFT_JOIN_HPP
