
! include(../Clients.pri){
    error(Clients.pri not found.)
}

VPATH       +=  include/ src/
INCLUDEPATH +=  include/

HEADERS     +=  SimulationGrapher.hpp

SOURCES     +=  SimulationGrapher.cpp   \
                main.cpp
