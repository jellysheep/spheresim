
#ifndef _EIGEN_CALCULATOR_FORCE_H_
#define _EIGEN_CALCULATOR_FORCE_H_

#include "EigenCalculator_Engine.h"

#include "Spheres.h"

template <int dims, bool _3D_>
class EigenCalculator_Engine;

#define F EigenCalculator_Force<dims,_3D_>

template <int dims, bool _3D_>
class EigenCalculator_Force{
protected:
	EigenCalculator_Engine<dims,_3D_>* calc;
	
	Sphere* spheres;
	eVector *spheresPos, *spheresSpeed, *spheresForce;
	scalar** both_r;
	scalar _E;
	
public:
	EigenCalculator_Force(EigenCalculator_Engine<dims,_3D_>* c){
		calc = c;
	};
	
	virtual void calcForces(){
		spheres = calc->spheres;
		spheresPos = calc->spheresPos;
		spheresSpeed = calc->spheresSpeed;
		spheresForce = calc->spheresForce;
		both_r = calc->both_r;
		_E = calc->_E;
	}
	
	virtual void spheresCountChanged(int c){
		spheres = calc->spheres;
		spheresPos = calc->spheresPos;
		spheresSpeed = calc->spheresSpeed;
		spheresForce = calc->spheresForce;
		both_r = calc->both_r;
		_E = calc->_E;
	}
	
	virtual void paintGL(){}
};

#endif  /*_EIGEN_CALCULATOR_FORCE_H_*/
