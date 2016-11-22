#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <string>

#include "NeuroMetrics/metrics.hxx"
#include "NeuroMetrics/converter.hxx"

namespace py = pybind11;

namespace neurometrics {


template<unsigned DIM, class T>
void exportMetricsT(py::module & metricsModule, std::string & cls_name){

    typedef NeuroMetrics<DIM,T> Metrics;

    py::class_<Metrics>(metricsModule,cls_name.c_str())
        .def(py::init<>())
        .def("computeContingencyTable",[](Metrics & self, 
            andres::PyView<T,DIM> segA,
            andres::PyView<T,DIM> segB){
                {
                    py::gil_scoped_release allowThreads;
                    self.computeContingecyTable(segA, segB);
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
    std::string name = "Metrics1dUInt32";
    neurometrics::exportMetricsT<1,uint32_t>(metricsModule,name);
    name = "Metrics2dUInt32";
    neurometrics::exportMetricsT<2,uint32_t>(metricsModule,name);
    name = "Metrics3dUInt32";
    neurometrics::exportMetricsT<3,uint32_t>(metricsModule,name);
    
    name = "Metrics1dUInt64";
    neurometrics::exportMetricsT<1,uint64_t>(metricsModule,name);
    name = "Metrics2dUInt64";
    neurometrics::exportMetricsT<2,uint64_t>(metricsModule,name);
    name = "Metrics3dUInt64";
    neurometrics::exportMetricsT<3,uint64_t>(metricsModule,name);
}


} // namespace neurometrics
