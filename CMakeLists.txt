cmake_minimum_required(VERSION 2.8)

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_GREATER  3.1)
cmake_policy(SET CMP0054 OLD)
endif("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_GREATER  3.1)

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_GREATER  3.0)
cmake_policy(SET CMP0022 NEW)
endif("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_GREATER  3.0)

include("cmake/yong_injector/injector.cmake")
include("cmake/yong_hsprj.cmake")
include("cmake/yong_hslib.cmake")

hs_project(hs_3d_reconstructor CXX)

add_definitions(-DBOOST_ALL_NO_LIB)
include_directories("source")

add_subdirectory("source")
add_subdirectory("unit_test")

hs_project_end(hs_3d_reconstructor)
