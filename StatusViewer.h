#ifndef _STATUSVIEWER_H_
#define _STATUSVIEWER_H_

//#include <QObject>
//#include <QTimer>
#include <QThread>
#include <QTimer>
#include "NanosecondTimer.h"
#include "Spheres.h"
class GLWidget;
class Calculator;
class Control;

class StatusViewer : public QThread, protected NanosecondTimer {

	Q_OBJECT // must include this if you use Qt signals/slots	 

protected:
	GLWidget* glWidget;
	Calculator* clTimer;
	Control* ctl;
	scalar lastGlWidgetFrames, lastCalculatorFrames;
	const static scalar f = 0.1;
	bool running;
	QTimer* statusTimer;

public:
	StatusViewer(GLWidget* glw, Calculator* clt);
	
	void run();

signals:
	void fpsChanged(scalar glFps, scalar calFps, scalar fbLoad, scalar realSpeed);
	void temperatureChanged(double temp);
	
public slots:
	void stop();
	void updateTimer();

};


#endif  /* _STATUSVIEWER_H_ */
