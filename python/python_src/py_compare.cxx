// boost python related
#include <boost/python.hpp>

#include <numpy/arrayobject.h>
#include <numpy/noprefix.h>

// vigra numpy array converters
#include <vigra/numpy_array.hxx>
#include <vigra/numpy_array_converters.hxx>

#include <NeuroMetrics/partition-comparison.hxx>
#include <NeuroMetrics/rand-score.hxx>

namespace bp = boost::python;


template<class T>
double pyRandIndex(
    vigra::NumpyArray<1, UInt32> gt,
    vigra::NumpyArray<1, UInt32> seg,
    const bool ignoreDefaultLabel 
){
    return andres::randIndex(gt.begin(),gt.end(),seg.begin(),ignoreDefaultLabel);
}

template<class T>
double pyVariationOfInformation(
    vigra::NumpyArray<1, UInt32> gt,
    vigra::NumpyArray<1, UInt32> seg,
    const bool ignoreDefaultLabel 
){
    return andres::variationOfInformation(gt.begin(),gt.end(),seg.begin(),ignoreDefaultLabel);
}

template <class T>
vigra::NumpyArray<1, double> pyFscore(
    vigra::NumpyArray<1, T> segA,
    vigra::NumpyArray<1, T> segB)
{
    vigra::NumpyArray<1, double> ret( vigra::NumpyArray<1, double>::difference_type(2) );
    std::pair<double, double> res = fscore( segA.begin(), segA.end(), segB.begin(), segB.end() );
    ret[0] = res.first;
    ret[1] = res.second;
    return ret;
}


template<class T>
void export_metrics_t(){

    bp::def("randIndex",vigra::registerConverters(&pyRandIndex<T>),
        (
            bp::arg("a"),
            bp::arg("b"),
            bp::arg("ignoreDefaultLabel")
        )
    );

    bp::def("variationOfInformation",vigra::registerConverters(&pyVariationOfInformation<T>),
        (
            bp::arg("a"),
            bp::arg("b"),
            bp::arg("ignoreDefaultLabel")
        )
    );
    
    bp::def("randFScore",vigra::registerConverters(&pyFscore<T>),
        (
            bp::arg("segA"),
            bp::arg("segB")
        )
    );
}



void export_metrics(){

  export_metrics_t<vigra::UInt32>();
  export_metrics_t<vigra::UInt16>();
}

BOOST_PYTHON_MODULE(NeuroMetrics) {

    // Do not change next 4 lines
    import_array(); 
    vigra::import_vigranumpy();
    boost::python::numeric::array::set_module_and_type("numpy", "ndarray");
    boost::python::docstring_options docstringOptions(true,true,false);
    // No not change 4 line above
    
    export_metrics();
}

