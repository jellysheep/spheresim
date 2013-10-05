project(SphereSim_Clients)
cmake_minimum_required(VERSION 2.8)

set(CLIENTLIB_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/ClientLib/include)

include_directories("${CLIENTLIB_INCLUDE_DIR}")

set(HEADERS ${HEADERS}
	${CLIENTLIB_INCLUDE_DIR}/ActionSender.hpp
)

add_subdirectory(ClientLib)
add_subdirectory(Tester)
add_subdirectory(Viewer)
add_subdirectory(Benchmark)
