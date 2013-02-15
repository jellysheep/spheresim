#ifndef _UI_CONTROL_
#define _UI_CONTROL_

#include "Ui_Control_calculations.h"
#include "Ui_Control_rendering.h"
#include "Circles.h"
#include <QMainWindow>

class GLWidget;
class Calculator;
class StatusViewer;
class QDockWidget;

class CustomDockWidget : public QDockWidget {
public:
	CustomDockWidget(const char* c, QWidget* w):QDockWidget(c,w){}
	QSize sizeHint() const {
		return QSize(10,10);
	}
};

class Control : public QMainWindow {
	Q_OBJECT
protected:
	GLWidget* glw;
	Calculator* cal;
	StatusViewer* sv;
	Ui::Rendering* rend;
	Ui::Calculations* calc;
	CustomDockWidget *rendWg, *calcWg;
	void updateGL();
	bool fullscreen;
public:
	Control(GLWidget* g, Calculator* c, StatusViewer* s);
	void showMaximized();
	void showNormal();
	void showFullScreen();
public slots:
	void fpsChanged(scalar glFps, scalar calFps, scalar fbLoad, scalar realSpeed);
	void xAutoRot(double angle);
	void yAutoRot(double angle);
	void zAutoRot(double angle);
	void useColors(bool b);
	void showTrace(bool b);
	void connectTrace(bool b);
	void xBoxSize(double angle);
	void yBoxSize(double angle);
	void zBoxSize(double angle);
	void speedChanged(double d);
	void toggleRunning();
	void keyPressEvent(QKeyEvent* event);
	void showWireframe(bool b);
	void showReflections(bool b);
	void oneSphereSize(bool b);
	void maxSphereSize(double s);
	void minSphereSize(double s);
};

#endif /* _UI_CONTROL_ */
