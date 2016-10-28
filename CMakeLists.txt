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

#
# PYBIND
#
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/pybind11/include")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/pybind11")

#
# MARRAY
#
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/marray/include")

#--------------------------------------------------------------
# add python to project tree
#--------------------------------------------------------------
add_subdirectory(python) 
