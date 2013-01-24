#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

#include <QThread>
#include "Circles.h"
#include "FramesCounter.h"

class GLWidget;

class Calculator : public QThread, public FramesCounter{
	Q_OBJECT
public:
	virtual void set(GLWidget* w)=0;
	
	virtual void paintGL(bool readNewFrame)=0;
    
	virtual void fpsChanged(scalar fps)=0;

	virtual scalar getFrameBufferLoad()=0;
	
	virtual bool getNewFrame()=0;
	
	virtual void setNewFrame(bool newFrame)=0;
	
    CircleExtension* ceBuffer;
    
    virtual bool getRunning()=0;
	
public slots:
    virtual void start()=0;
    
    virtual void stop()=0;
    
    virtual void boxSizeChanged()=0;
};

#endif

