
#include "EigenCalculator_CellGravitation.h"
#include "EigenCalculator_CellCountCollider.h"

template <int dims, bool _3D_>
EigenCalculator_CellGravitation<dims,_3D_>::EigenCalculator_CellGravitation(
	EigenCalculator_Engine<dims,_3D_>* c, 
	EigenCalculator_CellCountCollider<dims,_3D_>* ccc)
	:EigenCalculator_Force<dims,_3D_>(c), cellCountCollider(ccc){
	
	numAllCells = ccc->numCells_*2;
	massVecSumOfCell = new eVector[numAllCells];
	massSumOfCell = new scalar[numAllCells];
	centerOfMassOfCell = new eVector[numAllCells];
}

template <int dims, bool _3D_>
void EigenCalculator_CellGravitation<dims,_3D_>::calcGravitation(int i, int j){
	if(i == j) return;
	eVector d_pos = F::spheresPos[j]-F::spheresPos[i];
	
	//d_pos.s2 = 0;
	scalar d = d_pos.norm();
	//d = std::max(d,(scalar)0.00005f);
	d = std::max(d,std::max((scalar)F::spheres[i].size/5, (scalar)F::spheres[j].size/5));
	
	eVector d_n, force;
	
	d_n = d_pos/d;
	// bzw. normalize(d_pos);
	// Gravitation:
	force = G*G_fact*F::spheres[i].mass*F::spheres[j].mass/pow(d,2) *d_n;
	F::spheresForce[i] += force;
	
	F::spheresForce[j] -= force;
}
	
template <int dims, bool _3D_>
void EigenCalculator_CellGravitation<dims,_3D_>::calcForces(){
	F::calcForces();
	if(G_fact!=0){
		numSpheresInCell = cellCountCollider->numSpheresInCell;
		spheresInCell = cellCountCollider->spheresInCell;
		numCells_ = cellCountCollider->numCells_;
		cellOfSphere = cellCountCollider->cellOfSphere;
		int i;
		///reset sums:
		for(i = 1; i<numAllCells; i++){
			massVecSumOfCell[i].setZero();
			massSumOfCell[i] = 0;
		}
		///sum up masses in smallest cells:
		for(i = 0; i<spheresCount; i++){
			massVecSumOfCell[numCells_+cellOfSphere[i]] += F::spheres[i].mass*F::spheresPos[i];
			massSumOfCell[numCells_+cellOfSphere[i]] += F::spheres[i].mass;
		}
		///sum up masses to parent cells and calculate center of mass:
		int parentCell;
		for(i = numAllCells-1; i>1; i--){
			if(massSumOfCell[i]!=0){
				parentCell = i>>1;
				massVecSumOfCell[parentCell] += massVecSumOfCell[i];
				massSumOfCell[parentCell] += massSumOfCell[i];
				
				centerOfMassOfCell[i] = massVecSumOfCell[i]/massSumOfCell[i];
			}
		}
		///calculate forces:
		int cellID, sphereID, j;
		eVector d_pos, d_n, force;
		scalar d;
		for(i = 0; i<spheresCount; i++){
			cellID = cellOfSphere[i];
			///pairwise gravitation in same cell:
			for(j = 0; j<numSpheresInCell[cellID]; j++){
				sphereID = spheresInCell[cellID][j];
				if(i>sphereID){
					calcGravitation(i, sphereID);
				}
			}
			cellID += numCells_;
			//printf("Sphere in cell %6d calculates gravitation to cells: %6d", cellID, cellID);
			///gravitation to center of mass in different cells:
			for(;cellID>1;cellID/=2){
				///go to other cell in parent:
				cellID ^= 1;
				//printf(", %6d", cellID);
				
				///calculate force:
				
				d_pos = centerOfMassOfCell[cellID]-F::spheresPos[i];
				
				d = d_pos.norm();
				//d = std::max(d,(scalar)0.00005f);
				d = std::max(d,(scalar)F::spheres[i].size/5);
				
				d_n = d_pos/d;
				// bzw. normalize(d_pos);
				/// Gravitation:
				force = G*G_fact*F::spheres[i].mass*massSumOfCell[cellID]/pow(d,2) *d_n;
				F::spheresForce[i] += force;
			}
			//printf("\n");
		}
	}
}

template class EigenCalculator_CellGravitation<2,false>;
template class EigenCalculator_CellGravitation<3,true>;

