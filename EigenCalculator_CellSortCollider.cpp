
#include "EigenCalculator_CellSortCollider.h"
#include <cmath>

template <int dims, bool _3D_>
void EigenCalculator_CellSortCollider<dims,_3D_>::sortSpheresByCells(){
	if(_3D_){
		for(int i = 0; i<spheresCount; i++){
			//calculate cell ID
			cellOfSphere[i] = C::calcCellID(C::gridIndex[i][0], C::gridIndex[i][1], C::gridIndex[i][2]);
		}
	}else{
		for(int i = 0; i<spheresCount; i++){
			//calculate cell ID
			cellOfSphere[i] = C::calcCellID(C::gridIndex[i][0], C::gridIndex[i][1]);
			//printf
		}
	}
	
	int temp;
	int j, cid; //sphere ID
	for (int i=1; i < spheresCount; i++){
		cid = posCell[i].sphereAtPos;
		temp = cellOfSphere[cid];
		j = i-1;

		//while (j >= 0 && spheresPos[p[j].sphereAtPos](dim) > temp){
		while (j >= 0 && cellOfSphere[posCell[j].sphereAtPos] > temp){
			posCell[j+1].sphereAtPos = posCell[j].sphereAtPos;
			j--;
		}

		posCell[j+1].sphereAtPos = cid;
	}
	
	//save array index of each sphere
	for(int i = 0; i<spheresCount; i++){
		posCell[posCell[i].sphereAtPos].posOfSphere = i;
	}
	
	/*
	//set first sphere of each cell to -1
	for(int i = curUnit.size*boxSize.s0/gridWidth + 1; i>=0; i--){
		for(j = curUnit.size*boxSize.s1/gridWidth + 1; j>=0; j--){
			if(_3D_){
				for(int k = curUnit.size*boxSize.s2/gridWidth; k>=0; k--){
					firstSphereInCell[i + C::numCellsPerAxis*j + C::numCellsPerAxis*C::numCellsPerAxis*k] = -1;
				}
			}else{
				firstSphereInCell[i + C::numCellsPerAxis*j] = -1;
			}
		}
	}// */
	
	//find posCell array position of first sphere of each cell
	for(int i = spheresCount-1; i>=0; i--){
		firstSphereInCell[cellOfSphere[posCell[i].sphereAtPos]] = i;
	}
	/*for(int i = 0; i<spheresCount; i++){
		printf("Sphere: %2d cellID: %2d first sphere in cell: %2d\n", posCell[i].sphereAtPos, cellOfSphere[posCell[i].sphereAtPos], posCell[firstSphereInCell[cellOfSphere[posCell[i].sphereAtPos]]].sphereAtPos);
	}// */
}

#define twiceCalcCollisions 1

template <int dims, bool _3D_>
void EigenCalculator_CellSortCollider<dims,_3D_>::checkCollision(int i, int x, int y, int z, bool sameCell){ 
	//sphere ID, cell position, bool if same cell (i.e. dx = dy = dz = 0)
	if(x<0 || y<0 || z<0 || x>=C::numCellsPerAxis || y>=C::numCellsPerAxis || z>=C::numCellsPerAxis){
		//if(x>=C::numCellsPerAxis || y>=C::numCellsPerAxis) printf("return because out of bounds\n");
		return;
	}
	int cellID = C::calcCellID(x,y,z);
	int j = firstSphereInCell[cellID]; //sphere 2 pos in array
	if(j < 0 || j>=spheresCount){
		//printf("empty cell: %2d (%2d)\n", cellID, j);
		return;
	}
	int c2 = posCell[j].sphereAtPos; //sphere 2 ID
	if(c2 < 0 || c2>=spheresCount) return;
	//printf("sphere 2 ID: %3d\n", c2);
	//if(c2>spheresCount) printf("cell: %2d sphere 2 ID: %2d cell of sphere before: %2d cell of next sphere: %2d\n", cellID, c2, cellOfSphere[posCell[j-1].sphereAtPos], cellOfSphere[posCell[j+1].sphereAtPos]);
	//printf("cell: %2d | %2d cell of sphere before: %2d cell of next sphere: %2d\n", cellID, cellOfSphere[posCell[j].sphereAtPos], cellOfSphere[posCell[j-1].sphereAtPos], cellOfSphere[posCell[j+1].sphereAtPos]);
	while(cellOfSphere[c2] == cellID){
		#if twiceCalcCollisions==0
			if(!(sameCell && spheresOldPos[i][0]>spheresOldPos[c2][0]))
		#endif
		{
			//if(spheresOldPos[i][0]>spheresOldPos[c2][0]) 
			if(i>c2) 
			P::collideBalls(i, c2);
		}
		j++;
		//if(j>=spheresCount) return;
		c2 = posCell[j].sphereAtPos;
		if(c2>=spheresCount || c2<0) return;
	}; //check collisions of all spheres in that cell
}

