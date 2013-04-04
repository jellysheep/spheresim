

#include "EigenCalculator_EfficientPairCollider.h"


#define _G_ 1
#define newFor 0
#define joinFors 1

template <int dims, bool _3D_>
void EigenCalculator_EfficientPairCollider<dims,_3D_>::calcForces(){
	C::calcForces();
	
	parallelFor
	for(int i = 0; i<spheresCount; i++){
		eVector force;
		
		eVector d_pos, d_n;
		scalar d, d_, R, _E_;
	#if newFor
		int j;
		for(int a = 0; a<x; a++){
			j = (i+a+1)%spheresCount;
	#else
		for(int j = i+1; j<spheresCount; j++){
	#endif
			
			//#define c2 sphere[i]
			//both_r = spheres[i].size + spheres[j].size;
			//both_r = this->both_r[i][j];
			
			#if _G_
			if(G==0)
			#endif
			{
				if(abs(C::gridIndex[i][0]-C::gridIndex[j][0]) >1) continue;
				if(abs(C::gridIndex[i][1]-C::gridIndex[j][1]) >1) continue;
				if(_3D_){
					if(abs(C::gridIndex[i][2]-C::gridIndex[j][2]) >1) continue;
				}
			}
			
			P::collideBalls(i,j);
		}
	}
}

template class EigenCalculator_EfficientPairCollider<2,false>;
template class EigenCalculator_EfficientPairCollider<3,true>;
