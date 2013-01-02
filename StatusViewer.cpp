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
	lastGlWidgetFrames = 0;
	lastClTimerFrames = 0;
}

void StatusViewer::updateTimer() {
	printf("StatusViewer: \n\n");
	double factor = getElapsedNS()/1000000000.0;
	printf("Elapsed:  %8.2f seconds\n", factor);
	double glWidgetFrames = glWidget->popFramesCounter() / factor;
	if(lastGlWidgetFrames == 0){
		lastGlWidgetFrames = glWidgetFrames;
	}else{
		lastGlWidgetFrames *= (1-f);
		lastGlWidgetFrames += f*glWidgetFrames;
	}
	printf("GlWidget: %8.2f fps       [now: %8.2f fps]\n", lastGlWidgetFrames, glWidgetFrames);
	double clTimerFrames = clTimer->popFramesCounter() / factor;
	if(lastClTimerFrames == 0){
		lastClTimerFrames = clTimerFrames;
	}else{
		lastClTimerFrames *= (1-f);
		lastClTimerFrames += f*clTimerFrames;
	}
	printf("ClTimer:  %8.2f fps       [now: %8.2f fps]\n", lastClTimerFrames, clTimerFrames);
	fps = lastClTimerFrames;
	clTimer->fpsChanged(lastClTimerFrames);
}
