PROJECT(circles)
cmake_minimum_required(VERSION 2.8)

#need to include FindOPENCL.cmake to
SET( CMAKE_MODULE_PATH ${circles_SOURCE_DIR}/cmake)

SET( CMAKE_CXX_COMPILER "ccache" )
SET( CMAKE_CXX_COMPILER_ARG1 "g++" )

message("CMake module path: ${CMAKE_MODULE_PATH}\n")

FIND_PACKAGE (OPENCL)

INCLUDE_DIRECTORIES( 
	${circles_SOURCE_DIR}/../opencl10
)

FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
SET(CMAKE_REQUIRED_LIBRARIES ${GL_LIBRARY})
LINK_DIRECTORIES(${OPENGL_LIBRARIES})
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})

SET(QT_USE_QTOPENGL TRUE)

SET(circles_SOURCES main.cpp 	GLWidget.cpp	ClTimer.cpp 	StatusViewer.cpp	Circles.cpp	ExponentialSlider.cpp	ExponentialInput.cpp)
SET(circles_HEADERS 			GLWidget.h		ClTimer.h 		StatusViewer.h 		Circles.h	ExponentialSlider.h		ExponentialInput.h	Ui_Control.h	NanosecondTimer.h FramesCounter.h)

QT4_WRAP_CPP(circles_HEADERS_MOC ${circles_HEADERS})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_EXECUTABLE(circles_ocgl ${circles_SOURCES} ${circles_HEADERS_MOC})
TARGET_LINK_LIBRARIES(circles_ocgl ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${cllib} ${OPENCL_LIBRARIES})
