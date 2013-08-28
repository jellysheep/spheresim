project(SphereSim_Server)
cmake_minimum_required(VERSION 2.8)

set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(USE_DOUBLE 1)
add_definitions("-DUSE_DOUBLE=${USE_DOUBLE}")

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

set(SOURCES
	${PROJECT_SOURCE_DIR}/main.cpp
	${PROJECT_SOURCE_DIR}/ActionServer.cpp
	${PROJECT_SOURCE_DIR}/ActionReceiver.cpp
	${PROJECT_SOURCE_DIR}/SphereManager.cpp
	${PROJECT_SOURCE_DIR}/SphereCalculator.cpp
	${PROJECT_SOURCE_DIR}/ButcherTableau.cpp
)
set(HEADERS ${HEADERS}
	${PROJECT_INCLUDE_DIR}/ActionServer.hpp
	${PROJECT_INCLUDE_DIR}/ActionReceiver.hpp
	${PROJECT_INCLUDE_DIR}/SphereManager.hpp
	${PROJECT_INCLUDE_DIR}/SphereCalculator.hpp
	${PROJECT_INCLUDE_DIR}/ButcherTableau.hpp
)

add_executable(SphereSim_Server ${SOURCES} ${HEADERS})
target_link_libraries(SphereSim_Server)
qt5_use_modules(SphereSim_Server Core Network)

install(TARGETS SphereSim_Server
	RUNTIME DESTINATION bin)
