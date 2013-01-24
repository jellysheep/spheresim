
#include "Ui_Control.h"

#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QtConcurrentRun>
#include <QStyleFactory>
#include <QMainWindow>
#include <QHBoxLayout>
#include <cstdio>

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif

#include "GLWidget.h"
#include "OpenClCalculator.h"
#include "CirclesHolder.h"
#include "StatusViewer.h"

int main(int argc, char *argv[]) {
	
	XInitThreads();

	QApplication app(argc, argv);
	
	QApplication::setStyle(QStyleFactory::create("Plastique"));
	
	qRegisterMetaType<scalar>("scalar");
	
	Calculator* clTimer = new OpenClCalculator();
	printf("Calculator initialized!\n");
	
	QMainWindow* win = new QMainWindow();
	
	win->setCentralWidget(new QWidget());
	QWidget* f = win->centralWidget();
	
	QHBoxLayout* layout = new QHBoxLayout();
	f->setLayout(layout);

	//QWidget* w = new QWidget();
	GLWidget window(clTimer);
	//window.setMinimumSize(QSize(600,600));
	//window.resize(600,600);
	//window.show();
	layout->addWidget(&window);
	//layout->addWidget(w);
	printf("GLWidget initialized!\n");
	
	clTimer->set(&window);
	
	StatusViewer statusViewer(&window, clTimer);
	statusViewer.start();
	printf("StatusViewer initialized!\n");
	
	QWidget* w2 = new QWidget();
	Control* control = new Control(&window, clTimer, &statusViewer);
	control->setupUi(w2);
	layout->addWidget(w2);
	//control->resize(600,600);
	//control->show();
	//layout->addWidget(control);
	printf("Control initialized!\n");
	
	//win->show();
	win->showMaximized();
	
	//QFuture<void> future = QtConcurrent::run(start, clTimer);
	clTimer->start();
	printf("Calculator started!\n");
	//QFuture<void> future2 = QtConcurrent::run(run, &statusViewer);

	return app.exec();
}
