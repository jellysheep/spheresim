#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include "GLWidget.h"
#include "ClTimer.h"
#include "CirclesHolder.h"
#include "StatusViewer.h"

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);
	
	ClTimer* clTimer = new ClTimer();

	GLWidget window(clTimer);
	window.resize(800,600);
	window.show();
	
	StatusViewer statusViewer(&window, clTimer);
	
	clTimer->start();

	return app.exec();
}
