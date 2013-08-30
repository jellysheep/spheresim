project(SphereSim_Tester)
cmake_minimum_required(VERSION 2.8)

set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set (VIEWER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Viewer/include)
set (VIEWER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Viewer/src)

set(USE_DOUBLE 1)
add_definitions("-DUSE_DOUBLE=${USE_DOUBLE}")

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${VIEWER_INCLUDE_DIR}")

add_definitions("-DQT_NO_DEBUG_OUTPUT")

set(SOURCES
	${PROJECT_SOURCE_DIR}/main.cpp
	${VIEWER_SOURCE_DIR}/ActionSender.cpp
	${PROJECT_SOURCE_DIR}/ServerTester.cpp
)
set(HEADERS ${HEADERS}
	${VIEWER_INCLUDE_DIR}/ActionSender.hpp
	${PROJECT_INCLUDE_DIR}/ServerTester.hpp
)

add_executable(SphereSim_Tester ${SOURCES} ${HEADERS})
target_link_libraries(SphereSim_Tester)
qt5_use_modules(SphereSim_Tester Core Network)

install(TARGETS SphereSim_Tester
	RUNTIME DESTINATION bin)
