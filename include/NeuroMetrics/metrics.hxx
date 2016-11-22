#pragma once

#include <vector>
#include <iterator>
#include <cmath>
#include <stdexcept>

#include <andres/marray.hxx>

#include "NeuroMetrics/tools/for_each_coordinate.hxx"
#include "NeuroMetrics/tools/parallel_max_element.hxx"

namespace neurometrics {

// bundle all metrics that can be computed from the contingency table
// -> need to calculate it only once
// TODO ignore label 0 for gt is hardcoded, make it accessible!
template<unsigned DIM, class T>
class NeuroMetrics {

public:
    
    typedef andres::Marray<double> ContingencyTable;
    
    // constructor
    NeuroMetrics();

    // interface
    
    // compute contingency table
    void computeContingecyTable(const andres::View<T> &, const andres::View<T> &);
    // parallel version, not debugged yet! hence not exposed to python
    void computeContingecyTable(const andres::View<T> &, const andres::View<T> &, const int);

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

template<unsigned DIM, class T>
NeuroMetrics<DIM,T>::NeuroMetrics()
    : hasContingencyTable(false), hasRandPrimitives(false), hasViPrimitives(false),
    n(0), contingencyTable(),
    rowSum(), colSum(),
    randA(0), randB(0), randAB(0),
    viA(0), viB(0), viAB(0)
{}

template<unsigned DIM, class T>
void NeuroMetrics<DIM,T>::computeContingecyTable(
        const andres::View<T> & segA,
        const andres::View<T> & segB
        )
{

    typedef std::array<int64_t,DIM> Coord;
    
    n = 1;
    Coord shape;
    for(size_t d = 0; d < DIM; ++d) {
        shape[d] = segA.shape(d);
        n *= shape[d];
    }

    size_t nLabelsA = *( std::max_element( segA.begin(), segA.end() ) ) + 1;
    size_t nLabelsB = *( std::max_element( segB.begin(), segB.end() ) ) + 1;

    // init the contingency matrix 
    size_t contingencyShape[] = {nLabelsA,nLabelsB};
    contingencyTable.resize(contingencyShape, contingencyShape+2, 0.);
    
    // compute the contingency matrix
    tools::forEachCoordinate(shape, [&](const Coord & coord){
        T labelA = segA(coord.begin());        
        T labelB = segB(coord.begin());        
        ++contingencyTable(labelA,labelB);
    });

    // compute the sum of rows
    rowSum.assign( contingencyTable.shape(0), 0.);
    for( size_t i = 1; i < rowSum.size(); ++i )
    {
        for( size_t j = 0; j < contingencyTable.shape(1); ++j )
        {
            rowSum[i] += contingencyTable(i,j);
        }
    }
    
    // compute the sum of cols
    colSum.assign(contingencyTable.shape(1), 0.);
    for( size_t j = 1; j < colSum.size(); ++j )
    {
        for( size_t i = 1; i < contingencyTable.shape(0); ++i )
        {
            colSum[j] += contingencyTable(i,j);
        }
    }

    hasContingencyTable = true;
}

    
template<unsigned DIM, class T>
void NeuroMetrics<DIM,T>::computeContingecyTable(
        const andres::View<T> & segA,
        const andres::View<T> & segB,
        const int numberOfThreads
        )
{

    typedef std::array<int64_t,DIM> Coord;
    
    n = 1;
    Coord shape;
    for(size_t d = 0; d < DIM; ++d) {
        shape[d] = segA.shape(d);
        n *= shape[d];
    }

    auto popt = tools::ParallelOptions(numberOfThreads);
    tools::ThreadPool threadpool(popt);
    size_t actualNumThreads = threadpool.nThreads();

    // max element is super slow here for some reason...
    T nLabelsA = tools::parallelMax<DIM>(segA, threadpool)+1; 
    T nLabelsB = tools::parallelMax<DIM>(segB, threadpool)+1; 

    // init the contingency matrix 
    size_t contingencyShape[] = {nLabelsA,nLabelsB};
    contingencyTable.resize(contingencyShape, contingencyShape+2, 0.);
    
    // parallel
    std::vector<ContingencyTable> cTableThreadVec(actualNumThreads);
    tools::parallel_foreach(threadpool, actualNumThreads,[&](int tid, int i){
        cTableThreadVec[tid].resize(contingencyShape, contingencyShape+2, 0.);   
    });

    auto checkCoord = [&](const Coord & coordinate) {
        for(int d = 0; d < DIM; ++d) {
            if( coordinate[d] < 0 || coordinate[d] > shape[d]) {
                std::cout << "DIM " << d << " out of range with " << coordinate[d] << std::endl;
                throw std::runtime_error("Coordinate out of range");
            }
        }
    };

    tools::parallelForEachCoordinate(threadpool, shape, [&](const int tid, const Coord & coord){
        checkCoord(coord);
        auto & cTable = cTableThreadVec[tid];
        T labelA = segA(coord.begin());
        T labelB = segB(coord.begin());
        ++cTable(labelA,labelB);
    });
    std::cout << "After Ctable" << std::endl;

    for(int tid = 0; tid < actualNumThreads; ++tid)
        contingencyTable += cTableThreadVec[tid];
    std::cout << "After merge" << std::endl;

    
    // compute the sum of rows
    rowSum.assign( contingencyTable.shape(0), 0.);
    for( size_t i = 1; i < rowSum.size(); ++i )
    {
        for( size_t j = 0; j < contingencyTable.shape(1); ++j )
        {
            rowSum[i] += contingencyTable(i,j);
        }
    }
    
    // compute the sum of cols
    colSum.assign(contingencyTable.shape(1), 0.);
    for( size_t j = 1; j < colSum.size(); ++j )
    {
        for( size_t i = 1; i < contingencyTable.shape(0); ++i )
        {
            colSum[j] += contingencyTable(i,j);
        }
    }

    hasContingencyTable = true;
}


template<unsigned DIM, class T>
void NeuroMetrics<DIM,T>::computeRandPrimitives()
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


template<unsigned DIM, class T>
double NeuroMetrics<DIM,T>::randIndex()
{
    if(!hasContingencyTable)
        throw std::runtime_error("Need to call computeContingencyTable first");
    if (!hasRandPrimitives)
        computeRandPrimitives();
    return 1. - (randA + randB - 2.*randAB) / (n*n);
}


template<unsigned DIM, class T>
double NeuroMetrics<DIM,T>::randPrecision()
{
    if(!hasContingencyTable)
        throw std::runtime_error("Need to call computeContingencyTable first");
    if (!hasRandPrimitives)
        computeRandPrimitives();
    return randAB / randB;
}


template<unsigned DIM, class T>
double NeuroMetrics<DIM,T>::randRecall()
{
    if(!hasContingencyTable)
        throw std::runtime_error("Need to call computeContingencyTable first");
    if (!hasRandPrimitives)
        computeRandPrimitives();
    return randAB / randA;
}

    
template<unsigned DIM, class T>
double NeuroMetrics<DIM,T>::randScore()
{
    if(!hasContingencyTable)
        throw std::runtime_error("Need to call computeContingencyTable first");
    double prec = randPrecision();
    double rec  = randRecall();
    return 2.0 * prec * rec / (prec + rec );
}


template<unsigned DIM, class T>
void NeuroMetrics<DIM,T>::computeViPrimitives() {
    
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

template<unsigned DIM, class T>
double NeuroMetrics<DIM,T>::variationOfInformation()
{
    if(!hasContingencyTable)
        throw std::runtime_error("Need to call computeContingencyTable first");
    if(!hasViPrimitives)
        computeViPrimitives();
    return viA + viB - 2. * viAB;
}

template<unsigned DIM, class T>
double NeuroMetrics<DIM,T>::viPrecision()
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

template<unsigned DIM, class T>
double NeuroMetrics<DIM,T>::viRecall()
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

template<unsigned DIM, class T>
double NeuroMetrics<DIM,T>::viScore()
{
    double prec = viPrecision();
    double rec = viRecall();
    return 2. * prec * rec / (prec + rec);
}
    

} // namespace neurometrics
