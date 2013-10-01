project(SphereSim)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${SphereSim_SOURCE_DIR}/cmake)

file(STRINGS "VERSION" VERSION)
add_definitions("-DVERSION=${VERSION}")

set(USE_EIGEN_LIBRARY 1)
if(USE_EIGEN_LIBRARY GREATER 0)
	find_package(Eigen3 REQUIRED)
	include_directories("${EIGEN3_INCLUDE_DIR}")
endif(USE_EIGEN_LIBRARY GREATER 0)
add_definitions("-DUSE_EIGEN_LIBRARY=${USE_EIGEN_LIBRARY}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(LIBRARY_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
if(DEFINED COMPATIBLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++03 -fPIC")
	set(_COMPATIBLE ${COMPATIBLE})
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
endif()

set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

set(HEADERS
	${PROJECT_INCLUDE_DIR}/Actions.hpp
	${PROJECT_INCLUDE_DIR}/Connection.hpp
	${PROJECT_INCLUDE_DIR}/Version.hpp
	${PROJECT_INCLUDE_DIR}/VectorLib.hpp
	${PROJECT_INCLUDE_DIR}/Vector.hpp
	${PROJECT_INCLUDE_DIR}/Sphere.hpp
	${PROJECT_INCLUDE_DIR}/SphereTransmit.hpp
	${PROJECT_INCLUDE_DIR}/Console.hpp
	${PROJECT_INCLUDE_DIR}/Dimension.hpp
	${PROJECT_INCLUDE_DIR}/Wall.hpp
	${PROJECT_INCLUDE_DIR}/Integrators.hpp
)
set(SOURCES
	${PROJECT_SOURCE_DIR}/Connection.cpp
	${PROJECT_SOURCE_DIR}/Console.cpp
)

#documentation:
FIND_PACKAGE(Doxygen)
if (NOT DOXYGEN_FOUND)
	message(WARNING "Doxygen is needed to build the documentation.")
endif()
configure_file(Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
add_custom_target (docs COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
	SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)

add_subdirectory(Server)
add_subdirectory(Clients)
