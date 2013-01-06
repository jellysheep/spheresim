#include "StatusViewer.h"
#include "GLWidget.h"
#include "ClTimer.h"
#include <cstdio>
#include <cmath>

#define sign(x) (((x)<0)?(-1):(1))

StatusViewer::StatusViewer(GLWidget* glw, ClTimer* clt) {
	/*QTimer* statusTimer = new QTimer(this);
	statusTimer->setInterval(1000);
	QObject::connect(statusTimer, SIGNAL(timeout()), this, SLOT(updateTimer()), Qt::QueuedConnection);
	statusTimer->start();*/
	glWidget = glw;
	clTimer = clt;
	lastGlWidgetFrames = 0;
	lastClTimerFrames = 0;
}

void StatusViewer::run() {
	while(true){
		msleep(500);
		
		printf("\nStatusViewer: \n");
		scalar factor = getElapsedNS()/1000000000.0;
		printf("Elapsed:    %8.2f seconds\n", factor);
		
		scalar glWidgetFrames = glWidget->popFramesCounter() / factor;
		if(lastGlWidgetFrames == 0){
			lastGlWidgetFrames = glWidgetFrames;
		}else{
			lastGlWidgetFrames *= (1-f);
			lastGlWidgetFrames += f*glWidgetFrames;
		}
		printf("GlWidget:   %8.2f fps       [now: %8.2f fps]\n", lastGlWidgetFrames, glWidgetFrames);
		
		scalar frameBufferLoad = clTimer->getFrameBufferLoad();
		//printf("frameBufferLoad:  %6.4f\n", frameBufferLoad);
		
		scalar clTimerFrames = clTimer->popFramesCounter() / factor;
		if(lastClTimerFrames == 0){
			lastClTimerFrames = clTimerFrames;
		}else{
			lastClTimerFrames *= (1-f);
			lastClTimerFrames += f*clTimerFrames;
		}
		printf("ClTimer:    %8.2f fps       [now: %8.2f fps]\n", lastClTimerFrames, clTimerFrames);
		
		if(lastClTimerFrames < minFps){
			speedCorrection = lastClTimerFrames / minFps;
		}else{
			speedCorrection = 1.0;
		}
		static scalar bufferLoadTarget = 0.65;
		double nextClTimerFrames = lastClTimerFrames*(1+
			0.1*(pow(16,pow((frameBufferLoad>bufferLoadTarget)?
				(1-((1-frameBufferLoad)*0.5/(1-bufferLoadTarget))):
				(frameBufferLoad*0.5/bufferLoadTarget),2)))*
			sign(frameBufferLoad-bufferLoadTarget));
		printf("frameBuffer:%10.4f => ClTimer:   %8.2f fps\n", frameBufferLoad, nextClTimerFrames);
		//renderFps = min((int)nextClTimerFrames, renderFpsMax);
		//glWidget->rotationTimer->setInterval(1000/renderFps);
		
		fps = nextClTimerFrames;
		clTimer->fpsChanged(nextClTimerFrames);
	}
}
