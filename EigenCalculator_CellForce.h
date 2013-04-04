

#ifndef _EIGEN_CALCULATOR_CELL_FORCE_H_
#define _EIGEN_CALCULATOR_CELL_FORCE_H_

#include "EigenCalculator_Force.h"
#include <cmath>
#include <cstdio>

template <int dims, bool _3D_>
class EigenCalculator_Engine;

#define C EigenCalculator_CellForce<dims,_3D_>

template <int dims, bool _3D_>
class EigenCalculator_CellForce : public virtual EigenCalculator_Force<dims,_3D_>{
protected:
	
	int* curveIndices;
	int indexCounter;
	virtual void buildCurveIndices();
	void buildCurveIndices_RowColumn();
	void buildCurveIndices_zOrder();
	void buildCurveIndices_Peano();
	void buildCurveIndices_Hilbert();
	void buildPeanoCurve(int x, int y, int z, int step, int direction);
	
	int calcCellID(int x, int y, int z=0);
	
	int *cellOfSphere;
	Pos *posCell; //spheres sorted by cell ID
	int *firstSphereInCell; //first sphere in each cell, or -1
	
	int gridSteps;
	scalar gridWidth;
	int **gridIndex; //?
	int numCellsPerAxis, numCells_;
	void updateGridSize();
	
public:
	EigenCalculator_CellForce(EigenCalculator_Engine<dims,_3D_>* c);
	
	virtual void calcForces(){
		F::calc->setGridWith(gridWidth);
		F::calcForces();
		for(int i = 0; i<spheresCount; i++){
			gridIndex[i][0] = F::spheresPos[i](0)/gridWidth;
			gridIndex[i][1] = F::spheresPos[i](1)/gridWidth;
			if(_3D_){
				gridIndex[i][2] = F::spheresPos[i](2)/gridWidth;
			}
		}
	};
};

#endif  /*_EIGEN_CALCULATOR_CELL_FORCE_H_*/
