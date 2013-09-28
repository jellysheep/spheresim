project(SphereSim_Tester)
cmake_minimum_required(VERSION 2.8)

set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

add_definitions("-DQT_NO_DEBUG_OUTPUT")

find_package(Qt5Test REQUIRED)

set(SOURCES ${SOURCES}
	${PROJECT_SOURCE_DIR}/main.cpp
	${PROJECT_SOURCE_DIR}/ServerTester.cpp
)
set(HEADERS ${HEADERS}
	${PROJECT_INCLUDE_DIR}/ServerTester.hpp
)

add_executable(SphereSim_Tester ${SOURCES} ${HEADERS})
target_link_libraries(SphereSim_Tester SphereSim_ClientLib)
qt5_use_modules(SphereSim_Tester Core Network Test)

install(TARGETS SphereSim_Tester
	RUNTIME DESTINATION bin)
