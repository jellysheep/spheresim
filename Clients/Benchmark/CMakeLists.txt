project(SphereSim_Benchmark)
cmake_minimum_required(VERSION 2.8)

set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

add_definitions("-DQT_NO_DEBUG_OUTPUT")

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)

set(SOURCES ${SOURCES}
    ${PROJECT_SOURCE_DIR}/ServerBenchmark.cpp
    ${PROJECT_SOURCE_DIR}/main.cpp
)
set(HEADERS ${HEADERS}
    ${PROJECT_INCLUDE_DIR}/ServerBenchmark.hpp
)

add_executable(SphereSim_Benchmark ${SOURCES} ${HEADERS})
target_link_libraries(SphereSim_Benchmark SphereSim_ClientLib)
qt5_use_modules(SphereSim_Benchmark Core Test)

install(TARGETS SphereSim_Benchmark
    RUNTIME DESTINATION bin)
