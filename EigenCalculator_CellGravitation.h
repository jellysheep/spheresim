
#ifndef _EIGEN_CALCULATOR_CELL_GRAVITATION_H_
#define _EIGEN_CALCULATOR_CELL_GRAVITATION_H_

#include "EigenCalculator_Force.h"

//Implementation of the Barnes-Hut simulation

template <int dims, bool _3D_>
class EigenCalculator_Engine;

template <int dims, bool _3D_>
class EigenCalculator_CellForce;

template <int dims, bool _3D_>
class EigenCalculator_CellGravitation : public EigenCalculator_CellForce<dims,_3D_>{
protected:
	void calcGravitation(int i, int j, bool bothWays=true);
	
	int* numSpheresInCell;
	int** spheresInCell;
	void countSpheresPerCell();
	
	eVector* massVecSumOfCell; // sum of  m_i * r_i
	scalar* massSumOfCell; // sum of m_i
	
	eVector* centerOfMassOfCell;
	
	int numAllCells;
	
	void buildApproximatingCells();
	
	static const int maxNumPairwiseCells;
	int** pairwiseCells;
	int* numPairwiseCells;
	
	static const int maxNumApproximatingCells;
	int** approximatingCells; //  cells wich can be approximated to calculate ...
	int* numApproximatingCells; //... the force on spheres in a specific cell
	
	void addApproximatingCells(int curCellID, int testCellID);
	eVector *cellPos, *cellSize;
	scalar *cellHalfDiagLength;
	static const scalar maxTheta;
	
	void buildOwnCurveIndices();
	
	void paintCell(int i);
	
public:
	EigenCalculator_CellGravitation(EigenCalculator_Engine<dims,_3D_>* c);
	
	virtual void calcForces();
	
	void paintGL();
};

#endif  /*_EIGEN_CALCULATOR_CELL_GRAVITATION_H_*/


