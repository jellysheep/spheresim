project(SphereSim)
cmake_minimum_required(VERSION 2.8)

file(STRINGS "VERSION" VERSION)
add_definitions("-DVERSION=${VERSION}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(LIBRARY_OUTPUT_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Qt5Core REQUIRED)

set(HEADERS
	${PROJECT_SOURCE_DIR}/VERSION
	${PROJECT_INCLUDE_DIR}/Actions.hpp
	${PROJECT_INCLUDE_DIR}/Connection.hpp
	${PROJECT_INCLUDE_DIR}/Version.hpp
)

add_subdirectory(Server)
add_subdirectory(Viewer)
