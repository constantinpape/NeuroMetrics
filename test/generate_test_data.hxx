// TODO refactor this to some other place

#include <array>
#include <random>

#include <andres/marray.hxx>

template<class T, unsigned DIM>
andres::Marray<T> generateTestSegmentation(const std::array<size_t,DIM> & shape,
        const T segRange,
        const size_t maxSegmentLen) {
   
    andres::Marray<T> ret(shape.begin(),shape.end());

    std::default_random_engine generator;
    
    std::uniform_int_distribution<T> valueDistribution(0,segRange);
    auto drawSegVal = std::bind(valueDistribution,generator);
    
    std::uniform_int_distribution<size_t> lenDistribution(1,maxSegmentLen);
    auto drawLenVal = std::bind(lenDistribution,generator);

    auto it = ret.begin();
    while(it != ret.end()) {
        
        auto len = drawLenVal();
        auto val = drawSegVal();

        for(size_t i = 0; i < len; ++i) {
            *it = val;
            ++it;
            if it == ret.end()
                break;
        }
    }
    return ret;
}
