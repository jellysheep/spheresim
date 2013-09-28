project(SphereSim_Viewer)
cmake_minimum_required(VERSION 2.8)

set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories("${PROJECT_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_BINARY_DIR}")

set(SOURCES ${SOURCES}
	${PROJECT_SOURCE_DIR}/main.cpp
	${PROJECT_SOURCE_DIR}/GLWidget.cpp
)
set(HEADERS ${HEADERS}
	${PROJECT_INCLUDE_DIR}/GLWidget.hpp
)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

find_package(OpenGL REQUIRED)

set(UI UI/MainWindow.ui)
qt5_wrap_ui(HEADERS_UI ${UI})
qt5_add_resources(RESOURCES UI/MainWindow.qrc)

add_executable(SphereSim_Viewer ${SOURCES} ${HEADERS} ${HEADERS_UI} ${RESOURCES})
target_link_libraries(SphereSim_Viewer SphereSim_ClientLib ${OPENGL_LIBRARIES})
qt5_use_modules(SphereSim_Viewer Core Network Widgets OpenGL)

install(TARGETS SphereSim_Viewer
	RUNTIME DESTINATION bin)
