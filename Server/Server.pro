
! include(../SphereSim.pri){
	error(SphereSim.pri not found.)
}

DEFINES		+=	USE_DOUBLE=1

VPATH		+=	include/ src/
INCLUDEPATH	+=	include/

HEADERS		+=	ActionServer.hpp		\
				ActionReceiver.hpp		\
				SphereCalculator.hpp	\
				ButcherTableau.hpp		\
				SimulationWorker.hpp	\
				WorkQueue.hpp			\
				Array.hpp				\
				TwoDimArray.hpp

SOURCES		+=	main.cpp				\
				ActionServer.cpp		\
				ActionReceiver.cpp		\
				SphereCalculator.cpp	\
				ButcherTableau.cpp		\
				SimulationWorker.cpp	\
				WorkQueue.cpp

equals(NO_OPENMP, "true") {
	DEFINES			+=	NO_OPENMP=1
} else {
	LIBS			+=	-lgomp
	QMAKE_CXXFLAGS	+=	-fopenmp
}

QMAKE_CXXFLAGS		+=	-Ofast -fno-fast-math
QMAKE_LFLAGS		+=	-Ofast -fno-fast-math
