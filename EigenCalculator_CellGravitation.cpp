
#include "EigenCalculator_CellGravitation.h"
#include "EigenCalculator_Engine.h"
#include "EigenCalculator_CellForce.h"
#include <GL/glu.h>

//Implementation of the Barnes-Hut simulation

template <int dims, bool _3D_>
EigenCalculator_CellGravitation<dims,_3D_>::EigenCalculator_CellGravitation(
	EigenCalculator_Engine<dims,_3D_>* c):F(c),C(c,false){
	
	cellOfSphere = new int[spheresCount];
	
	numAllCells = C::numCells_*2;
	massVecSumOfCell = new eVector[numAllCells];
	massSumOfCell = new scalar[numAllCells];
	centerOfMassOfCell = new eVector[numAllCells];
	
	numSpheresInCell = new int[C::numCells_];
	spheresInCell = new int*[C::numCells_];
	for(int i = (C::numCells_)-1; i>=0; i--){
		spheresInCell[i] = new int[maxNumSpheresInCell];
	}
	
	buildApproximatingCells();
}

template <int dims, bool _3D_>
const int EigenCalculator_CellGravitation<dims,_3D_>::maxNumApproximatingCells = 500;

template <int dims, bool _3D_>
const int EigenCalculator_CellGravitation<dims,_3D_>::maxNumPairwiseCells = 500;

template <int dims, bool _3D_>
const scalar EigenCalculator_CellGravitation<dims,_3D_>::maxTheta = 1.5;

template <int dims, bool _3D_>
void EigenCalculator_CellGravitation<dims,_3D_>::buildOwnCurveIndices(){
	printf("building curve indices... \n");
	//z-Order, but also saves cell positions
	int x,y,z,ID;
	for(y = 0; y<C::numCellsPerAxis; y++){
		for(x = 0; x<C::numCellsPerAxis; x++){
			if(_3D_){
				for(z = 0; z<C::numCellsPerAxis; z++){
					ID = C::calcZOrder(x,y,z);
					C::curveIndices[x+C::numCellsPerAxis*y + C::numCellsPerAxis*C::numCellsPerAxis*z] = ID;
					ID += C::numCells_;
					cellPos[ID](0) = x*1.0/C::numCellsPerAxis;
					cellPos[ID](1) = y*1.0/C::numCellsPerAxis;
					cellPos[ID](2) = z*1.0/C::numCellsPerAxis;
					cellSize[ID](0) = 1.0/C::numCellsPerAxis;
					cellSize[ID](1) = 1.0/C::numCellsPerAxis;
					cellSize[ID](2) = 1.0/C::numCellsPerAxis;
				}
			}else{
				ID = C::calcZOrder(x,y);
				C::curveIndices[x+C::numCellsPerAxis*y] = ID;
				ID += C::numCells_;
				cellPos[ID](0) = x*1.0/C::numCellsPerAxis;
				cellPos[ID](1) = y*1.0/C::numCellsPerAxis;
				cellSize[ID](0) = 1.0/C::numCellsPerAxis;
				cellSize[ID](1) = 1.0/C::numCellsPerAxis;
			}
		}
	}
	printf("Curve indices successfully built!\n");
}

template <int dims, bool _3D_>
void EigenCalculator_CellGravitation<dims,_3D_>::buildApproximatingCells(){
	cellPos = new eVector[numAllCells];
	cellSize = new eVector[numAllCells];
	cellHalfDiagLength = new scalar[numAllCells];
	buildOwnCurveIndices();
	
	int i, j, dimension;
	//calculate pos. and size of cells:
	for(int i = numAllCells-1; i>=1; i--){
		if(i>=C::numCells_){
			//add half size to position:
			cellPos[i] += cellSize[i]/2;
		}
		//half length of cell diagonal:
		cellHalfDiagLength[i] = std::sqrt(cellSize[i](0)*cellSize[i](0)
			+cellSize[i](1)*cellSize[i](1)
			+(_3D_?(cellSize[i](2)*cellSize[i](2)):0))/2;
		
		//find out in which dimension parent cell is divided:
		dimension = 0;
		for(j = i; j>1; j/=2){
			dimension++;
		}
		dimension %= dims;
		//set size of parent cell:
		//j is id of parent cell
		j = i>>1;
		//if second cell in parent cell:
		if(i%2 == 1){
			cellSize[j](0) = cellSize[i](0);
			cellSize[j](1) = cellSize[i](1);
			if(_3D_){
				cellSize[j](2) = cellSize[i](2);
			}
		}else{
			cellSize[j](dimension) += cellSize[i](dimension);
			cellPos[j] = cellPos[i];
			cellPos[j](dimension) += cellSize[i](dimension)/2;
		}
	}
	printf("Cell size of cell 1 is: %3f %3f %3f\n", cellSize[1](0), cellSize[1](1), (_3D_?cellSize[1](2):0));
	printf("Cell size of cell 1025 is: %3f %3f %3f\n", cellSize[1025](0), cellSize[1025](1), (_3D_?cellSize[1025](2):0));
	printf("Cell pos of cell 1025 is: %3f %3f %3f\n", cellPos[1025](0), cellPos[1025](1), (_3D_?cellPos[1025](2):0));
	
	numPairwiseCells = new int[C::numCells_];
	pairwiseCells = new int*[C::numCells_];
	
	numApproximatingCells = new int[C::numCells_];
	approximatingCells = new int*[C::numCells_];
	
	for(i = 0; i<C::numCells_; i++){
		numPairwiseCells[i] = 0;
		pairwiseCells[i] = new int[maxNumPairwiseCells];
		
		numApproximatingCells[i] = 0;
		approximatingCells[i] = new int[maxNumApproximatingCells];
		
		addApproximatingCells(C::numCells_+i, 1);
	}
}

