
! include(../Clients.pri){
    error(Clients.pri not found.)
}

QT          +=  testlib

VPATH       +=  include/ src/
INCLUDEPATH +=  include/

HEADERS     +=  ServerTester.hpp

SOURCES     +=  ServerTester.cpp    \
                main.cpp
