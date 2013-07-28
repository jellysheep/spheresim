project(SphereSim)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${SphereSim_SOURCE_DIR}/cmake)

file(STRINGS "VERSION" VERSION)
add_definitions("-DVERSION=${VERSION}")

set(USE_DOUBLE 1)
add_definitions("-DUSE_DOUBLE=${USE_DOUBLE}")
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

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

#for mingw-w64 compatibility:
set(Qt5Core_LIBRARIES Qt5Core)
set(Qt5Network_LIBRARIES Qt5Network)
set(QT_MOC_EXECUTABLE moc-qt5)

set(HEADERS
	${PROJECT_INCLUDE_DIR}/Actions.hpp
	${PROJECT_INCLUDE_DIR}/Connection.hpp
	${PROJECT_INCLUDE_DIR}/Version.hpp
	${PROJECT_INCLUDE_DIR}/VectorLib.hpp
	${PROJECT_INCLUDE_DIR}/Vector.hpp
	${PROJECT_INCLUDE_DIR}/Sphere.hpp
	${PROJECT_INCLUDE_DIR}/Console.hpp
)

add_subdirectory(Server)
add_subdirectory(Viewer)
add_subdirectory(Tester)
