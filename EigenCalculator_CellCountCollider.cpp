
#include "EigenCalculator_CellCountCollider.h"
#include <cstdio>

int x = 0;
template <int dims, bool _3D_>
void EigenCalculator_CellCountCollider<dims,_3D_>::countSpheresPerCell(){
	//no spheres in cells:
	for(int i = (C::numCells_)-1; i>=0; i--){
		numSpheresInCell[i] = 0;
	}
	
	//printf("%3d\n", x++);
	static const scalar fact = 1.0;
	bool tooManySpheres = false;
	parallelFor
	for(int i = 0; i<spheresCount; i++){
		//no spheres already colliding:
		numCollsPerSphere[i] = 0;
		//if(i == 999) printf(" %3d\n", i);
		volatile int minX, maxX, minY, maxY, minZ, maxZ, x, y, z, cellID;
		minX = (int)((F::spheresPos[i](0)-F::spheres[i].size*fact)/C::gridWidth);
		maxX = (int)((F::spheresPos[i](0)+F::spheres[i].size*fact)/C::gridWidth);
		minY = (int)((F::spheresPos[i](1)-F::spheres[i].size*fact)/C::gridWidth);
		maxY = (int)((F::spheresPos[i](1)+F::spheres[i].size*fact)/C::gridWidth);
		if(_3D_){
			minZ = (int)((F::spheresPos[i](2)-F::spheres[i].size*fact)/C::gridWidth);
			maxZ = (int)((F::spheresPos[i](2)+F::spheres[i].size*fact)/C::gridWidth);
		}
		for(x = minX; x<=maxX; x++){
			for(y = minY; y<=maxY; y++){
				if(_3D_){
					for(z = minZ; z<=maxZ; z++){
						cellID = C::calcCellID(x,y,z);
						if(numSpheresInCell[cellID]<maxNumSpheresInCell){
							spheresInCell[cellID][numSpheresInCell[cellID]++] = i;
						}else{
							//printf("Too many spheres in cell %5d!\n", cellID);
							tooManySpheres = true;
						}
					}
				}else{
					cellID = C::calcCellID(x,y);
				}
				if(numSpheresInCell[cellID]<maxNumSpheresInCell){
					spheresInCell[cellID][numSpheresInCell[cellID]++] = i;
				}else{
					//printf("Too many spheres in cell %5d!\n", cellID);
					tooManySpheres = true;
				}
			}
		}
	}
	if(tooManySpheres) printf("Too many spheres in cell!\n");
	/*for(int i = (numCells_)-1; i>=0; i--){
		if(numSpheresInCell[i]>=maxNumSpheresInCell){
			printf("Many (%3d) spheres in cell!\n", numSpheresInCell[i]);
		}
		numSpheresInCell[i] = std::min(maxNumSpheresInCell-1, (int)numSpheresInCell[i]);
	}*/
}

template <int dims, bool _3D_>
void EigenCalculator_CellCountCollider<dims,_3D_>::calcForces(){
	if(!ballResistance) return;
	C::calcForces();
	countSpheresPerCell();
	collideSpheresPerCell(false, NULL);
}

template <int dims, bool _3D_>
void EigenCalculator_CellCountCollider<dims,_3D_>::collideSpheresPerCell(bool numSpheresInCell_isSorted, int* cellIndices){
	bool tooManyColls = false;
	int i;
	//parallelFor
	for(int x = (C::numCells_)-1; x>=0; x--){
		///if(numSpheresInCell_isSorted) i = cellIndices[x]; else 
		i = x;
		//cycle through cell IDs
		
		//check collisions in cell:
		int num = numSpheresInCell[i],c,c2,tmp;
		if(num<2){
			///if(numSpheresInCell_isSorted) break;
			continue;
		}
		bool cont; //continue
		for(int id = 0, id2; id<num-1; id++){ //sphere id in cell
			for(id2 = id+1; id2<num; id2++){
				c = spheresInCell[i][id];
				c2 = spheresInCell[i][id2];
				//make shure that c<c2:
				if(c>c2){
					tmp = c;
					c = c2;
					c2 = tmp;
				}
				//check if spheres already have collided:
				cont = false;
				for(int a = numCollsPerSphere[c]-1; a>=0; a--){
					if(collsPerSphere[c][a]==c2){
						cont = true;
						break;
					}
				}
				if(cont) continue;
				//else list the higher id (c2) in the array of sphere with lower id (c):
				if(numCollsPerSphere[c]<maxNumCollsPerSphere){
					collsPerSphere[c][numCollsPerSphere[c]++] = c2;
					//and check collision:
					P::collideBalls(c,c2);
				}else{
					//printf("Too many spheres (%5d) colliding with sphere %5d!\n", numCollsPerSphere[c], c);
					tooManyColls = true;
				}
			}
		}
	}
	if(tooManyColls) printf("Too many spheres colliding with one sphere!\n");
}

template class EigenCalculator_CellCountCollider<2,false>;
template class EigenCalculator_CellCountCollider<3,true>;

