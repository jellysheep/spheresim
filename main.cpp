#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QtConcurrentRun>
#include <cstdio>

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif

#include "GLWidget.h"
#include "ClTimer.h"
#include "CirclesHolder.h"
#include "StatusViewer.h"

int main(int argc, char *argv[]) {
	
	XInitThreads();

	QApplication app(argc, argv);
	
	ClTimer* clTimer = new ClTimer();
	printf("ClTimer initialized!\n");

	GLWidget window(clTimer);
	window.resize(600,600);
	window.show();
	printf("GLWidget initialized!\n");
	
	clTimer->set(&window);
	
	StatusViewer statusViewer(&window, clTimer);
	statusViewer.start();
	printf("StatusViewer initialized!\n");
	
	//QFuture<void> future = QtConcurrent::run(start, clTimer);
	clTimer->start();
	printf("ClTimer started!\n");
	//QFuture<void> future2 = QtConcurrent::run(run, &statusViewer);

	return app.exec();
}
