
DEFINES		+=	USE_EIGEN_LIBRARY=1	\
				VERSION=$$system(cat VERSION)

QT			-=	gui
QT			+=	network

CONFIG		+=	c++11

INCLUDEPATH	+=	$$PWD/include/

VPATH 		+=	$$PWD/include/ $$PWD/src/

HEADERS		=	Actions.hpp			\
				Connection.hpp		\
				Version.hpp			\
				VectorLib.hpp		\
				Vector.hpp			\
				Sphere.hpp			\
				SphereTransmit.hpp	\
				Console.hpp			\
				Dimension.hpp		\
				Wall.hpp			\
				Integrators.hpp

SOURCES		=	Connection.cpp		\
				Console.cpp

QMAKE_CXXFLAGS_RELEASE	+=	-O3
QMAKE_LFLAGS_RELEASE	+=	-O3
