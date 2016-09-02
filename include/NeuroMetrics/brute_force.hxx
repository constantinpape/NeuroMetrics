#pragma once

#include <map>
#include <utility> // pair
#include <iterator> // iterator_traits
#include <cmath> // log
#include <stdexcept> // runtime_error

// brute force code for unit tests
template<class ITERATOR_0, class ITERATOR_1>
    size_t matchingPairsBruteForce(ITERATOR_0, ITERATOR_0, ITERATOR_1, const bool = false);
template<class ITERATOR_0, class ITERATOR_1>
    size_t matchingPairsBruteForce(ITERATOR_0, ITERATOR_0, ITERATOR_1, const bool, size_t&);
template<class ITERATOR_0, class ITERATOR_1>
    double randIndexBruteForce(ITERATOR_0, ITERATOR_0, ITERATOR_1, const bool = false);

template<class ITERATOR_0, class ITERATOR_1>
inline size_t
matchingPairsBruteForce
(
    ITERATOR_0 begin0,
    ITERATOR_0 end0,
    ITERATOR_1 begin1,
    const bool ignoreDefaultLabel
)
{
    size_t N;
    return matchingPairsBruteForce(begin0, end0, begin1, ignoreDefaultLabel, N);
}

template<class ITERATOR_0, class ITERATOR_1>
size_t
matchingPairsBruteForce
(
    ITERATOR_0 begin0,
    ITERATOR_0 end0,
    ITERATOR_1 begin1,
    const bool ignoreDefaultLabel,
    size_t& N // output: number of elements which have a non-zero label in both partitions
)
{
    typedef typename std::iterator_traits<ITERATOR_0>::value_type Label0;
    typedef typename std::iterator_traits<ITERATOR_1>::value_type Label1;

    size_t AB = 0;
    if(ignoreDefaultLabel) {
        N = 0;
        ITERATOR_1 it1 = begin1;
        for(ITERATOR_0 it0 = begin0; it0 != end0; ++it0, ++it1) {
            if(*it0 != Label0() && *it1 != Label1()) {
                ++N;
                ITERATOR_1 it1b = it1 + 1;
                for(ITERATOR_0 it0b = it0 + 1; it0b != end0; ++it0b, ++it1b) {
                    if(*it0b != Label0() && *it1b != Label1()) {
                        if((*it0 == *it0b && *it1 == *it1b) || (*it0 != *it0b && *it1 != *it1b)) {
                            ++AB;
                        }
                    }
                }
            }
        }
    }
    else {
        N = std::distance(begin0, end0);
        ITERATOR_1 it1 = begin1;
        for(ITERATOR_0 it0 = begin0; it0 != end0; ++it0, ++it1) {
            ITERATOR_1 it1b = it1 + 1;
            for(ITERATOR_0 it0b = it0 + 1; it0b != end0; ++it0b, ++it1b) {
                if( (*it0 == *it0b && *it1 == *it1b) || (*it0 != *it0b && *it1 != *it1b) ) {
                    ++AB;
                }
            }
        }

    }
    return AB;
}



template<class ITERATOR_0, class ITERATOR_1>
inline double
randIndexBruteForce
(
    ITERATOR_0 begin0,
    ITERATOR_0 end0,
    ITERATOR_1 begin1,
    const bool ignoreDefaultLabel
)
{
    size_t N;
    const size_t n = matchingPairsBruteForce(begin0, end0, begin1, ignoreDefaultLabel, N);
    if(N == 0) {
        throw std::runtime_error("No element is labeled in both partitions.");
    }
    else {
        return static_cast<double>(n) * 2 / static_cast<double>(N * (N-1));
    }
}
