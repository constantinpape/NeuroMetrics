// copied from nifty
#pragma once

#include "NeuroMetrics/tools/threadpool.hxx"

namespace neurometrics{
namespace tools{


template<class T>
class BlockStorage{
public:
    typedef andres::Marray<T> ArrayType;
    typedef andres::View<T> ViewType;
    template<class SHAPE>
    BlockStorage(
        const SHAPE & maxShape,  
        const std::size_t numberOfBlocks
    )
    :   arrayVec_(numberOfBlocks, ArrayType(maxShape.begin(), maxShape.end())){
        std::fill(zeroCoord_.begin(), zeroCoord_.end(), 0);
    }

    template<class SHAPE>
    BlockStorage(
        ThreadPool & threadpool,
        const SHAPE & maxShape,  
        const std::size_t numberOfBlocks
    )
    :   zeroCoord_(maxShape.size(),0),
        arrayVec_(numberOfBlocks, ArrayType(maxShape.begin(), maxShape.end())){
    }

    template<class SHAPE>
    ViewType getView(const SHAPE & shape, const std::size_t blockIndex) {
        return arrayVec_[blockIndex].view(zeroCoord_.begin(), shape.begin());
    }

    ViewType getView(const std::size_t blockIndex) {
        return static_cast<ViewType & >(arrayVec_[blockIndex]);
    }

private:
    std::vector<uint64_t> zeroCoord_;
    std::vector<ArrayType> arrayVec_;
};

template<class T>
class BlockView{
public:
    typedef andres::View<T> ViewType;


    template<class SHAPE>
    BlockView(
        const SHAPE & maxShape,  
        const std::size_t numberOfBlocks
    ){

    }

    template<class SHAPE>
    BlockView(
        ThreadPool & threadpool,
        const SHAPE & maxShape,  
        const std::size_t numberOfBlocks
    ){

    }


    ViewType getView(const std::size_t blockIndex) {
       return ViewType();
    }

    template<class SHAPE>
    ViewType getView(const SHAPE & shape, const std::size_t blockIndex) {
        return ViewType();
    }

private:
    //std::vector<ViewType> viewVec_;
};

template<class ARRAY>
struct BlockStorageSelector;


template<class T, bool C, class A>
struct BlockStorageSelector<andres::View<T, C, A> >
{
   typedef BlockView<T> type;
};

template<class T, class A>
struct BlockStorageSelector<andres::Marray<T, A> >
{
   typedef BlockView<T> type;
};


} // end namespace tools
} // end namespace neurometrics
