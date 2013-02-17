#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

#include <QThread>
#include "Circles.h"
#include "FramesCounter.h"
#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

class GLWidget;
class PlotWidget;

class Calculator : public QThread, public FramesCounter{
	Q_OBJECT
protected:
	GLWidget* glWidget;
	
	static char hex(int i){
		if(i<0 || i>15){
			return '0';
		}else if(i<10){
			return '0'+i;
		}else{
			return 'A'+(i-10);
		}
	}
	static void add(FILE* file, double d){
		unsigned char* c = (unsigned char*)&d;
		unsigned int x = 0;
		for(int i = 0; i<8; i++){
			fprintf(file, "%c%c", hex(c[i]/16), hex(c[i]%16));
		}
	}
	
	void run();
	
	virtual void doStep()=0;
	virtual void saveFrame()=0;
	
	virtual void save()=0;
	
	long elapsedFrames;
	int readNum_render, bufferReadIndex, bufferWriteIndex;
	
	bool newFrame;
	volatile bool running, hasStopped;
	
	virtual void circleCountChanged_subclass(int i)=0;
	virtual void maxCircleCountChanged_subclass(int i)=0;
	
	int lightTarget;
	void initCeBuffer(int i);
	bool performingAction;
	
	PlotWidget* plotWg;
	int numWalls;
	int forceCounter, forceCounter2;
	bool bufferFilled, plotNext;
	scalar **wallForces, *curWallForces;
	
	fstream f;
	
public:
	Calculator();
	
	void set(GLWidget* w);
	
	virtual void paintGL(bool readNewFrame);
	
	virtual void fpsChanged(scalar timeInterval)=0;
	
	scalar getFrameBufferLoad();

	bool getNewFrame(){
		return newFrame;
	}
	
	void setNewFrame(bool b){
		newFrame = b;
	}
	
	CircleExtension* ceBuffer;
	
	bool isRunning();
	
	virtual Circle* getCircle(int i)=0;
	
	void set(PlotWidget* plotWg);
	
	void initFileSave();
	void stopFileSave();
	
public slots:
	void start(){
		printf("starting calculator... ");
		if(running || !hasStopped){
			printf("already running!\n");
			return;
		}
		running = true;
		hasStopped = false;
		QThread::start();
		printf("started calculator! \n");
	}
	void stop(){
		if(running)
			running = false;
		printf("wait for hasStopped==true\n");
		while(hasStopped == false);
		printf("return from stop\n");
	}
	
	virtual void boxSizeChanged()=0;
	
	virtual void gravityChanged()=0;
	
	virtual void circleCountChanged(int i);
	
	virtual void maxCircleCountChanged(int i);
	
	virtual void updateG()=0;
	
	virtual void updateAirResistance()=0;
	
	virtual void updateWallResistance()=0;
	
	virtual void updateEModul()=0;
	
	virtual void updatePoisson()=0;
	
	virtual void updateElasticity()=0;
	
	virtual void updateSphereSize()=0;
};

#endif

