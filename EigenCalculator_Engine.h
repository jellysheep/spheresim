#ifdef ENGINE_CPP

#ifndef _EIGEN_CALCULATOR_ENGINE_H_
#define _EIGEN_CALCULATOR_ENGINE_H_

#define NDEBUG
#include <Eigen/Dense>
using namespace Eigen;

#include <QThread>

#include "Spheres.h"
#include "Calculator.h"
#include "EigenCalculator_QObject.h"

#define useSSE 1

#if useSSE
	//Vector4d and Vector4f are vectorized
	//typedef Eigen::Matrix<scalar, 4, 1> eVector;
	#if _double_
		typedef Vector4d eVector;
	#else
		typedef Vector4f eVector;
	#endif
#else
	//typedef Eigen::Matrix<scalar, 3, 1> eVector;
	#if _double_
		typedef Vector3d eVector;
	#else
		typedef Vector3f eVector;
	#endif
#endif

struct Pos {
	int posOfSphere, sphereAtPos;
};

class EigenCalculator_Engine: public EigenCalculator_QObject {

protected:
	
	void save();
		
	eVector** renderBuffer;
	Sphere* spheres;
	eVector *spheresOldPos, *spheresPos, *spheresSpeed, *spheresForce;
	scalar** both_r;
	int gridSteps;
	scalar gridWidth;
	int **gridIndex;
	
	void doStep();
	bool saveFrame();
	
	Sphere c;
	
	scalar timeInterval;
	
	void sphereCountChanged_subclass(int i);
	void maxSphereCountChanged_subclass(int i);
	
	void initSphere(int i);
	
	scalar _E;
	
	void calcWallResistance();
	void calcBallResistance();
	void sumUpForces();
	
	void collideBalls(int i, int j);
	
	Pos *posX, *posY, *posZ; //positions of spheres sorted by axes
	void sort(Pos* p, int dim); //sort the spheres
	void calcSortedBallResistance();
	
	int numCells;
	const static int maxCellsPerAxis = 27;
	int *cellOfSphere;
	Pos *posCell; //spheres sorted by cell ID
	int *firstSphereInCell; //first sphere in each cell, or -1
	void sortSpheresByCells();
	void calcCellSortedBallResistance();
	void checkCollision(int i, int x, int y, int z, bool sameCell=false);
	inline int calcCellID(int x, int y, int z=0);
	
	void loadConfig(const char* file);
	
	bool isFixed(int i);
	
	const static int rowsPerStep = 3, curveSteps = 3; //Peano-Kurve, RowColumn-Order
	//const static int rowsPerStep = 2, curveSteps = 5; //Z-Order, Hilbert-Kurve
	int* curveIndices;
	int indexCounter;
	void buildCurveIndices_RowColumn();
	void buildCurveIndices_zOrder();
	void buildCurveIndices_Peano();
	void buildCurveIndices_Hilbert();
	void buildPeanoCurve(int x, int y, int z, int step, int direction);
	
	int pow_int(int a, unsigned int b){
		return (b>=2 ? a*a*pow(a, b-2) : (b == 1 ? a : 1));
	}
	
	static const int maxSpheresInCell = 30;
	char* spheresPerCell;
	int** spheresInCell;

public:
	EigenCalculator_Engine();
	
	Sphere* getSphere(int i);
	Sphere* getDirectSphere(int i);
	
	void fpsChanged(scalar timeInterval);
	
	scalar getTemperature();
	
	void paintGL(bool b);
	
	void boxSizeChanged();
	void gravityChanged();
	void updateG();
	void updateAirResistance();
	void updateWallResistance();
	void updateEModul();
	void updatePoisson();
	void updateElasticity();
	void updateGridSize();
	
	void loadConfig();
};

extern void start(EigenCalculator_Engine* clTimer);

#endif  /* _EIGEN_CALCULATOR_ENGINE_H_ */

#endif
