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
	
public:
	virtual void set(GLWidget* w)=0;
	
	virtual void paintGL(bool readNewFrame)=0;
    
	virtual void fpsChanged(scalar fps)=0;

	virtual scalar getFrameBufferLoad()=0;
	
	virtual bool getNewFrame()=0;
	
	virtual void setNewFrame(bool newFrame)=0;
	
    CircleExtension* ceBuffer;
    
    virtual bool getRunning()=0;
    
    virtual Circle* getCirclesBuffer()=0;
	
public slots:
    virtual void start()=0;
    
    virtual void stop()=0;
    
    virtual void boxSizeChanged()=0;
    
	virtual void gravityChanged()=0;
};

#endif

