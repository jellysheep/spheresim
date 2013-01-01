#ifndef _CLTIMER_H_
#define _CLTIMER_H_

#include <QThread>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl_platform.h>
#include "CL/cl.hpp"

#include "CirclesHolder.h"
#include "FramesCounter.h"
#include "Circles.h"

class ClTimer: public QThread, public FramesCounter {

	Q_OBJECT // must include this if you use Qt signals/slots

protected:
	char* file_contents(const char *filename, int *length);
	const char* oclErrorString(cl_int error);
	
	char hex(int i);
	void add(double d);
	void save(int readNum, Circle** c_CPU);

	void run();
	CirclesHolder* circlesHolder;
	
	unsigned int deviceUsed;
	std::vector<cl::Device> devices;
	
	cl::Context context;

	cl::CommandQueue queue;
	cl::Program program;
	cl::Kernel moveStep_kernel;
	cl::Kernel randomFill_kernel;

	cl::Buffer cl_circles;
	cl::Buffer cl_boxSize;
	cl::Buffer cl_m_z, cl_m_w;
	cl::Buffer cl_max_speed;
	cl::Buffer cl_circlesCount, cl_E;
	cl::Buffer cl_elastic, cl_gravity;
		
	cl_int err;
	cl::Event event;
	static const unsigned int numEvents = 2;
	unsigned int eventCounter;
	bool eventsFull;
	cl::Event* events;
	FILE * file;
	
    int readNum;
    Circle* c_CPU[2];
    Circle* circlesBuffer;
    bool circlesBufferUsed;

public:
	ClTimer();
	
	cl::Buffer* getCirclesBuffer(){
		return &cl_circles;
	}
	
	void paintGL(cl_double3 rotation, double translateZ);
};

#endif  /* _CLTIMER_H_ */