template <int dims, bool _3D_>
void EigenCalculator_CellSortCollider<dims,_3D_>::calcForces(){
	if(!ballResistance) return;
	C::calcForces();
	//QThread::msleep(50);
	sortSpheresByCells();
	
	/*
	for(int pid = 0; pid<spheresCount; pid++){ // pos. ID
		printf("%3d ", posX[pid].sphereAtPos);
	}
	printf("\n"); // */

	/// Kugeln kollidieren nur mit anderen, die in Zellen rechts, oben, vorne davon liegen.
	/// Dadurch werden doppelt berechnete Kollisionen vermieden.
	
	if(_3D_){
		parallelFor
		for(int i = 0; i<spheresCount; i++){ //sphere ID 1
			int x = C::gridIndex[i][0], y = C::gridIndex[i][1], z = C::gridIndex[i][2];
			// dz = -1
			// ___
			// ___
			// ___
			
			#if twiceCalcCollisions
				checkCollision(i, x+0, y+0, z-1);
				checkCollision(i, x+1, y+0, z-1);
				checkCollision(i, x-1, y+0, z-1);
				checkCollision(i, x+0, y+1, z-1);
				checkCollision(i, x+1, y+1, z-1);
				checkCollision(i, x-1, y+1, z-1);
				checkCollision(i, x+0, y-1, z-1);
				checkCollision(i, x+1, y-1, z-1);
				checkCollision(i, x-1, y-1, z-1);
			#endif
			
			// dz = 0
			// _XX
			// _0X
			// __X
			
			checkCollision(i, x+0, y+0, z+0, true);
			checkCollision(i, x+1, y+0, z+0);
			checkCollision(i, x+1, y+1, z+0);
			checkCollision(i, x+1, y+1, z+0);
			checkCollision(i, x+0, y+1, z+0);
			
			#if twiceCalcCollisions
				checkCollision(i, x+0, y-1, z+0);
				checkCollision(i, x-1, y+0, z+0);
				checkCollision(i, x-1, y+1, z+0);
				checkCollision(i, x-1, y-1, z+0);
			#endif
			
			// dz = 1
			// XXX
			// XXX
			// XXX
			
			checkCollision(i, x+0, y+0, z+1);
			checkCollision(i, x+1, y+0, z+1);
			checkCollision(i, x-1, y+0, z+1);
			checkCollision(i, x+0, y+1, z+1);
			checkCollision(i, x+1, y+1, z+1);
			checkCollision(i, x-1, y+1, z+1);
			checkCollision(i, x+0, y-1, z+1);
			checkCollision(i, x+1, y-1, z+1);
			checkCollision(i, x-1, y-1, z+1);
		}
	}else{
		parallelFor
		for(int i = 0; i<spheresCount; i++){ //sphere ID 1
			int x = C::gridIndex[i][0], y = C::gridIndex[i][1], z = 0;
			// _XX
			// _0X
			// __X
			checkCollision(i, x+0, y+0, z);
			checkCollision(i, x+1, y+0, z);
			checkCollision(i, x+1, y+1, z);
			checkCollision(i, x+1, y-1, z);
			checkCollision(i, x+0, y+1, z);
			
			#if twiceCalcCollisions
				checkCollision(i, x+0, y-1, z);
				checkCollision(i, x-1, y+0, z);
				checkCollision(i, x-1, y+1, z);
				checkCollision(i, x-1, y-1, z);
			#endif
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_CellSortCollider<dims,_3D_>::spheresCountChanged(int c){
	P::spheresCountChanged(c);
	C::spheresCountChanged(c);
	cellOfSphere = newCopy(cellOfSphere, spheresCount, c);
	
	//posCell:
	Pos* pos = newCopy(posCell, spheresCount, c);
	int sphereAtLastPos, posOfLastSphere;
	//if c<spheresCount:
	for(int i = spheresCount-1; i>=c; i--){
		sphereAtLastPos = posCell[i].sphereAtPos;
		posOfLastSphere = posCell[i].posOfSphere;
		pos[posOfLastSphere].sphereAtPos = sphereAtLastPos;
		pos[sphereAtLastPos].posOfSphere = posOfLastSphere;
	}
	//if c>spheresCount:
	for(int i = spheresCount; i<c; i++){
		pos[i].sphereAtPos = i;
		pos[i].posOfSphere = i;
	}
	posCell = pos;
}

template class EigenCalculator_CellSortCollider<2,false>;
template class EigenCalculator_CellSortCollider<3,true>;
