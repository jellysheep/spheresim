project(SphereSim_Server)
cmake_minimum_required(VERSION 2.8)

set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(USE_DOUBLE 1)
add_definitions("-DUSE_DOUBLE=${USE_DOUBLE}")
set(NO_OPENMP 1)
add_definitions("-DNO_OPENMP=${NO_OPENMP}")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -fno-fast-math")
if(NO_OPENMP EQUAL 0)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

set(SOURCES ${SOURCES}
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/ActionServer.cpp
    ${PROJECT_SOURCE_DIR}/ActionReceiver.cpp
    ${PROJECT_SOURCE_DIR}/SphereCalculator.cpp
    ${PROJECT_SOURCE_DIR}/ButcherTableau.cpp
    ${PROJECT_SOURCE_DIR}/SimulationWorker.cpp
    ${PROJECT_SOURCE_DIR}/WorkQueue.cpp
)
set(HEADERS ${HEADERS}
    ${PROJECT_INCLUDE_DIR}/ActionServer.hpp
    ${PROJECT_INCLUDE_DIR}/ActionReceiver.hpp
    ${PROJECT_INCLUDE_DIR}/SphereCalculator.hpp
    ${PROJECT_INCLUDE_DIR}/ButcherTableau.hpp
    ${PROJECT_INCLUDE_DIR}/SimulationWorker.hpp
    ${PROJECT_INCLUDE_DIR}/WorkQueue.hpp
    ${PROJECT_INCLUDE_DIR}/TwoDimArray.hpp
)

add_executable(SphereSim_Server ${SOURCES} ${GLOBAL_SOURCES} ${HEADERS})
if(NO_OPENMP EQUAL 0)
    target_link_libraries(SphereSim_Server gomp)
else()
    target_link_libraries(SphereSim_Server)
endif()
qt5_use_modules(SphereSim_Server Core Network)

install(TARGETS SphereSim_Server
    RUNTIME DESTINATION bin)
