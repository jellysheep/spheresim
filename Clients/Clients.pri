
! include(../SphereSim.pri){
    error(SphereSim.pri not found.)
}

INCLUDEPATH +=  $$PWD/ClientLib/include/

VPATH       +=  $$PWD/ClientLib/include/

HEADERS     +=  ActionSender.hpp

LIBS        +=  -L../ClientLib -lClientLib
