
#include "Ui_Control.h"
#include "Dialog.h"

#include "ExceptionHandler.h"

#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QtConcurrentRun>
#include <QStyleFactory>
#include <QHBoxLayout>
#include <cstdio>

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif

#include "GLWidget.h"
#include "CirclesHolder.h"
#include "StatusViewer.h"

#define PROFILING 0

int main(int argc, char *argv[]) {
	
	addExceptionHandler();
	
	//XInitThreads();

	QApplication app(argc, argv);
	
	app.setStyle(QStyleFactory::create("Plastique"));
	
	qRegisterMetaType<scalar>("scalar");
	
	//Calculator* clTimer = new OpenClCalculator();
	//Calculator* clTimer = new EigenCalculator();
	Calculator* clTimer = Dialog().getCalculator();
	if(clTimer == NULL) exit(0);
	printf("Calculator initialized!\n");
	
	#if PROFILING == 0

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
	
		clTimer->start();
		
	#endif
	
	//QApplication::setQuitOnLastWindowClosed(false);
	
	return app.exec();
}
