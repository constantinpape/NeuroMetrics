#--------------------------------------------------------------
# NeuroMetrics
#--------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.3)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
include(CMakeCopyIfDifferent)
project(NeuroMetrics)

#--------------------------------------------------------------
# c++ 11
#--------------------------------------------------------------
SET(CMAKE_CXX_FLAGS "-std=c++0x")

#--------------------------------------------------------------
# global headers
#--------------------------------------------------------------
file(GLOB_RECURSE headers include/*.hxx)
include_directories(include)

#--------------------------------------------------------------
# vigra
#--------------------------------------------------------------
FIND_PACKAGE(VIGRA REQUIRED)
include_directories(${VIGRA_INCLUDE_DIR})

#--------------------------------------------------------------
# numpy
#--------------------------------------------------------------
FIND_PACKAGE(NUMPY REQUIRED)
include_directories(${PYTHON_NUMPY_INCLUDE_DIR})

#--------------------------------------------------------------
# Boost and Python
#--------------------------------------------------------------
FIND_PACKAGE(PythonInterp)
FIND_PACKAGE(PythonLibs)
FIND_PACKAGE(Boost COMPONENTS python)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
LINK_LIBRARIES(${Boost_LIBRARIES} ${PYTHON_LIBRARIES}) # Deprecated but so convenient!

#--------------------------------------------------------------
# add python to project tree
#--------------------------------------------------------------
add_subdirectory(python) 
