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
class PlotWidget;

class Control : public QMainWindow {
	Q_OBJECT
protected:
	GLWidget* glw;
	Calculator* cal;
	StatusViewer* sv;
	Ui::Rendering* rend;
	Ui::Calculations* calc;
	QDockWidget *rendWg, *calcWg;
	PlotWidget *graphWg;
	void updateGL();
	bool fullscreen;
public:
	Control(GLWidget* g, Calculator* c, StatusViewer* s);
	void showMaximized();
	void showNormal();
	void showFullScreen();
	PlotWidget* getPlotWidget(){
		return graphWg;
	}
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
	void setShowCube(bool b);
	void earthGravity(double g);
	void interGravity(double g);
	void setAirResistance(double a);
	void setWallResistance(bool b);
	void setEModul(double d);
	void setPoisson(double d);
	void setElasticity(double d);
	void setHueStep(double d);
	void setShowLights(bool b);
	void setTraceAmount(double d);
	void setRender(bool b);
	void setSave(bool b);
};

#endif /* _UI_CONTROL_ */
