#pragma once

#include <vector>
#include <iterator>
#include <cmath>
#include <stdexcept>

#include <andres/marray.hxx>

namespace neurometrics {

// bundle all metrics that can be computed from the contingency table
// -> need to calculate it only once
// TODO ignore label 0 for gt is hardcoded, make it accessible!
class NeuroMetrics {

public:
    
    // TODO use marray here
    //typedef std::vector<std::vector<double>> ContingencyTable;
    typedef andres::Marray<double> ContingencyTable;
    
    // constructor
    NeuroMetrics();

    // interface
    
    // compute contingency table
    template<class ITERATOR_0, class ITERATOR_1>
    void computeContingecyTable(ITERATOR_0, ITERATOR_0, ITERATOR_1, ITERATOR_1);

    // rand measures
    // implementations adapted from
    // https://github.com/fiji/Trainable_Segmentation/blob/master/src/main/java/trainableSegmentation/metrics/RandError.java
    // adaptedRandIndexStats3DN2
    double randIndex();
    double randPrecision();
    double randRecall();
    double randScore();

    // vi measures
    // implentations adapted from:
    // https://github.com/fiji/Trainable_Segmentation/blob/master/src/main/java/trainableSegmentation/metrics/VariationOfInformation.java
    // foregroundRestrictedFscore
    double variationOfInformation();
    double viPrecision();
    double viRecall();
    double viScore();

private:
    
    // compute rand primitives
    void computeRandPrimitives();

    // compute vi primitives
    void computeViPrimitives();
    
    // flags to keep track of things that were already computed
    bool hasContingencyTable;
    bool hasRandPrimitives;
    bool hasViPrimitives;
    
    // contigency table and stuff
    size_t n;
    ContingencyTable contingencyTable;
    std::vector<double> rowSum;
    std::vector<double> colSum;

    // rand primitives
    double randA; // quadratic sum of row sums
    double randB; // quadratic sum of col sums
    double randAB;// quadratic sum of contingency table entries

