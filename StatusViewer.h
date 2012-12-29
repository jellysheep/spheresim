#ifndef _STATUSVIEWER_H_
#define _STATUSVIEWER_H_

#include <QObject>
#include <QTimer>
#include "NanosecondTimer.h"
#include "GLWidget.h"
#include "ClTimer.h"

class StatusViewer : public QObject, protected NanosecondTimer {

	Q_OBJECT // must include this if you use Qt signals/slots	  

protected:
	GLWidget* glWidget;
	ClTimer* clTimer;

public:
	StatusViewer(GLWidget* glw, ClTimer* clt);

public slots:
	void updateTimer();
};

#endif  /* _STATUSVIEWER_H_ */
