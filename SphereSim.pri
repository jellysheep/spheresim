
DEFINES     +=  USE_EIGEN_LIBRARY=1 \
                VERSION=$$system(cat VERSION)

QT          -=  gui
QT          +=  network

CONFIG      +=  c++11

INCLUDEPATH +=  $$PWD/include/ /usr/include/eigen3

VPATH       +=  $$PWD/include/ $$PWD/src/

HEADERS     =   Actions.hpp         \
                Connection.hpp      \
                Version.hpp         \
                VectorLib.hpp       \
                Vector.hpp          \
                Sphere.hpp          \
                DataTransmit.hpp    \
                Console.hpp         \
                Dimension.hpp       \
                Wall.hpp            \
                Integrators.hpp     \
                Object.hpp          \
                SimulatedSystem.hpp

SOURCES     =   Connection.cpp      \
                Console.cpp         \
                Object.cpp          \
                SimulatedSystem.cpp \
                DataTransmit.cpp

QMAKE_CXXFLAGS_RELEASE  +=  -O3 -Wno-unused-local-typedefs -Wno-enum-compare \
                            -Wno-unused-parameter -Wno-unused-variable \
                            -Wno-ignored-qualifiers -Wno-strict-aliasing \
                            -Wno-reorder -Wno-maybe-uninitialized -fPIC
QMAKE_LFLAGS_RELEASE    +=  -O3 -fPIC
