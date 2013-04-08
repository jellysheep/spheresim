
#ifndef _EIGEN_CALCULATOR_STRIPE_COLLIDER_H_
#define _EIGEN_CALCULATOR_STRIPE_COLLIDER_H_

#include "EigenCalculator_Force.h"
#include "EigenCalculator_CellForce.h"
#include "EigenCalculator_PairCollider.h"

template <int dims, bool _3D_>
class EigenCalculator_Engine;

template <int dims, bool _3D_>
class EigenCalculator_StripeCollider : 
	protected EigenCalculator_CellForce<dims,_3D_>,
	public EigenCalculator_PairCollider<dims,_3D_>{
protected:
	
	Pos *posX, *posY, *posZ; //positions of spheres sorted by axes
	void sort(Pos* p, int dim);
	
	Pos* spheresCountChanged(Pos* p, int c);

public:
	EigenCalculator_StripeCollider(EigenCalculator_Engine<dims,_3D_>* c) :
		EigenCalculator_Force<dims,_3D_>(c),
		EigenCalculator_CellForce<dims,_3D_>(c),
		EigenCalculator_PairCollider<dims,_3D_>(c){
		posX = new Pos[spheresCount];
		posY = new Pos[spheresCount];
		if(_3D_){
			posZ = new Pos[spheresCount];
		}
		//parallelFor
		for(int i = 0; i<spheresCount; i++){
			posX[i].posOfSphere = i;
			posX[i].sphereAtPos = i;
			posY[i].posOfSphere = i;
			posY[i].sphereAtPos = i;
			if(_3D_){
				posZ[i].posOfSphere = i;
				posZ[i].sphereAtPos = i;
			}
		}
	}
	
	virtual void calcForces();
	
	virtual void spheresCountChanged(int c);
};

#endif  /*_EIGEN_CALCULATOR_STRIPE_COLLIDER_H_*/

