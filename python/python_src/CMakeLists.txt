pybind11_add_module(_NeuroMetrics
    py_module.cxx
    py_metrics.cxx
)

target_link_libraries(_NeuroMetrics ${PYTHON_LIBRARIES})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/python/NeuroMetrics)

ADD_CUSTOM_COMMAND(
    TARGET _NeuroMetrics
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different $<TARGET_FILE:_NeuroMetrics> ${CMAKE_BINARY_DIR}/python/NeuroMetrics/
)

add_custom_target(copy_python__init__ ALL
    COMMAND cmake -E copy_if_different
    ${CMAKE_SOURCE_DIR}/python/python_module/__init__.py
    ${CMAKE_BINARY_DIR}/python/NeuroMetrics/__init__.py)
