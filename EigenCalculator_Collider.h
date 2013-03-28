
#ifndef _EIGEN_CALCULATOR_COLLIDER_H_
#define _EIGEN_CALCULATOR_COLLIDER_H_

#include "EigenCalculator_eVector.h"

#include "Spheres.h"

template <int dims, bool _3D_>
class EigenCalculator_Engine;

template <int dims, bool _3D_>
class EigenCalculator_Collider{
protected:
	EigenCalculator_Engine<dims,_3D_>* calc;
	
public:
	EigenCalculator_Collider(EigenCalculator_Engine<dims,_3D_>* c){
		calc = c;
	};
	
	virtual void calcCollisionForces()=0;
	
	virtual void spheresCountChanged(int spheresCount)=0;
};

#endif  /*_EIGEN_CALCULATOR_COLLIDER_H_*/
