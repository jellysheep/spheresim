#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <cstdio>
#include "GLWidget.h"
#include "ClTimer.h"
#include "CirclesHolder.h"
#include "StatusViewer.h"

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);
	
	ClTimer* clTimer = new ClTimer();
	printf("ClTimer initialized!\n");

	GLWidget window(clTimer);
	window.resize(600,600);
	window.show();
	printf("GLWidget initialized!\n");
	
	clTimer->set(&window);
	
	StatusViewer statusViewer(&window, clTimer);
	printf("StatusViewer initialized!\n");
	
	clTimer->start();
	printf("ClTimer started!\n");

	return app.exec();
}
