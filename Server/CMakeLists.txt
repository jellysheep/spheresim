project(SphereSim_Server)
cmake_minimum_required(VERSION 2.8)

set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SOURCES
	${PROJECT_SOURCE_DIR}/main.cpp
	${PROJECT_SOURCE_DIR}/ActionReceiver.cpp
)
set(HEADERS ${HEADERS}
	${PROJECT_INCLUDE_DIR}/ActionReceiver.hpp
)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_INCLUDE_DIR}")

add_executable(SphereSim_Server ${SOURCES} ${HEADERS})
target_link_libraries(SphereSim_Server)
