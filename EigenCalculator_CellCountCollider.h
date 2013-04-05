
#ifndef _EIGEN_CALCULATOR_CELL_COUNT_COLLIDER_H_
#define _EIGEN_CALCULATOR_CELL_COUNT_COLLIDER_H_

#include "EigenCalculator_Force.h"
#include "EigenCalculator_CellForce.h"
#include "EigenCalculator_PairCollider.h"

template <int dims, bool _3D_>
class EigenCalculator_Engine;

template <int dims, bool _3D_>
class EigenCalculator_CellCountCollider : 
	protected EigenCalculator_CellForce<dims,_3D_>,
	public EigenCalculator_PairCollider<dims,_3D_>{
	
protected:
	
	int* numSpheresInCell;
	int** spheresInCell;
	void countSpheresPerCell();
	int* numCollsPerSphere;
	int** collsPerSphere;
	
	void collideSpheresPerCell(bool numSpheresInCell_isSorted, int* cellIndices);
	void collideSpheresPerCell();

public:
	EigenCalculator_CellCountCollider(EigenCalculator_Engine<dims,_3D_>* c):
		EigenCalculator_Force<dims,_3D_>(c),
		EigenCalculator_CellForce<dims,_3D_>(c),
		EigenCalculator_PairCollider<dims,_3D_>(c){
		numSpheresInCell = new int[C::numCells_];
		spheresInCell = new int*[C::numCells_];
		for(int i = (C::numCells_)-1; i>=0; i--){
			spheresInCell[i] = new int[maxNumSpheresInCell];
		}
		
		numCollsPerSphere = new int[spheresCount];
		collsPerSphere = new int*[spheresCount];
		for(int i = 0; i<spheresCount; i++){
			collsPerSphere[i] = new int[maxNumCollsPerSphere];
		}
	}
	
	virtual void calcForces();
};

#endif  /*_EIGEN_CALCULATOR_CELL_COUNT_COLLIDER_H_*/


