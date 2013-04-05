PROJECT(SphereSim)
cmake_minimum_required(VERSION 2.8)

#need to include FindOPENCL.cmake to
SET( CMAKE_MODULE_PATH ${SphereSim_SOURCE_DIR}/cmake)
SET( CMAKE_INCLUDE_PATH ${SphereSim_SOURCE_DIR}/cmake)

SET( CMAKE_CXX_COMPILER "ccache" )
SET( CMAKE_CXX_COMPILER_ARG1 "g++" )

ADD_DEFINITIONS(-DENGINE_CPP -DENGINE_READ -DENGINE_THRUST_ -DENGINE_OPENCL)

#ADD_DEFINITIONS(-DQT_DEBUG -DDEBUG)
#SET(QT_USE_QTOPENGL TRUE)

ADD_CUSTOM_TARGET(run_ SphereSim ${CMAKE_BINARY_DIR})
ADD_CUSTOM_TARGET(run rm -f CMakeFiles/SphereSim.dir/main.cpp.o COMMAND unset CPPFLAGS COMMAND make run_ ${CMAKE_BINARY_DIR})
ADD_CUSTOM_TARGET(profile rm -f CMakeFiles/SphereSim.dir/main.cpp.o COMMAND CPPFLAGS=-DPROFILING make SphereSim ${CMAKE_BINARY_DIR})
#SET_TARGET_PROPERTIES(profile PROPERTIES COMPILE_FLAGS "-DPROFILING")
#SET_SOURCE_FILES_PROPERTIES(main.cpp PROPERTIES COMPILE_FLAGS -DPROFILING)
#SET_PROPERTY( SOURCE main.cpp PROPERTY COMPILE_FLAGS -DPROFILING )
#ADD_CUSTOM_TARGET(main DEPENDS main.cpp.o)
#SET_TARGET_PROPERTIES(main PROPERTIES COMPILE_FLAGS -DPROFILING)
set_source_files_properties(main.cpp PROPERTIES COMPILE_FLAGS $(CPPFLAGS))

set(DEVICE_SYSTEM "THRUST_DEVICE_SYSTEM_OMP") #THRUST_DEVICE_SYSTEM_OMP THRUST_DEVICE_SYSTEM_TBB

#INCLUDE ("OptimizeForArchitecture")
#OptimizeForArchitecture ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fopenmp -DTHRUST_DEVICE_SYSTEM=${DEVICE_SYSTEM}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2 -msse -Ofast -march=native -mtune=native") #  -msse3 -msse4
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

if(OPENMP_FOUND)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

message("CMake module path: ${CMAKE_MODULE_PATH}\n")

FIND_PACKAGE (OPENCL)

FIND_PACKAGE(Eigen3)

#find_package(CUDA)
INCLUDE ("FindThrust")

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
	${CMAKE_BINARY_DIR}
	"/opt/cuda-toolkit/include"
)

SET(SphereSim_SOURCES main.cpp 	GLWidget.cpp	OpenClCalculator.cpp 	StatusViewer.cpp	Spheres.cpp	ExponentialSlider.cpp	ExponentialInput.cpp	Ui_Control.cpp	Calculator.cpp	EigenCalculator_Engine.cpp	ExceptionHandler.cpp	PlotWidget.cpp	Dialog.cpp	FileCalculator.cpp	ThrustCalculator.cpp	EigenCalculator_PairCollider.cpp	EigenCalculator_EfficientPairCollider.cpp	EigenCalculator_StripeCollider.cpp	EigenCalculator_CellSortCollider.cpp	EigenCalculator_CellCountCollider.cpp	EigenCalculator_CellForce.cpp	EigenCalculator_PairGravitation.cpp	EigenCalculator_CellGravitation.cpp)
SET(SphereSim_HEADERS 			GLWidget.h		OpenClCalculator.h 		StatusViewer.h 		Spheres.h	ExponentialSlider.h		ExponentialInput.h		Ui_Control.h	Calculator.h	EigenCalculator_Engine.h	ExceptionHandler.h		PlotWidget.h	Dialog.h	FileCalculator.h	ThrustCalculator.h		EigenCalculator_PairCollider.h		EigenCalculator_EfficientPairCollider.h		EigenCalculator_StripeCollider.h	EigenCalculator_CellSortCollider.h		EigenCalculator_CellCountCollider.h		EigenCalculator_CellForce.h		EigenCalculator_PairGravitation.h	EigenCalculator_CellGravitation.h EigenCalculator_Force.h EigenCalculator_PairCollider.h NanosecondTimer.h FramesCounter.h)
QT5_WRAP_CPP(SphereSim_HEADERS_MOC ${SphereSim_HEADERS})

SET(SphereSim_UI Calculations_Qt5.ui Rendering_Qt5.ui Dialog_Qt5.ui)
QT5_WRAP_UI(SphereSim_HEADERS_UI ${SphereSim_UI})

FIND_PACKAGE(Qt5LinguistTools)
SET(ts_files "./translate_de_DE.ts")
#~ QT5_CREATE_TRANSLATION(qm_files "./" ${SphereSim_HEADERS_UI} ${SphereSim_HEADERS} ${ts_files})
#QT5_ADD_TRANSLATION(qm_files ${ts_files})

#INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS})

#message("QT_DEFINITIONS: ${QT_DEFINITIONS}\n")
#message("QT_LIBRARIES: ${QT_LIBRARIES}\n")
#message("QT_DEBUG_LIBRARIES: ${QT_DEBUG_LIBRARIES}\n")

ADD_EXECUTABLE(SphereSim ${SphereSim_SOURCES} ${SphereSim_HEADERS_MOC} ${SphereSim_HEADERS_UI} ${qm_files})

TARGET_LINK_LIBRARIES(SphereSim ${OPENGL_LIBRARIES} ${cllib} ${OPENCL_LIBRARIES} qwt gomp)

qt5_use_modules(SphereSim Core Widgets OpenGL)
