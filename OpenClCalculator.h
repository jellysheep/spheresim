#ifdef ENGINE_OPENCL

#ifndef _OPENCL_CALCULATOR_H_
#define _OPENCL_CALCULATOR_H_

#include <QThread>

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl_platform.h>
#include "CL/cl.hpp"

#include "Spheres.h"
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

	cl::Buffer cl_spheres;
	cl::Buffer cl_boxSize, cl_sphereSize;
	cl::Buffer cl_m_z, cl_m_w;
	cl::Buffer cl_max_speed;
	cl::Buffer cl_spheresCount, cl_E;
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
	bool spheresBufferUsed;
	
	Sphere** c_CPU_render;
	Sphere* c_CPU_save[2];
	Sphere* spheresBuffer;
	
	void doStep();
	bool saveFrame();
	
	void sphereCountChanged_subclass(int i);
	void maxSphereCountChanged_subclass(int i);
	
	bool isFixed(int i);

public:
	OpenClCalculator();
	
	Sphere* getSphere(int i);
	Sphere* getSphereWithSpeed(int i);
	Sphere* getDirectSphere(int i);
	void setSphere(int i, Sphere* s);
	
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
	void updateGridSize();
	void loadConfig();
	void updateMagnitude();
};

extern void start(OpenClCalculator* clTimer);

#endif  /* _OPENCL_CALCULATOR_H_ */

#endif
