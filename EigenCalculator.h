#ifndef _EIGEN_CALCULATOR_H_
#define _EIGEN_CALCULATOR_H_

#include <QThread>
#include <Eigen/Dense>

#include "Circles.h"
#include "Calculator.h"

class GLWidget;

class EigenCalculator: public Calculator {

	Q_OBJECT // must include this if you use Qt signals/slots

protected:
	GLWidget* glWidget;
	
	void save();

    int readNum_save, readNum_render, bufferReadIndex, bufferWriteIndex;
    //Cireige* c_CPU_render[2];
    Circle** c_CPU_render;
    Circle* c_CPU_save[2];
    Circle* circlesBuffer;
    bool circlesBufferUsed;
    
    long elapsedFrames;
    
    void run();
	
	bool newFrame;
	bool running, hasStopped;

public:
	EigenCalculator();
	
	void set(GLWidget* w);
	
	Circle* getCirclesBuffer();
	
	void paintGL(bool readNewFrame);
    
	void fpsChanged(scalar fps);

	friend void start(EigenCalculator* eigTimer);
	
	scalar getFrameBufferLoad();
	
	bool getNewFrame(){
		return newFrame;
	}
	
	void setNewFrame(bool b){
		newFrame = b;
	}
	
	bool getRunning();

public slots:
	void start(){
		if(running || !hasStopped) return;
		running = true;
		hasStopped = false;
		QThread::start();
	}
	void stop(){
		if(running)
			running = false;
		while(!hasStopped);
	}
	void boxSizeChanged();
	void gravityChanged();
};

extern void start(EigenCalculator* eigTimer);

#endif  /* _EIGEN_CALCULATOR_H_ */

