#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include "GLWidget.h"
#include "ClTimer.h"
#include "CirclesHolder.h"
#include "StatusViewer.h"

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);
	
	CirclesHolder circlesHolder;

	GLWidget window(&circlesHolder);
	window.resize(800,600);
	window.show();
	
	ClTimer* clTimer = new ClTimer(&circlesHolder);
	
	StatusViewer statusViewer(&window, clTimer);

	return app.exec();
}
