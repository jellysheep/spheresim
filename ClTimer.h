#ifndef _CLTIMER_H_
#define _CLTIMER_H_

#include <QThread>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl_platform.h>
#include "CL/cl.hpp"

#include "CirclesHolder.h"
#include "FramesCounter.h"

class ClTimer: public QThread, public FramesCounter {

	Q_OBJECT // must include this if you use Qt signals/slots

protected:
	char* file_contents(const char *filename, int *length);
	const char* oclErrorString(cl_int error);

	void run();
	CirclesHolder* circlesHolder;
	
	unsigned int deviceUsed;
	std::vector<cl::Device> devices;
	
	cl::Context context;

	cl::CommandQueue queue;
	cl::Program program;
	cl::Kernel moveStep_kernel;
	cl::Kernel randomFill_kernel;

	cl::Buffer cl_circle;
	cl::Buffer cl_size;
	cl::Buffer cl_m_z, cl_m_w;
	cl::Buffer cl_max_speed;
	cl::Buffer cl_circlesCount, cl_E;
		
	cl_int err;
	cl::Event event;
	static const unsigned int numEvents = 10;
	unsigned int eventCounter;
	bool eventsFull;
	cl::Event* events;

public:
	ClTimer(CirclesHolder* ch);
};

#endif  /* _CLTIMER_H_ */

