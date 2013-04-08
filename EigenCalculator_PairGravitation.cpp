
#include "EigenCalculator_PairGravitation.h"

template <int dims, bool _3D_>
void EigenCalculator_PairGravitation<dims,_3D_>::calcGravitation(int i, int j){
	if(i == j) return;
	eVector d_pos = F::spheresPos[j]-F::spheresPos[i];
	
	//d_pos.s2 = 0;
	scalar d = d_pos.norm();
	//d = std::max(d,(scalar)0.00005f);
	d = std::max(d,std::max((scalar)F::spheres[i].size/50, (scalar)F::spheres[j].size/50));
	
	eVector d_n, force;
	
	d_n = d_pos/d;
	// bzw. normalize(d_pos);
	// Gravitation:
	force = G*G_fact*F::spheres[i].mass*F::spheres[j].mass/pow(d,2) *d_n;
	F::spheresForce[i] += force;
	
	F::spheresForce[j] -= force;
}
	
template <int dims, bool _3D_>
void EigenCalculator_PairGravitation<dims,_3D_>::calcForces(){
	F::calcForces();
	if(G_fact!=0){
		for(int i = 0; i<spheresCount-1; i++){
			for(int j = i+1; j<spheresCount; j++){
				calcGravitation(i,j);
			}
		}
	}
}

template class EigenCalculator_PairGravitation<2,false>;
template class EigenCalculator_PairGravitation<3,true>;
