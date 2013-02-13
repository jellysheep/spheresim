#ifndef _UI_CONTROL_
#define _UI_CONTROL_

#include "Ui_Control_inner.h"
#include "Circles.h"
#include <QMainWindow>

class GLWidget;
class Calculator;
class StatusViewer;
class QDockWidget;

class CustomDockWidget : public QWidget {
	
};

class Control : public QMainWindow {
	Q_OBJECT
protected:
	GLWidget* glw;
	Calculator* cal;
	StatusViewer* sv;
	Ui::Rendering* rend;
	Ui::Calculations* calc;
	QDockWidget *rendWg, *calcWg;
public:
	Control(GLWidget* g, Calculator* c, StatusViewer* s);
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
};

#endif /* _UI_CONTROL_ */