    // vi primitives
    double viA; 
    double viB; 
    double viAB;
};


// implementation

NeuroMetrics::NeuroMetrics()
    : hasContingencyTable(false), hasRandPrimitives(false), hasViPrimitives(false),
    n(0), contingencyTable(),
    rowSum(), colSum(),
    randA(0), randB(0), randAB(0),
    viA(0), viB(0), viAB(0)
{}

template<class ITERATOR_0, class ITERATOR_1>
void NeuroMetrics::computeContingecyTable(
        ITERATOR_0 segABegin,
        ITERATOR_0 segAEnd,
        ITERATOR_1 segBBegin,
        ITERATOR_1 segBEnd)
{
    // typedefs
    typedef typename std::iterator_traits<ITERATOR_0>::value_type Label0;
    typedef typename std::iterator_traits<ITERATOR_1>::value_type Label1;
    
    n = std::distance(segABegin, segAEnd);

    if ( n != std::distance(segBBegin, segBEnd) )
        throw std::runtime_error("Segmentation sizes do not match!");

    size_t nLabelsA = *( std::max_element( segABegin, segAEnd ) ) + 1;
    size_t nLabelsB = *( std::max_element( segBBegin, segBEnd ) ) + 1;

    // init the contingency matrix 
    size_t shape[] = {nLabelsA,nLabelsB};
    contingencyTable.resize(shape, shape+2, 0.);

    // compute the contingency matrix
    ITERATOR_0 segA_it = segABegin;
    ITERATOR_1 segB_it = segBBegin;
    for( ; segA_it != segAEnd; segA_it++, segB_it++ )
    {
        Label0 i = *( segA_it );
        Label1 j = *( segB_it );
        contingencyTable(i,j)++;
    }
    
    // compute the sum of rows
    rowSum.assign( contingencyTable.shape(0), 0.);
    for( size_t i = 1; i < rowSum.size(); i++ )
    {
        for( size_t j = 0; j < contingencyTable.shape(1); j++ )
        {
            rowSum[i] += contingencyTable(i,j);
        }
    }
    
    // compute the sum of cols
    colSum.assign(contingencyTable.shape(1), 0.);
    for( size_t j = 1; j < colSum.size(); j++ )
    {
        for( size_t i = 1; i < contingencyTable.shape(0); i++ )
        {
            colSum[j] += contingencyTable(i,j);
        }
    }

    hasContingencyTable = true;
}


void NeuroMetrics::computeRandPrimitives()
{
    double aux = 0.;
    for( size_t i = 1; i < contingencyTable.shape(0) ; i++)
    {
        aux += contingencyTable(i,0);
    }

    // sum of square of rows
    for( size_t i = 0; i < rowSum.size(); i++ )
    {
        randA += rowSum[i] * rowSum[i];
    }
    
    // sum of square of cols
    for( size_t j = 0; j < colSum.size(); j++ )
    {
        randB += colSum[j] * colSum[j];
    }

    randB += aux / n;

    for( size_t i = 1; i < contingencyTable.shape(0); i++ )
    {
        for( size_t j = 1; j < contingencyTable.shape(1); j++ )
        {
            randAB += contingencyTable(i,j) * contingencyTable(i,j);
        }
    }

    randAB += aux / n;
    
    hasRandPrimitives = true;
}


double NeuroMetrics::randIndex()
{
    if(!hasContingencyTable)
        throw std::runtime_error("Need to call computeContingencyTable first");
    if (!hasRandPrimitives)
        computeRandPrimitives();
    return 1. - (randA + randB - 2.*randAB) / (n*n);
}


double NeuroMetrics::randPrecision()
{
    if(!hasContingencyTable)
        throw std::runtime_error("Need to call computeContingencyTable first");
    if (!hasRandPrimitives)
        computeRandPrimitives();
    return randAB / randB;
}


double NeuroMetrics::randRecall()
{
    if(!hasContingencyTable)
        throw std::runtime_error("Need to call computeContingencyTable first");
    if (!hasRandPrimitives)
        computeRandPrimitives();
    return randAB / randA;
}

    
double NeuroMetrics::randScore()
{
    if(!hasContingencyTable)
        throw std::runtime_error("Need to call computeContingencyTable first");
    double prec = randPrecision();
    double rec  = randRecall();
    return 2.0 * prec * rec / (prec + rec );
}


void NeuroMetrics::computeViPrimitives() {
    
    double aux = 0.;
    for( size_t i = 1; i < contingencyTable.shape(0) ; i++)
    {
        aux += contingencyTable(i,0);
    }
    aux /= n;

    // sum of square of rows
    for( size_t i = 0; i < rowSum.size(); i++ )
    {
        if( (rowSum[i] / n) != 0)
            viA += (rowSum[i] / n) * log( (rowSum[i] / n) );
    }
    
    // sum of square of cols
    for( size_t j = 0; j < colSum.size(); j++ )
    {
        if( (colSum[j] / n) != 0)
            viB += (colSum[j] / n) * log( (colSum[j] / n) );
    }

    viB -= aux * log(n);

    for( size_t i = 1; i < contingencyTable.shape(0); i++ )
    {
        for( size_t j = 1; j < contingencyTable.shape(1); j++ )
        {
            if( (contingencyTable(i,j) / n) != 0)
                viAB += (contingencyTable(i,j) / n) * log(contingencyTable(i,j) / n);
        }
    }

    viAB -= aux / log(n);

    hasViPrimitives = true;
}

double NeuroMetrics::variationOfInformation()
{
    if(!hasContingencyTable)
        throw std::runtime_error("Need to call computeContingencyTable first");
    if(!hasViPrimitives)
        computeViPrimitives();
    return viA + viB - 2. * viAB;
}

double NeuroMetrics::viPrecision()
{
    if(!hasContingencyTable)
        throw std::runtime_error("Need to call computeContingencyTable first");
    if(!hasViPrimitives)
        computeViPrimitives();
    if(viA == 0.) {
        return 0.;
    }
    if(viB == 0.) {
        return 1.;
    }
    return ( viA + viB - viAB) / viA;
}

double NeuroMetrics::viRecall()
{
    if(!hasContingencyTable)
        throw std::runtime_error("Need to call computeContingencyTable first");
    if(!hasViPrimitives)
        computeViPrimitives();
    if(viA == 0.) {
        return 1.;
    }
    if(viB == 0.) {
        return 0.;
    }
    return (viB + viA - viAB) / viB;
}

double NeuroMetrics::viScore()
{
    double prec = viPrecision();
    double rec = viRecall();
    return 2. * prec * rec / (prec + rec);
}
    

} // namespace neurometrics
