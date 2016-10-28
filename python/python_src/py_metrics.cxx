#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <NeuroMetrics/metrics.hxx>
#include <NeuroMetrics/converter.hxx>

namespace py = pybind11;

namespace neurometrics {


template<class T>
void exportMetricsT(py::module & metricsModule){

    typedef NeuroMetrics Metrics;

    py::class_<Metrics>(metricsModule,"Metrics")
        .def(py::init<>())
        .def("computeContingencyTable",[](Metrics & self, 
            andres::PyView<T,1> segA,
            andres::PyView<T,1> segB){
                {
                    py::gil_scoped_release allowThreads;
                    self.computeContingecyTable(segA.begin(), segA.end(), segB.begin(), segB.end());
                }
        })
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

void exportMetrics(py::module & metricsModule) {

    //neurometrics::exportMetricsT<uint16_t>(metricsModule);
    neurometrics::exportMetricsT<uint32_t>(metricsModule);
    //neurometrics::exportMetricsT<uint64_t>(metricsModule);
}


} // namespace neurometrics
