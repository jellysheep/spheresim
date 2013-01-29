#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

#include <QThread>
#include "Circles.h"
#include "FramesCounter.h"
#include <cstdio>
#include <cstdlib>

class GLWidget;

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
    int readNum_save, readNum_render, bufferReadIndex, bufferWriteIndex;
	
	bool newFrame;
	bool running, hasStopped;
	
public:
	Calculator();
	
	void set(GLWidget* w);
	
	virtual void paintGL(bool readNewFrame)=0;
    
	virtual void fpsChanged(scalar timeInterval)=0;
	
	scalar getFrameBufferLoad();
	
	virtual bool getNewFrame()=0;
	
	virtual void setNewFrame(bool newFrame)=0;
	
    CircleExtension* ceBuffer;
    
    bool getRunning();
    
    virtual Circle* getCirclesBuffer()=0;
	
public slots:
	void start(){
		printf("starting calculator...");
		if(running || !hasStopped) return;
		running = true;
		hasStopped = false;
		QThread::start();
		printf("started calculator! \n");
	}
	void stop(){
		if(running)
			running = false;
		while(!hasStopped);
	}
    
    virtual void boxSizeChanged()=0;
    
	virtual void gravityChanged()=0;
};

#endif

