project(SphereSim_Grapher)
cmake_minimum_required(VERSION 2.8)

set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)

set(SOURCES ${SOURCES}
    ${PROJECT_SOURCE_DIR}/SimulationGrapher.cpp
    ${PROJECT_SOURCE_DIR}/main.cpp
)
set(HEADERS ${HEADERS}
    ${PROJECT_INCLUDE_DIR}/SimulationGrapher.hpp
)

add_executable(SphereSim_Grapher ${SOURCES} ${HEADERS})
target_link_libraries(SphereSim_Grapher SphereSim_ClientLib)
qt5_use_modules(SphereSim_Grapher Core)

install(TARGETS SphereSim_Grapher
    RUNTIME DESTINATION bin)
