
#include "EigenCalculator_StripeCollider.h"


template <int dims, bool _3D_>
void EigenCalculator_StripeCollider<dims,_3D_>::sort(Pos* p, int dim){
	//scalar temp;
	int temp;
	int j, cid; //sphere ID

	for (int i=1; i < spheresCount; i++){
		cid = p[i].sphereAtPos;
		//temp = spheresPos[cid](dim);
		temp = C::gridIndex[cid][dim];
		j = i-1;

		//while (j >= 0 && spheresPos[p[j].sphereAtPos](dim) > temp){
		while (j >= 0 && C::gridIndex[p[j].sphereAtPos][dim] > temp){
			p[j+1].sphereAtPos = p[j].sphereAtPos;
			j--;
		}

		p[j+1].sphereAtPos = cid;
	}
}

template <int dims, bool _3D_>
void EigenCalculator_StripeCollider<dims,_3D_>::calcForces(){
	C::calcForces();
	
	sort(posX, 0);
	/*
	for(int pid = 0; pid<spheresCount; pid++){ // pos. ID
		printf("%3d ", posX[pid].sphereAtPos);
	}
	printf("\n"); // */

	/// Kugeln kollidieren nur mit anderen, die rechts davon liegen.
	/// Dadurch werden doppelt berechnete Kollisionen vermieden.
	
	parallelFor
	for(int pid = 0; pid<spheresCount; pid++){ // pos. ID
		int i = posX[pid].sphereAtPos, j; //sphere IDs
		posX[i].posOfSphere = pid;
		for(int pid2 = pid+1; pid2<spheresCount; pid2++){
			j = posX[pid2].sphereAtPos;
			if((C::gridIndex[j][0] <= C::gridIndex[i][0]+1)){
			//if(abs(gridIndex[i][0]-gridIndex[j][0]) <=1){
			//if(spheresPos[j][0]-spheresPos[i][0] < both_r[i][j]){
				if(abs(C::gridIndex[i][1]-C::gridIndex[j][1]) <=1){
					if((!_3D_) || (abs(C::gridIndex[i][2]-C::gridIndex[j][2]) <=1)){
						P::collideBalls(i, j);
					}
				}
			}else{
				break;
			}
		}
	}
}

template class EigenCalculator_StripeCollider<2,false>;
template class EigenCalculator_StripeCollider<3,true>;
