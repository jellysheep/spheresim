#include "StatusViewer.h"
#include "GLWidget.h"
#include "ClTimer.h"
#include <cstdio>

StatusViewer::StatusViewer(GLWidget* glw, ClTimer* clt) {
	QTimer* statusTimer = new QTimer(this);
	statusTimer->setInterval(1000);
	QObject::connect(statusTimer, SIGNAL(timeout()), this, SLOT(updateTimer()), Qt::QueuedConnection);
	statusTimer->start();
	glWidget = glw;
	clTimer = clt;
}

void StatusViewer::updateTimer() {
	printf("StatusViewer: \n");
	double factor = getElapsedNS()/1000000000.0;
	printf("Elapsed:  %9.2f seconds\n", factor);
	double glWidgetFrames = glWidget->popFramesCounter() / factor;
	printf("GlWidget: %9.2f fps\n", glWidgetFrames);
	double clTimerFrames = clTimer->popFramesCounter() / factor;
	printf("clTimer:  %9.2f fps\n\n", clTimerFrames);
}
