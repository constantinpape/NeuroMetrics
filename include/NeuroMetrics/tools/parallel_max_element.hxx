#pragma once

#include <andres/marray.hxx>

#include <NeuroMetrics/tools/for_each_coordinate.hxx>

namespace neurometrics{
namespace tools{

    template<unsigned DIM,class T>
    T parallelMax(const andres::View<T> array, ThreadPool & threadpool) {

        typedef std::array<int64_t,DIM> Coord;

        Coord shape;
        for(int d = 0; d < DIM; ++d)
            shape[d] = array.shape(d);
        
        size_t numberOfThreads = threadpool.nThreads();
        std::vector<T> maxThreadVector(numberOfThreads,0);

        parallelForEachCoordinate(threadpool, shape, [&](const int tid, const Coord & coord){
            T & maxThread = maxThreadVector[tid];
            T val = array(coord.begin());
            if( val > maxThread )
                maxThread = val;
        });
        return *std::max_element(maxThreadVector.begin(),maxThreadVector.end());
    }


} // namespace tools
} // namespace neurometrics
