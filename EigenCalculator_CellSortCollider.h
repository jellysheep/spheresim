
#ifndef _EIGEN_CALCULATOR_CELL_SORT_COLLIDER_H_
#define _EIGEN_CALCULATOR_CELL_SORT_COLLIDER_H_

#include "EigenCalculator_Force.h"
#include "EigenCalculator_CellForce.h"
#include "EigenCalculator_PairCollider.h"
#include <cstdio>

template <int dims, bool _3D_>
class EigenCalculator_Engine;

template <int dims, bool _3D_>
class EigenCalculator_CellSortCollider : 
	protected EigenCalculator_CellForce<dims,_3D_>,
	public EigenCalculator_PairCollider<dims,_3D_>{
protected:
	
	void sortSpheresByCells();
	void checkCollision(int i, int x, int y, int z, bool sameCell=false);
	
	int *cellOfSphere;
	Pos *posCell; //spheres sorted by cell ID
	int *firstSphereInCell; //first sphere in each cell, or -1
	
public:
	EigenCalculator_CellSortCollider(EigenCalculator_Engine<dims,_3D_>* c):
		EigenCalculator_Force<dims,_3D_>(c),
		EigenCalculator_CellForce<dims,_3D_>(c),
		EigenCalculator_PairCollider<dims,_3D_>(c){
		
		///cellSorting
		
		firstSphereInCell = new int[C::numCells_];
		memset(firstSphereInCell, 0, C::numCells_);
		posCell = new Pos[spheresCount];
		cellOfSphere = new int[spheresCount];

		///cellSorting
		for(int i = 0; i<spheresCount; i++){
			posCell[i].sphereAtPos = i;
			posCell[i].posOfSphere = i;
			
		}
	}
	
	virtual void calcForces();
	
	virtual void spheresCountChanged(int c);
};

#endif  /*_EIGEN_CALCULATOR_CELL_SORT_COLLIDER_H_*/

