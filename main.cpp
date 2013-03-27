
#include "Ui_Control.h"
#include "Dialog.h"

#include "ExceptionHandler.h"

#include <QtWidgets/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QtConcurrent/QtConcurrentRun>
#include <QStyleFactory>
#include <QHBoxLayout>
#include <cstdio>

#include "GLWidget.h"
//#include "SpheresHolder.h"
#include "StatusViewer.h"

//#define PROFILING 0

//#if PROFILING
#ifdef PROFILING
	#include "EigenCalculator_Engine.h"
#endif

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif

int main(int argc, char *argv[]) {
	
	addExceptionHandler();
	
	//XInitThreads();

	QApplication app(argc, argv);
	
	app.setStyle(QStyleFactory::create("Plastique"));
	
	qRegisterMetaType<scalar>("scalar");
	
	//#if PROFILING == 0
	#ifndef PROFILING
	
		//Calculator* clTimer = new OpenClCalculator();
		//Calculator* clTimer = new EigenCalculator();
		Calculator* clTimer = Dialog().getCalculator();
		if(clTimer == NULL) exit(0);
		printf("Calculator initialized!\n");

		GLWidget window(clTimer);
		printf("GLWidget initialized!\n");
		
		clTimer->set(&window);
		
		StatusViewer statusViewer(&window, clTimer);
		//statusViewer.start();
		printf("StatusViewer initialized!\n");
		
		Control* win = new Control(&window, clTimer, &statusViewer);
		win->setDockNestingEnabled(true);
		printf("Control initialized!\n");
		
		clTimer->set(win->getPlotWidget());
		
		win->show();
		win->showMaximized();
		
	#else
		
		renderBool = false;
		
		Calculator* clTimer;
		//Calculator* clTimer = new EigenCalculator_Engine<(use3D?3:2),use3D>();
		if(use3D){
			clTimer = new EigenCalculator_Engine<3,true>();
		}else{
			clTimer = new EigenCalculator_Engine<2,false>();
		}
		
		if(clTimer == NULL) exit(0);
		printf("Calculator initialized!\n");
		
		StatusViewer statusViewer(NULL, clTimer);
		//statusViewer.start();
		printf("StatusViewer initialized!\n");
	
		clTimer->start();
		
	#endif
	
	//QApplication::setQuitOnLastWindowClosed(false);
	
	return app.exec();
}
