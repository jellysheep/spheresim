#ifndef _OPENCL_CALCULATOR_H_
#define _OPENCL_CALCULATOR_H_

#include <QThread>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl_platform.h>
#include "CL/cl.hpp"

#include "Circles.h"
#include "Calculator.h"

class GLWidget;

class OpenClCalculator: public Calculator {

	Q_OBJECT // must include this if you use Qt signals/slots

protected:
	GLWidget* glWidget;
	char* file_contents(const char *filename, int *length);
	const char* oclErrorString(cl_int error);
	
	void save();

	unsigned int deviceUsed;
	std::vector<cl::Device> devices;
	
	cl::Context context;

	cl::CommandQueue queue;
	cl::Program program;
	cl::Kernel moveStep_kernel;
	cl::Kernel randomFill_kernel;
	
	cl::Kernel moveStep_addInterForces_kernel;
	cl::Kernel moveStep_addWallForces_kernel;
	cl::Kernel moveStep_updatePositions_kernel;

	cl::Buffer cl_circles;
	cl::Buffer cl_boxSize, cl_size;
	cl::Buffer cl_m_z, cl_m_w;
	cl::Buffer cl_max_speed;
	cl::Buffer cl_circlesCount, cl_E;
	cl::Buffer cl_elastic, cl_gravity, cl_timeInterval;
	cl::Buffer cl_poisson, cl_G;
		
	cl_int err;
	cl::Event event;
	static const unsigned int numEvents = 2;
	unsigned int eventCounter;
	bool eventsFull;
	cl::Event* events;
	FILE * file;
	
    int readNum_save, readNum_render, bufferReadIndex, bufferWriteIndex;
    //Circle* c_CPU_render[2];
    Circle** c_CPU_render;
    Circle* c_CPU_save[2];
    Circle* circlesBuffer;
    bool circlesBufferUsed;
    
    long elapsedFrames;
    
    void run();
	
	bool newFrame;
	bool running, hasStopped;

public:
	OpenClCalculator();
	
	void set(GLWidget* w);
	
	Circle* getCirclesBuffer();
	
	void paintGL(bool readNewFrame);
    
	void fpsChanged(scalar fps);

	friend void start(OpenClCalculator* clTimer);
	
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

extern void start(OpenClCalculator* clTimer);

#endif  /* _OPENCL_CALCULATOR_H_ */

