#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace neurometrics {

    void exportMetrics(py::module &);

}


PYBIND11_MODULE(_NeuroMetrics, metricsModule) {
    metricsModule.doc() = "Pythonbindings for Neuro Metrics.";

    using namespace neurometrics;
    exportMetrics(metricsModule);

}
