#pragma once

#include <andres/marray.hxx>

namespace neurometrics {
namespace tools {

    template<class T, class COORD>
    inline void readSubarray(
        const andres::View<T> array,
        const COORD & begin,
        const COORD & end,
        andres::View<T> & subarray) {

        const auto dim = array.dimension();
        std::vector<int64_t> subShape(dim);
        for( int d = 0; d < dim; ++d)
            subShape[d] = begin[d] - end[d];
        subarray = array.view(begin.begin(), subShape.end());
    }

}
}
