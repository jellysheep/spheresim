
#ifndef _EIGEN_CALCULATOR_PAIR_COLLIDER_H_
#define _EIGEN_CALCULATOR_PAIR_COLLIDER_H_

#include "EigenCalculator_Force.h"

template <int dims, bool _3D_>
class EigenCalculator_Engine;

#define P EigenCalculator_PairCollider<dims,_3D_>

template <int dims, bool _3D_>
class EigenCalculator_PairCollider : public virtual EigenCalculator_Force<dims,_3D_>{
protected:
	void collideBalls(int i, int j);
	
public:
	EigenCalculator_PairCollider(EigenCalculator_Engine<dims,_3D_>* c):EigenCalculator_Force<dims,_3D_>(c){
	}
	
	virtual void calcForces();
	
	virtual void spheresCountChanged(int c);
};

#endif  /*_EIGEN_CALCULATOR_PAIR_COLLIDER_H_*/

