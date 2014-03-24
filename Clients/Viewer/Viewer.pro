
! include(../Clients.pri){
    error(Clients.pri not found.)
}

QT          +=  opengl

VPATH       +=  include/ src/ QRC/
INCLUDEPATH +=  include/

HEADERS     +=  GLWidget.hpp    \
                MainWindow.hpp  \
                StartDialog.hpp

SOURCES     +=  GLWidget.cpp    \
                MainWindow.cpp  \
                StartDialog.cpp \
                main.cpp

RESOURCES   =   MainWindow.qrc  \
                StartDialog.qrc \
                Shaders.qrc

FORMS       =   MainWindow.ui   \
                StartDialog.ui
