
! include(../Clients.pri){
    error(Clients.pri not found.)
}

QT          +=  testlib

VPATH       +=  include/ src/
INCLUDEPATH +=  include/

HEADERS     +=  ServerBenchmark.hpp

SOURCES     +=  ServerBenchmark.cpp \
                main.cpp
