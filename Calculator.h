#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

#include <QThread>
#include "Circles.h"
#include "FramesCounter.h"
#include <cstdio>
#include <cstdlib>

#include <fstream>

class GLWidget;
class PlotWidget;

class Calculator : public QThread, public FramesCounter{
	Q_OBJECT
protected:
	GLWidget* glWidget;
	
	static void addHex(std::ostream &f, float fl, bool newLine);
	static void addHex(std::ostream &f, double d, bool newLine);
	static void addHex(std::ostream &f, int i, bool newLine);
	
	void run();
	
	virtual void doStep()=0;
	virtual bool saveFrame()=0;
	
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
	
	std::fstream f, f2;
	
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
	
	///get latest circles (at 'write' position in buffer)
	virtual Circle* getDirectCircle(int i)=0;
	
	void set(PlotWidget* plotWg);
	
	void initFileSave();
	void stopFileSave();
	
	virtual scalar getTemperature()=0;
	
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
	void quit(){
		stop();
		if(saveBool){
			stopFileSave();
		}
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
	
	void saveFrameToFile();
	
	void saveConfig();
};

#endif

