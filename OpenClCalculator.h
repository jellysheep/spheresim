#ifdef ENGINE_OPENCL

#ifndef _OPENCL_CALCULATOR_H_
#define _OPENCL_CALCULATOR_H_

#include <QThread>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl_platform.h>
#include "CL/cl.hpp"

#include "Circles.h"
#include "Calculator.h"

class OpenClCalculator: public Calculator {

	Q_OBJECT // must include this if you use Qt signals/slots

protected:
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
	cl::Buffer cl_boxSize, cl_sphereSize;
	cl::Buffer cl_m_z, cl_m_w;
	cl::Buffer cl_max_speed;
	cl::Buffer cl_circlesCount, cl_E;
	cl::Buffer cl_elastic, cl_gravity, cl_timeInterval;
	cl::Buffer cl_poisson, cl_G;
	cl::Buffer cl_flags;
		
	cl_int err;
	cl::Event event;
	static const unsigned int numEvents = 2;
	unsigned int eventCounter;
	bool eventsFull;
	cl::Event* events;
	FILE * file;
	bool circlesBufferUsed;
	
	Circle** c_CPU_render;
	Circle* c_CPU_save[2];
	Circle* circlesBuffer;
	
	void doStep();
	bool saveFrame();
	
	void circleCountChanged_subclass(int i);
	void maxCircleCountChanged_subclass(int i);
	
	bool isFixed(int i);

public:
	OpenClCalculator();
	
	Circle* getCircle(int i);
	Circle* getDirectCircle(int i);
	
	void fpsChanged(scalar timeInterval);
	
	scalar getTemperature(){
		return 0;
	}
	
public slots:
	void boxSizeChanged();
	void gravityChanged();
	void updateG();
	void updateAirResistance();
	void updateWallResistance();
	void updateEModul();
	void updatePoisson();
	void updateElasticity();
	void updateSphereSize();
	void loadConfig();
};

extern void start(OpenClCalculator* clTimer);

#endif  /* _OPENCL_CALCULATOR_H_ */

#endif
