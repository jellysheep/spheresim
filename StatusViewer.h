#ifndef _STATUSVIEWER_H_
#define _STATUSVIEWER_H_

//#include <QObject>
//#include <QTimer>
#include <QThread>
#include "NanosecondTimer.h"
#include "Circles.h"
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
	const static scalar f = 0.5;

public:
	StatusViewer(GLWidget* glw, Calculator* clt);
	
	void run();

signals:
	void fpsChanged(scalar glFps, scalar calFps, scalar fbLoad, scalar realSpeed);

};


#endif  /* _STATUSVIEWER_H_ */
