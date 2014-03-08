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
	${PROJECT_SOURCE_DIR}/MainWindow.cpp
	${PROJECT_SOURCE_DIR}/StartDialog.cpp
)
set(HEADERS ${HEADERS}
	${PROJECT_INCLUDE_DIR}/GLWidget.hpp
	${PROJECT_INCLUDE_DIR}/MainWindow.hpp
	${PROJECT_INCLUDE_DIR}/StartDialog.hpp
)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

find_package(OpenGL REQUIRED)

set(UI QRC/MainWindow.ui QRC/StartDialog.ui)
qt5_wrap_ui(HEADERS_UI ${UI})
qt5_add_resources(RESOURCES QRC/MainWindow.qrc QRC/StartDialog.qrc QRC/Shaders.qrc)

add_executable(SphereSim_Viewer ${SOURCES} ${HEADERS} ${HEADERS_UI} ${RESOURCES})
if(WIN32)
	target_link_libraries(SphereSim_Viewer SphereSim_ClientLib ${OPENGL_LIBRARIES} libGLESv2)
else()
	target_link_libraries(SphereSim_Viewer SphereSim_ClientLib ${OPENGL_LIBRARIES})
endif()
qt5_use_modules(SphereSim_Viewer Core Network Widgets OpenGL)

install(TARGETS SphereSim_Viewer
	RUNTIME DESTINATION bin)
