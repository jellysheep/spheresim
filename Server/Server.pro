
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
				SimulatedSystem.hpp		\
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

LIBS		+=	-lgomp

QMAKE_CXXFLAGS_RELEASE	+=	-Ofast -fno-fast-math -fopenmp
QMAKE_LFLAGS_RELEASE	+=	-Ofast -fno-fast-math
