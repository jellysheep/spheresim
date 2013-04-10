#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

#include <QThread>
#include "Spheres.h"
#include "FramesCounter.h"
#include <cstdio>
#include <cstdlib>

#include <fstream>
#include <iostream>
#include <sstream>

#include <QDateTime>

class GLWidget;
class PlotWidget;

//#define _read(f,stream,var){														\
	for(int _i = 0; !(stream>>(hexadec?std::hex:std::dec)>>var) && _i<20; _i++){	\
		readLine(f);																\
	}																				\
}

//#define saveInVar(stream,x) {				\
	saveInVar_(stream,x);					\
	std::cout<<#x<<": "<<x<<"\n";		\
}

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
	
	virtual void sphereCountChanged_subclass(int i)=0;
	virtual void maxSphereCountChanged_subclass(int i)=0;
	
	int lightTarget;
	void initCeBuffer(int i);
	bool performingAction;
	
	PlotWidget* plotWg;
	int numWalls;
	int forceCounter, forceCounter2;
	bool bufferFilled, plotNext;
	scalar **wallForces, *curWallForces;
	
	std::fstream f, f2;
	
	std::istringstream iss;
	std::string line;
	float readHexFloat(std::fstream& f);
	float readFloat(std::fstream& f);
	void saveInVar_(std::fstream& f, scalar &s);
	void saveInVar_(std::fstream& f, int &i);
	void readLine(std::fstream& f);
	bool hexadec, eof, initialized;
	
	template<typename T>
	void _read(std::fstream& f, std::istringstream& stream, T& var){														\
		for(int _i = 0; !(stream>>(hexadec?std::hex:std::dec)>>var) && _i<20; _i++){	\
			readLine(f);																\
		}																				\
	}
	
	template<typename T>
	void saveInVar(std::fstream& stream, T& x) {
		saveInVar_(stream,x);
		std::cout<<"read var: "<<x<<"\n";
	}
	
	virtual bool isFixed(int i)=0;
	
	QDateTime simStartDate; //when simulation was started, or as in .s2c/.s3c file.
	QDateTime curSimTime;
	QTime curTimer;
	
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
	
	SphereExtension* ceBuffer;
	
	bool isRunning();
	
	virtual Sphere* getSphere(int i)=0;
	
	///get latest spheres (at 'write' position in buffer)
	virtual Sphere* getDirectSphere(int i)=0;
	
	void set(PlotWidget* plotWg);
	
	void initFileSave();
	void stopFileSave();
	
	virtual scalar getTemperature()=0;
	
	QDateTime getCurSimTime(){
		if(running){
			curSimTime = simStartDate.addMSecs(speed*curTimer.elapsed());
		}
		return curSimTime;
	}
	
	void updateSpeed(double s){
		if(running){
			simStartDate = simStartDate.addMSecs(speed*curTimer.restart());
		}
		speed = s;
	}
	
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
		simStartDate = curSimTime;
		curTimer.start();
		printf("started calculator! \n");
	}
	void stop(){
		if(running)
			running = false;
		printf("wait for hasStopped==true\n");
		while(hasStopped == false){
			curSimTime = simStartDate.addMSecs(speed*curTimer.elapsed());
		}
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
	
	virtual void sphereCountChanged(int i);
	
	virtual void maxSphereCountChanged(int i);
	
	virtual void updateG()=0;
	
	virtual void updateAirResistance()=0;
	
	virtual void updateWallResistance()=0;
	
	virtual void updateEModul()=0;
	
	virtual void updatePoisson()=0;
	
	virtual void updateElasticity()=0;
	
	virtual void updateGridSize()=0;
	
	void saveFrameToFile();
	
	void saveConfig();
	
	virtual void loadConfig() = 0;
	
	virtual void updateMagnitude()=0;

signals:
	virtual void fileLoaded();
};

#endif

