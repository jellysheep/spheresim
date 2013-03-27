PROJECT(SphereSim)
cmake_minimum_required(VERSION 2.8)

#need to include FindOPENCL.cmake to
SET( CMAKE_MODULE_PATH ${SphereSim_SOURCE_DIR}/cmake)
SET( CMAKE_INCLUDE_PATH ${SphereSim_SOURCE_DIR}/cmake)

SET( CMAKE_CXX_COMPILER "ccache" )
SET( CMAKE_CXX_COMPILER_ARG1 "g++" )

ADD_DEFINITIONS(-DENGINE_OPENCL -DENGINE_CPP -DENGINE_READ)

#INCLUDE ("OptimizeForArchitecture")
#OptimizeForArchitecture ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -g")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2 -msse -Ofast -march=native -mtune=native") #  -msse3 -msse4

if(OPENMP_FOUND)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

message("CMake module path: ${CMAKE_MODULE_PATH}\n")

FIND_PACKAGE (OPENCL)

FIND_PACKAGE(Eigen3)

#set( CMAKE_AUTOMOC ON )
#find_package( Qt REQUIRED )
find_package(Qt5Widgets REQUIRED)
#find_package(Qt5Declarative REQUIRED)
find_package(Qt5OpenGL REQUIRED)

FIND_PACKAGE(OpenGL REQUIRED)
SET(CMAKE_REQUIRED_LIBRARIES ${GL_LIBRARY})
LINK_DIRECTORIES(${OPENGL_LIBRARIES})
INCLUDE_DIRECTORIES(
	${OPENGL_INCLUDE_DIR}
	${EIGEN3_INCLUDE_DIR}
	${Qt_INCLUDES}
)

SET(QT_USE_QTOPENGL TRUE)

SET(SphereSim_SOURCES main.cpp 	GLWidget.cpp	OpenClCalculator.cpp 	StatusViewer.cpp	Spheres.cpp	ExponentialSlider.cpp	ExponentialInput.cpp	Ui_Control.cpp	Calculator.cpp	EigenCalculator_Engine.cpp	ExceptionHandler.cpp	PlotWidget.cpp	Dialog.cpp	FileCalculator.cpp)
SET(SphereSim_HEADERS 			GLWidget.h		OpenClCalculator.h 		StatusViewer.h 		Spheres.h	ExponentialSlider.h		ExponentialInput.h		Ui_Control.h	Calculator.h	EigenCalculator_Engine.h	ExceptionHandler.h		PlotWidget.h	Dialog.h	FileCalculator.h	NanosecondTimer.h FramesCounter.h EigenCalculator_QObject.h)

QT5_WRAP_CPP(SphereSim_HEADERS_MOC ${SphereSim_HEADERS})

#INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_EXECUTABLE(SphereSim ${SphereSim_SOURCES} ${SphereSim_HEADERS_MOC})
TARGET_LINK_LIBRARIES(SphereSim ${OPENGL_LIBRARIES} ${cllib} ${OPENCL_LIBRARIES} qwt)

qt5_use_modules(SphereSim Core Widgets OpenGL)
