PROJECT(SphereSim)
cmake_minimum_required(VERSION 2.8)

#need to include FindOPENCL.cmake to
SET( CMAKE_MODULE_PATH ${SphereSim_SOURCE_DIR}/cmake)
SET( CMAKE_INCLUDE_PATH ${SphereSim_SOURCE_DIR}/cmake)

SET( CMAKE_CXX_COMPILER "ccache" )
SET( CMAKE_CXX_COMPILER_ARG1 "g++" )

ADD_DEFINITIONS(-DENGINE_OPENCL -DENGINE_CPP -DENGINE_READ)

ADD_CUSTOM_TARGET(run_ SphereSim ${CMAKE_BINARY_DIR})
ADD_CUSTOM_TARGET(run rm -f CMakeFiles/SphereSim.dir/main.cpp.o COMMAND unset CPPFLAGS COMMAND make run_ ${CMAKE_BINARY_DIR})
ADD_CUSTOM_TARGET(profile rm -f CMakeFiles/SphereSim.dir/main.cpp.o COMMAND CPPFLAGS=-DPROFILING make run_ ${CMAKE_BINARY_DIR})
#SET_TARGET_PROPERTIES(profile PROPERTIES COMPILE_FLAGS "-DPROFILING")
#SET_SOURCE_FILES_PROPERTIES(main.cpp PROPERTIES COMPILE_FLAGS -DPROFILING)
#SET_PROPERTY( SOURCE main.cpp PROPERTY COMPILE_FLAGS -DPROFILING )
#ADD_CUSTOM_TARGET(main DEPENDS main.cpp.o)
#SET_TARGET_PROPERTIES(main PROPERTIES COMPILE_FLAGS -DPROFILING)
set_source_files_properties(main.cpp PROPERTIES COMPILE_FLAGS $(CPPFLAGS))

INCLUDE ("OptimizeForArchitecture")
OptimizeForArchitecture ()
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

FIND_PACKAGE(Qt4 REQUIRED)
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
SET(SphereSim_HEADERS 			GLWidget.h		OpenClCalculator.h 		StatusViewer.h 		Spheres.h	ExponentialSlider.h		ExponentialInput.h		Ui_Control.h	Calculator.h	EigenCalculator_Engine.h	ExceptionHandler.h		PlotWidget.h	Dialog.h	FileCalculator.h	NanosecondTimer.h FramesCounter.h)

QT4_WRAP_CPP(SphereSim_HEADERS_MOC ${SphereSim_HEADERS})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_EXECUTABLE(SphereSim ${SphereSim_SOURCES} ${SphereSim_HEADERS_MOC})
TARGET_LINK_LIBRARIES(SphereSim ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${cllib} ${OPENCL_LIBRARIES} qwt)
