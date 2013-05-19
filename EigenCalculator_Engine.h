#ifdef ENGINE_CPP

#ifndef _EIGEN_CALCULATOR_ENGINE_H_
#define _EIGEN_CALCULATOR_ENGINE_H_

#include "EigenCalculator_defines.h"

#include <QThread>

#include "Spheres.h"
#include "Calculator.h"
#include "EigenCalculator_Force.h"

template <int dims, bool _3D_>
class EigenCalculator_Force;

template <int dims, bool _3D_>
class EigenCalculator_Engine: public Calculator {

protected:
	
	friend class EigenCalculator_Force<dims,_3D_>;
		
	eVector** renderBuffer;
	Sphere* spheres;
	eVector *spheresOldPos, *spheresPos, *spheresSpeed, *spheresForce;
	scalar** both_r;
	scalar gridWidth;
	
	void doStep();
	bool saveFrame();
	
	void save();
	
	bool isFixed(int i);
	
	void loadConfig(const char* file);
	
	Sphere c;
	
	scalar timeInterval;
	
	void sphereCountChanged_subclass(int i);
	void maxSphereCountChanged_subclass(int i);
	
	void initSphere(int i);
	
	scalar _E;
	
	void calcWallResistance();
	
	int maxNumForces;
	EigenCalculator_Force<dims,_3D_>** forces;
	int numForces;
	
	template<class Force>
	void addForce();
	
	void addForce(EigenCalculator_Force<dims,_3D_>* force);
	
	void methodEulerPolygon();
	void methodRungeKutta();
	void methodLeapfrog();
	bool leapfrogFirstStepMade;
	void addStandardForces();
	eVector** k;

public:
	EigenCalculator_Engine();
	
	void paintGL(bool b);
	
	void fpsChanged(scalar timeInterval);
	
	Sphere* getSphere(int i);
	Sphere* getSphereWithSpeed(int i);
	Sphere* getDirectSphere(int i);
	void setSphere(int i, Sphere* s);
	
	scalar getTemperature();
	
	void boxSizeChanged();
	void gravityChanged();
	void updateG();
	void updateAirResistance();
	void updateWallResistance();
	void updateEModul();
	void updatePoisson();
	void updateElasticity();
	void updateGridSize();
	
	void loadConfig_();
	
	void setGridWith(scalar s);
	
	void updateMagnitude();
};

#endif  /* _EIGEN_CALCULATOR_ENGINE_H_ */

#endif
