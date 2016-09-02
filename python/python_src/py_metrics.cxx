// boost python related
#include <boost/python.hpp>

#include <numpy/arrayobject.h>
#include <numpy/noprefix.h>

// vigra numpy array converters
#include <vigra/numpy_array.hxx>
#include <vigra/numpy_array_converters.hxx>

#include <NeuroMetrics/metrics.hxx>

namespace bp = boost::python;

namespace neurometrics {

template<class T>
class PyNeuroMetrics : public NeuroMetrics {
    
    typedef NeuroMetrics Base;

public:
    void computeContingecyTable(
        vigra::NumpyArray<1, T> segA,
        vigra::NumpyArray<1, T> segB)
    {
        Base::computeContingecyTable(segA.begin(), segA.end(), segB.begin(), segB.end());
    }

    double randIndex() {
        return Base::randIndex();
    }
    double randScore() {
        return Base::randScore();
    }
    double randPrecision() {
        return Base::randPrecision();
    }
    double randRecall() {
        return Base::randRecall();
    }
    
    double variationOfInformation() {
        return Base::variationOfInformation();
    }
    double viScore() {
        return Base::viScore();
    }
    double viPrecision() {
        return Base::viPrecision();
    }
    double viRecall() {
        return Base::viRecall();
    }

};


template<class T>
void exportMetricsT(){

    typedef PyNeuroMetrics<T> Metrics;

    bp::class_<Metrics>("Metrics")
        .def("computeContingencyTable",vigra::registerConverters(&Metrics::computeContingecyTable))
        .def("randIndex", &Metrics::randIndex)
        .def("randScore", &Metrics::randScore)
        .def("randPrecision", &Metrics::randPrecision)
        .def("randRecall", &Metrics::randRecall)
        .def("variationOfInformation", &Metrics::variationOfInformation)
        .def("viScore", &Metrics::viScore)
        .def("viPrecision", &Metrics::viPrecision)
        .def("viRecall", &Metrics::viRecall)
    ;
        
}

} // namespace neurometrics


BOOST_PYTHON_MODULE(NeuroMetrics) {

    // Do not change next 4 lines
    import_array(); 
    vigra::import_vigranumpy();
    boost::python::numeric::array::set_module_and_type("numpy", "ndarray");
    boost::python::docstring_options docstringOptions(true,true,false);
    // No not change 4 line above
    
    // FIXME overloading does not work!
    //neurometrics::exportMetricsT<vigra::UInt16>();
    neurometrics::exportMetricsT<vigra::UInt32>();
    //neurometrics::exportMetricsT<vigra::UInt64>();
}

