
LIBS_SAVE	=	$$LIBS

! include(../Clients.pri){
	error(Clients.pri not found.)
}

LIBS		=	$$LIBS_SAVE

TEMPLATE	=	lib

CONFIG		+=	static

VPATH		+=	include/ src/
INCLUDEPATH	+=	include/

HEADERS		+=	ActionSender.hpp	\
				FrameBuffer.hpp		\
				SystemCreator.hpp

SOURCES		+=	ActionSender.cpp	\
				FrameBuffer.cpp		\
				SystemCreator.cpp
