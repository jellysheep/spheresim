
#ifndef _EIGEN_CALCULATOR_EFFICIENT_PAIR_COLLIDER_H_
#define _EIGEN_CALCULATOR_EFFICIENT_PAIR_COLLIDER_H_

#include "EigenCalculator_Force.h"
#include "EigenCalculator_CellForce.h"
#include "EigenCalculator_PairCollider.h"

template <int dims, bool _3D_>
class EigenCalculator_Engine;

template <int dims, bool _3D_>
class EigenCalculator_EfficientPairCollider :
	protected EigenCalculator_CellForce<dims,_3D_>,
	public EigenCalculator_PairCollider<dims,_3D_>{
protected:

public:
	EigenCalculator_EfficientPairCollider(EigenCalculator_Engine<dims,_3D_>* c) : 
		EigenCalculator_Force<dims,_3D_>(c),
		EigenCalculator_CellForce<dims,_3D_>(c),
		EigenCalculator_PairCollider<dims,_3D_>(c)
	{
	}
	
	virtual void calcForces();
};

#endif  /*_EIGEN_CALCULATOR_EFFICIENT_PAIR_COLLIDER_H_*/

