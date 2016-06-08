#pragma once

#include <map>
#include <utility> // pair
#include <iterator> // iterator_traits
#include <cmath> // log
#include <stdexcept> // runtime_error

namespace neurometrics {

// bundle all metrics that can be computed from the contingency table
// -> need to calculate it only once
template<class ITERATOR_0, class ITERATOR_1>
class metrics {

// TODO recheck where / if we need all the static casts

public:
    metrics(ITERATOR_0, ITERATOR_0, ITERATOR_1, const bool = true);

    // interface
    
    // rand measures
    double randIndex();
    double randPrecision();
    double randRecall();
    double randScore();

    // vi measures
    double variationOfInformation();
    // TODO
    //double viPrecision();
    //double viRecall();
    //double viScore();

private:
    // compute contingency table
    void computeContingecyTable();

    // compute rand primitives
    void computeRandPrimitives();

    // compute vi primitives
    // TODO coputeViPrimitives();

    // typedefs
    typedef typename std::iterator_traits<ITERATOR_0>::value_type Label0;
    typedef typename std::iterator_traits<ITERATOR_1>::value_type Label1;
    typedef std::pair<Label0, Label1> Pair;
    typedef std::map<Pair, size_t> contingencyTable;
    typedef std::map<Label0, size_t> rowSumMap;
    typedef std::map<Label1, size_t> columnSumMap;

    // private members
    
    // data iterators
    ITERATOR_0 begin0;
    ITERATOR_0 end0;
    ITERATIR_1 begin1;

    // flag for ignore default label 
    bool ignore_default_label;
    
    // flags to keep track of things that were already computed
    bool has_contingency;
    bool has_rand_primitives;
    //bool has_vi_primitives;
    
    // contigency table and stuff
    contingencyTable c_table;
    size_t N;
    rowSumMap row_sum;
    columnSumMap col_sum;

    // rand primitives
    size_t A; // quadratic sum of row sums
    size_t B; // quadratic sum of col sums
    size_t AB;// quadratic sum of contingency table entries

    // vi primitives
    // TODO
}


// implementation

metrics::metrics(ITERATOR_0 it0_0, ITERATOR_0 it0_1, ITERATOR_1 it1_0, const bool ignore)
{
    begin0 = it0_0;
    end0   = it0_1;    
    begin1 = it1_0;
    ignore_default_label = ignore;

    has_contingency = false
    has_rand_primitives = false
    //has_vi_primitives = false
}

void metrics::computeContingecyTable()
{
    typedef typename std::iterator_traits<ITERATOR_0>::value_type Label0;
    typedef typename std::iterator_traits<ITERATOR_1>::value_type Label1;
    
    if(ignore_default_label) {
        N = 0;
        for(; begin0 != end0; ++begin0, ++begin1) {
            if(*begin0 != Label0() && *begin1 != Label1()) {
                ++c_table[Pair(*begin0, *begin1)];
                ++row_sum[*begin0];
                ++col_Sum[*begin1];
                ++N;
            }
        }
    }
    else {
        N = std::distance(begin0, end0);
        for(; begin0 != end0; ++begin0, ++begin1) {
            ++c_table[Pair(*begin0, *begin1)];
            ++row_sum[*begin0];
            ++col_sum[*begin1];
        }
    }
    
    if(N == 0) {
        throw std::runtime_error("No element is labeled in both partitions.");
    }
    
    has_contingency = true;
}


void metrics::computeRandPrimitives()
{
    
    // comute the quadratic sum of row sums
    A = 0;
    for(typename rowSumMap::const_iterator it = row_sum.begin(); it != row_sum.end(); ++it) {
        A += it->second * it->second;
    }
    
    // comute the quadratic sum of col sums
    B = 0;
    for(typename columnSumMap::const_iterator it = col_sum.begin(); it != col_sum.end(); ++it) {
        B += it->second * it->second;
    }

    // compute the quadratic sum of the contingency table
    AB = 0;
    for(typename contingencyTable::const_iterator it = c_table.begin(); it != c_table.end(); ++it) {
        AB += (it->second) * (it->second);
    }
    
    has_rand_primitives = true;
}


double metrics::randIndex()
{
    
    if (!has_contingency)
        computeContingecyTable();
    
    if (!has_rand_primitives)
        computeRandPrimitives();

    return static_cast<double>(A + B) / static_cast<double>(N * (N-1));
    //return 1. - static_cast<double>( A + B - 2.0*AB ) / static_cast<double>(N*N);
}


double metrics::randPrecision()
{
    
    if (!has_contingency)
        computeContingecyTable();
    
    if (!has_rand_primitives)
        computeRandPrimitives();

    return static_cast<double>(AB) / static_cast<double>(B);
}


double metrics::randRecall()
{
    
    if (!has_contingency)
        computeContingecyTable();
    
    if (!has_rand_primitives)
        computeRandPrimitives();

    return static_cast<double>(AB) / static_cast<double>(A);
}

    
double merics::randScore()
{
    double prec = randPrecision();
    double rec  = randRecall();
    
    return 2.0 * prec * rec / (prec + rec );
}


double metrics::variationOfInformation()
{
    typedef std::map<Pair, double> PMatrix;
    typedef std::map<Label0, double> PVector0;
    typedef std::map<Label1, double> PVector1;
    
    typedef std::map<Pair, size_t> contingencyTable;
    typedef std::map<Label0, size_t> rowSumMap;
    typedef std::map<Label1, size_t> columnSumMap;
    
    if (!has_contingency)
        computeContingecyTable();

    // compute information
    double H0 = 0.0;
    for(typename rowSumMap::const_iterator it = row_sum.begin(); it != row_sum.end(); ++it) {
        // TODO do we need static casts here ?
        H0 -= (it->second / N) * std::log( (it->second / N) );
    }
    double H1 = 0.0;
    for(typename columnSumMap::const_iterator it = col_sum.begin(); it != col_sum.end(); ++it) {
        // TODO do we need static casts here ?
        H1 -= (it->second / N) * std::log( (it->second / N) );
    }
    double I = 0.0;
    for(typename contingencyTable::const_iterator it = c_table.begin(); it != c_table.end(); ++it) {
        // TODO do we need static casts here ?
        const Label0 j = it->first.first;
        const Label1 k = it->first.second;
        const double pjk_here = it->second / N;
        const double pj_here = row_sum[j] / N;
        const double pk_here = col_sum[k] / N;
        I += pjk_here * std::log( pjk_here / (pj_here * pk_here) );
    }

    return H0 + H1 - 2.0 * I;
}

} // namespace neurometrics
