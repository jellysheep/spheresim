#include "StatusViewer.h"
#include "GLWidget.h"
#include "Calculator.h"
#include <cstdio>
#include <cmath>

#define sign(x) (((x)<0)?(-1):(1))

StatusViewer::StatusViewer(GLWidget* glw, Calculator* clt) {
	statusTimer = new QTimer(this);
	statusTimer->setInterval(500);
	QObject::connect(statusTimer, SIGNAL(timeout()), this, SLOT(updateTimer()), Qt::AutoConnection);
	statusTimer->start();
	glWidget = glw;
	clTimer = clt;
	lastGlWidgetFrames = 0;
	lastCalculatorFrames = 0;
	
	QObject::connect(this, SIGNAL(destroyed()), 
		this, SLOT(stop()), Qt::AutoConnection);
	running = true;
}

void StatusViewer::run() {
	while(running){
		msleep(500);
		
		updateTimer();
	}
}

void StatusViewer::updateTimer(){
	printf("\nStatusViewer: \n");
	scalar factor = getElapsedNS()/1000000000.0;
	printf("Elapsed:	%8.2f seconds\n", factor);
	
	scalar glWidgetFrames = 0;
	if(glWidget){
		glWidgetFrames = glWidget->popFramesCounter() / factor;
		if(lastGlWidgetFrames == 0){
			lastGlWidgetFrames = glWidgetFrames;
		}else{
			lastGlWidgetFrames *= (1-f);
			lastGlWidgetFrames += f*glWidgetFrames;
		}
		printf("GlWidget:   %8.2f fps	   [now: %8.2f fps]\n", lastGlWidgetFrames, glWidgetFrames);
	}
	
	scalar frameBufferLoad = clTimer->getFrameBufferLoad();
	//printf("frameBufferLoad:  %6.4f\n", frameBufferLoad);
		
	scalar clTimerFrames = clTimer->popFramesCounter() / factor;
	if(lastCalculatorFrames == 0){
		lastCalculatorFrames = clTimerFrames;
	}else{
		lastCalculatorFrames *= (1-f);
		lastCalculatorFrames += f*clTimerFrames;
	}
	printf("Calculator:	%8.2f fps	   [now: %8.2f fps]\n", lastCalculatorFrames, clTimerFrames);
		
	if(clTimer->isRunning()){
		double nextCalculatorFrames = clTimerFrames;
		if(nextCalculatorFrames <= 0){
			nextCalculatorFrames = 1000000;
		}
		if(renderBool){
			static scalar bufferLoadTarget = 0.65;
			calcSpeedFact = (1
				//*
				+0.05*(pow(16,pow((frameBufferLoad>bufferLoadTarget)?
					(1-((1-frameBufferLoad)*0.5/(1-bufferLoadTarget))):
					(frameBufferLoad*0.5/bufferLoadTarget),2)))*
				sign(frameBufferLoad-bufferLoadTarget) // */
				);
			nextCalculatorFrames = lastCalculatorFrames*calcSpeedFact;
			
			printf("frameBuffer:%10.4f => Calculator:   %8.2f fps\n", frameBufferLoad, nextCalculatorFrames);
			//renderFps = min((int)nextCalculatorFrames, renderFpsMax);
			//glWidget->rotationTimer->setInterval(1000/renderFps);
		}
		
		if(nextCalculatorFrames < (speed*minFps)){
			speedCorrection = nextCalculatorFrames / (speed*minFps);
		}else{
			speedCorrection = 1.0;
		}

		fps = nextCalculatorFrames;
		double temp = clTimer->getTemperature();
		printf("Temperature: %f\n", temp);
		emit temperatureChanged(temp);
	}else{
		fps = 1000000;
		clTimerFrames = 1000000;
	}
		
	if(speed!=0 && fps!=0 && speedCorrection!=0){
		scalar timeInterval = speed*speedCorrection/fps;
		//printf("timeInterval: %10f\n", timeInterval);
		clTimer->fpsChanged(timeInterval);
	}else{
		printf("ERROR! fpsChanged! \n");
		printf("speed: %6f fps: %6f speedCorr.: 6%f\n", speed, fps, speedCorrection);
	}
	
	emit fpsChanged(glWidgetFrames, clTimerFrames, frameBufferLoad, speed*std::min(1.0,(double)(clTimerFrames/(speed*minFps))));
}

void StatusViewer::stop(){
	running = false;
	statusTimer->stop();
	QThread::quit();
}
