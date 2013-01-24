#ifndef _UI_CONTROL_
#define _UI_CONTROL_

#include "Ui_Control_inner.h"
#include "Circles.h"

class GLWidget;
class Calculator;
class StatusViewer;

class Control : public QObject, public Ui::Control {
	Q_OBJECT
protected:
	const GLWidget* glw;
	const Calculator* cal;
	const StatusViewer* sv;
public:
	Control(GLWidget* g, Calculator* c, StatusViewer* s);
	void setupUi(QWidget* w);
public slots:
	void fpsChanged(scalar glFps, scalar calFps, scalar fbLoad, scalar realSpeed);
	void xAutoRot(double angle);
	void yAutoRot(double angle);
	void zAutoRot(double angle);
	void useColours(bool b);
	void showTrace(bool b);
	void connectTrace(bool b);
};

#endif /* _UI_CONTROL_ */
