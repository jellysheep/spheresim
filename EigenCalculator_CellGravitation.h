
#ifndef _EIGEN_CALCULATOR_CELL_GRAVITATION_H_
#define _EIGEN_CALCULATOR_CELL_GRAVITATION_H_

#include "EigenCalculator_Force.h"

template <int dims, bool _3D_>
class EigenCalculator_Engine;

template <int dims, bool _3D_>
class EigenCalculator_CellCountCollider;

template <int dims, bool _3D_>
class EigenCalculator_CellGravitation : public virtual EigenCalculator_Force<dims,_3D_>{
protected:
	void calcGravitation(int i, int j);
	EigenCalculator_CellCountCollider<dims,_3D_>* cellCountCollider;
	
	int* numSpheresInCell;
	int** spheresInCell;
	int *cellOfSphere;
	
	eVector* massVecSumOfCell; // sum of  m_i * r_i
	scalar* massSumOfCell; // sum of m_i
	
	eVector* centerOfMassOfCell;
	
	int numCells_, numAllCells;
	
public:
	EigenCalculator_CellGravitation(EigenCalculator_Engine<dims,_3D_>* c, 
		EigenCalculator_CellCountCollider<dims,_3D_>* ccc);
	
	virtual void calcForces();
};

#endif  /*_EIGEN_CALCULATOR_CELL_GRAVITATION_H_*/


