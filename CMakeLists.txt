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

#-------------------------------------------------------------------------------------------------------------------
# Set a default build configuraton if none is specified. 'MinSizeRel' produces the smallest binaries
#-------------------------------------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'MinSizeRel' as none was specified.")
    set(CMAKE_BUILD_TYPE MinSizeRel CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
        "MinSizeRel" "RelWithDebInfo")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)


#--------------------------------------------------------------
# global headers
#--------------------------------------------------------------
file(GLOB_RECURSE headers include/*.hxx)
include_directories(include)

#
# BOOST
#
find_package(Boost  REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
MESSAGE( STATUS ${Boost_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})
MESSAGE( STATUS ${Boost_INCLUDE_DIR})

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
