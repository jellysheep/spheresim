
#ifndef _EIGEN_CALCULATOR_COLLIDER_H_
#define _EIGEN_CALCULATOR_COLLIDER_H_

#include "EigenCalculator_Collider.h"

template <int dims, bool _3D_>
class EigenCalculator_Engine<dims,_3D_>;

template <int dims, bool _3D_>
class EigenCalculator_PairCollider : public EigenCalculator_Collider{
protected:
	int count;
	
public:
	template <int dims, bool _3D_>
	EigenCalculator_PairCollider(EigenCalculator_Engine<dims,_3D_>* c):EigenCalculator_Collider(c){
	}
	
	template <int dims, bool _3D_>
	void calcCollisionForces(){
		for(int i = 0; i<count-1; i++){
			for(int j = i+1; j<count; j++){
				collideBalls(i,j);
			}
		}
	}
	
	virtual void spheresCountChanged(int spheresCount){
		count = spheresCount;
	}
};

#endif  /*_EIGEN_CALCULATOR_COLLIDER_H_*/

