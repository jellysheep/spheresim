
#include "EigenCalculator_PairCollider.h"

template <int dims, bool _3D_>
void EigenCalculator_PairCollider<dims,_3D_>::calcForces(){
	if(!ballResistance) return;
	F::calcForces();
	
	for(int i = 0; i<spheresCount-1; i++){
		for(int j = i+1; j<spheresCount; j++){
			collideBalls(i,j);
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_PairCollider<dims,_3D_>::spheresCountChanged(int c){
	F::spheresCountChanged(c);
}

template class EigenCalculator_PairCollider<2,false>;
template class EigenCalculator_PairCollider<3,true>;