template <int dims, bool _3D_>
void EigenCalculator_CellGravitation<dims,_3D_>::addApproximatingCells(int curCellID, int testCellID){
	if(curCellID == testCellID) return;
	
	if(curCellID>=numAllCells || testCellID>=numAllCells){
		printf("Error: sphere indices too big!\n");
		return;
	}
	
	int cid = curCellID-C::numCells_;
	scalar d = std::max(cellSize[testCellID](0), cellSize[testCellID](1));
	if(_3D_) d = std::max(d, cellSize[testCellID](2));
	scalar d2 = std::max(cellSize[curCellID](0), cellSize[curCellID](1));
	if(_3D_) d2 = std::max(d2, cellSize[curCellID](2));
	d = std::max(d,d2);
	//minimal distance between the two cells:
	scalar r = std::max((cellPos[curCellID]-cellPos[testCellID]).norm()
		- cellHalfDiagLength[curCellID] - cellHalfDiagLength[testCellID], (scalar)0.001);
	scalar theta = d/r;
	if(theta<maxTheta){
		if(numApproximatingCells[cid]<maxNumApproximatingCells){
			approximatingCells[cid][numApproximatingCells[cid]++] = testCellID;
		}else{
			printf("fatal error: too many approximating cells!!\n");
		}
	}else{
		if(testCellID>=C::numCells_){
			//smallest cell: calculate forces pairwise.
			if(numPairwiseCells[cid]<maxNumPairwiseCells){
				pairwiseCells[cid][numPairwiseCells[cid]++] = testCellID;
			}else{
				printf("fatal error: too many pairwise cells!!\n");
			}
		}else{
			//test subcells
			addApproximatingCells(curCellID, testCellID<<1);
			addApproximatingCells(curCellID, (testCellID<<1)+1);
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_CellGravitation<dims,_3D_>::countSpheresPerCell(){
	//no spheres in cells:
	for(int i = (C::numCells_)-1; i>=0; i--){
		numSpheresInCell[i] = 0;
	}
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
	//printf("%3d\n", x++);
	static const scalar fact = 1.0;
	bool tooManySpheres = false;
	parallelFor
	for(int i = 0; i<spheresCount; i++){
		int cellID = cellOfSphere[i];
		if(numSpheresInCell[cellID]<maxNumSpheresInCell){
			spheresInCell[cellID][numSpheresInCell[cellID]++] = i;
		}else{
			tooManySpheres = true;
		}
	}
	if(tooManySpheres) printf("Too many spheres in one cell!\n");
}

template <int dims, bool _3D_>
void EigenCalculator_CellGravitation<dims,_3D_>::calcGravitation(int i, int j, bool bothWays){
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
	
	if(bothWays) F::spheresForce[j] -= force;
}
	
template <int dims, bool _3D_>
void EigenCalculator_CellGravitation<dims,_3D_>::calcForces(){
	C::calcForces();
	if(G_fact!=0){
		countSpheresPerCell();
		
		int i;
		///reset sums:
		for(i = 1; i<numAllCells; i++){
			massVecSumOfCell[i].setZero();
			massSumOfCell[i] = 0;
		}
		///sum up masses in smallest cells:
		for(i = 0; i<spheresCount; i++){
			massVecSumOfCell[C::numCells_+cellOfSphere[i]] += F::spheres[i].mass*F::spheresPos[i];
			massSumOfCell[C::numCells_+cellOfSphere[i]] += F::spheres[i].mass;
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
		int cid, cid2, sid, sid2, j;
		eVector d_pos, d_n, force;
		scalar d;
		for(cid = 0; cid<C::numCells_; cid++){
			///pairwise gravitation to near cells:
			for(j = 0; j<numPairwiseCells[cid]; j++){
				cid2 = pairwiseCells[cid][j]-C::numCells_;
				for(sid = 0; sid<numSpheresInCell[cid]; sid++){
					for(sid2 = 0; sid2<numSpheresInCell[cid2]; sid2++){
						calcGravitation(spheresInCell[cid][sid],spheresInCell[cid2][sid2], false);
					}
				}
			}
			///gravitation to center of mass of far away cells:
			for(j = 0; j<numApproximatingCells[cid]; j++){
				//printf("_approximatingCells[%5d][%5d] = %5d;\n", cid, j, approximatingCells[cid][j]);
				cid2 = approximatingCells[cid][j];
				for(sid = 0; sid<numSpheresInCell[cid]; sid++){
					sid2 = spheresInCell[cid][sid];
					d_pos = centerOfMassOfCell[cid2]-F::spheresPos[sid2];
					
					d = d_pos.norm();
					//d = std::max(d,(scalar)0.00005f);
					d = std::max(d,(scalar)F::spheres[sid2].size/5);
					
					d_n = d_pos/d;
					// bzw. normalize(d_pos);
					/// Gravitation:
					force = G*G_fact*F::spheres[sid2].mass*massSumOfCell[cid2]/pow(d,2) *d_n;
					F::spheresForce[sid2] += force;
				}
			}
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_CellGravitation<dims,_3D_>::paintCell(int i){
	glBegin(GL_LINE_LOOP);
	glVertex2f(cellPos[i](0)-cellSize[i](0)/2, cellPos[i](1)-cellSize[i](1)/2);
	glVertex2f(cellPos[i](0)+cellSize[i](0)/2, cellPos[i](1)-cellSize[i](1)/2);
	glVertex2f(cellPos[i](0)+cellSize[i](0)/2, cellPos[i](1)+cellSize[i](1)/2);
	glVertex2f(cellPos[i](0)-cellSize[i](0)/2, cellPos[i](1)+cellSize[i](1)/2);
	glEnd();
}
	
template <int dims, bool _3D_>
void EigenCalculator_CellGravitation<dims,_3D_>::paintGL(){
	
	if(spheresCount>0){
		glPushMatrix();		
		glScalef(curUnit.size*boxSize.s0, curUnit.size*boxSize.s1, curUnit.size*boxSize.s2);
		
		int sid = 0;
		Sphere* s = F::calc->getSphere(sid);
		if(s!=NULL){
			int cid;
			if(use3D){
				cid = C::calcCellID(s->pos.s[0]/C::gridWidth, s->pos.s[1]/C::gridWidth, s->pos.s[2]/C::gridWidth);
			}else{
				cid = C::calcCellID(s->pos.s[0]/C::gridWidth, s->pos.s[1]/C::gridWidth);
			}
			//int cid = cellOfSphere[sid];
			cid = std::min(cid, C::numCells_-1);
			cid = std::max(cid, 0);
			
			glColor3d(0.0, 0.0, 1.0);
			paintCell(C::numCells_+cid);
			//~ printf("pairwise cells: \n");
			glColor3d(1.0, 0.0, 0.0);
			for(int i = 0; i<numPairwiseCells[cid]; i++){
				//~ printf("Cell size of cell %4d is: %3f %3f %3f\n", pairwiseCells[cid][i], cellSize[pairwiseCells[cid][i]](0), cellSize[pairwiseCells[cid][i]](1), (_3D_?cellSize[pairwiseCells[cid][i]](2):0));
				//~ printf("Cell pos. of cell %4d is: %3f %3f %3f\n", pairwiseCells[cid][i], cellPos[pairwiseCells[cid][i]](0), cellPos[pairwiseCells[cid][i]](1), (_3D_?cellPos[pairwiseCells[cid][i]](2):0));
				paintCell(pairwiseCells[cid][i]);
			}
			//~ printf("approximating cells: \n");
			glColor3d(0.0, 1.0, 0.0);
			for(int i = 0; i<numApproximatingCells[cid]; i++){
				//~ printf("Cell size of cell %4d is: %3f %3f %3f\n", approximatingCells[cid][i], cellSize[approximatingCells[cid][i]](0), cellSize[approximatingCells[cid][i]](1), (_3D_?cellSize[approximatingCells[cid][i]](2):0));
				//~ printf("Cell pos. of cell %4d is: %3f %3f %3f\n", approximatingCells[cid][i], cellPos[approximatingCells[cid][i]](0), cellPos[approximatingCells[cid][i]](1), (_3D_?cellPos[approximatingCells[cid][i]](2):0));
				paintCell(approximatingCells[cid][i]);
			}
			//printf("Cell pos. of sphere %4d is: %3d %3d %3d\n", sid, C::gridIndex[sid][0], C::gridIndex[sid][1], (_3D_?C::gridIndex[sid][2]:0));
			
			glPopMatrix();
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_CellGravitation<dims,_3D_>::spheresCountChanged(int c){
	C::spheresCountChanged(c);
	cellOfSphere = newCopy(cellOfSphere, spheresCount, c);
}

template class EigenCalculator_CellGravitation<2,false>;
template class EigenCalculator_CellGravitation<3,true>;

