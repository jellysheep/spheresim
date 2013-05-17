#ifdef ENGINE_CUDA

#ifndef _CUDA_CALCULATOR_CUH_
#define _CUDA_CALCULATOR_CUH_

#include "Spheres.h"

template <int dims, bool _3D_>
class cuCudaCalculator {

protected:
	
	//eVector** renderBuffer;
	Sphere* spheres;
	//eVector *spheresOldPos, *spheresPos, *spheresSpeed, *spheresForce;
	scalar** both_r;
	scalar gridWidth;
	
	void cu_doStep();
	bool cu_saveFrame();
	
	void cu_save();
	
	bool cu_isFixed(int i);
	
	void cu_loadConfig(const char* file);
	
	Sphere c;
	
	scalar timeInterval;
	
	void cu_sphereCountChanged_subclass(int i);
	void cu_maxSphereCountChanged_subclass(int i);
	
	void cu_initSphere(int i);
	
	scalar _E;
	
	void cu_calcWallResistance();
	
	int maxNumForces;
	//EigenCalculator_Force<dims,_3D_>** forces;
	int numForces;
	
	void cu_methodEulerPolygon();
	void cu_methodRungeKutta();
	void cu_addStandardForces();
	eVector** k;

public:
	cuCudaCalculator();
	
	void cu_paintGL(bool b);
	
	void cu_fpsChanged(scalar timeInterval);
	
	Sphere* cu_getSphere(int i);
	Sphere* cu_getDirectSphere(int i);
	
	scalar cu_getTemperature();
	
	void cu_boxSizeChanged();
	void cu_gravityChanged();
	void cu_updateG();
	void cu_updateAirResistance();
	void cu_updateWallResistance();
	void cu_updateEModul();
	void cu_updatePoisson();
	void cu_updateElasticity();
	void cu_updateGridSize();
	
	void cu_loadConfig();
	
	void cu_setGridWith(scalar s);
	
	void cu_updateMagnitude();
};

#endif  /* _CUDA_CALCULATOR_CUH_ */

#endif
