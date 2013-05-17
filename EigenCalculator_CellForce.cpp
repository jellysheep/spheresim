

#include "EigenCalculator_CellForce.h"
#include <cmath>
#include <iostream>
#include <bitset>

//Hilbert-Kurve:
//rotate/flip a quadrant appropriately
void rot(int n, int *x, int *y, int rx, int ry) {
    if (ry == 0) {
        if (rx == 1) {
            *x = n-1 - *x;
            *y = n-1 - *y;
        }
 
        //Swap x and y
        int t  = *x;
        *x = *y;
        *y = t;
    }
}
//convert (x,y) to d
int xy2d_Hilbert (int n, int x, int y) {
    int rx, ry, s, d=0;
    for (s=n/2; s>0; s/=2) {
        rx = (x & s) > 0;
        ry = (y & s) > 0;
        d += s * s * ((3 * rx) ^ ry);
        rot(s, &x, &y, rx, ry);
    }
    return d;
}
template <int dims, bool _3D_>
void EigenCalculator_CellForce<dims,_3D_>::buildCurveIndices_Hilbert(){
	for(int y = 0; y<C::numCellsPerAxis; y++){
		for(int x = 0; x<C::numCellsPerAxis; x++){
			if(_3D_){
				for(int z = 0; z<C::numCellsPerAxis; z++){
					curveIndices[x+C::numCellsPerAxis*y + C::numCellsPerAxis*C::numCellsPerAxis*z] = xy2d_Hilbert(C::numCellsPerAxis, x, y) + C::numCellsPerAxis*C::numCellsPerAxis*z;
				}
			}else{
				curveIndices[x+C::numCellsPerAxis*y] = xy2d_Hilbert(C::numCellsPerAxis, x, y);
			}
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_CellForce<dims,_3D_>::buildCurveIndices_RowColumn(){
	for(int y = 0; y<C::numCellsPerAxis; y++){
		for(int x = 0; x<C::numCellsPerAxis; x++){
			if(_3D_){
				for(int z = 0; z<C::numCellsPerAxis; z++){
					curveIndices[x+C::numCellsPerAxis*y + C::numCellsPerAxis*C::numCellsPerAxis*z] = x+C::numCellsPerAxis*y + C::numCellsPerAxis*C::numCellsPerAxis*z;
				}
			}else{
				curveIndices[x+C::numCellsPerAxis*y] = x+C::numCellsPerAxis*y;
			}
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_CellForce<dims,_3D_>::buildCurveIndices_zOrder(){
	for(int y = 0; y<C::numCellsPerAxis; y++){
		for(int x = 0; x<C::numCellsPerAxis; x++){
			if(_3D_){
				for(int z = 0; z<C::numCellsPerAxis; z++){
					curveIndices[x+C::numCellsPerAxis*y + C::numCellsPerAxis*C::numCellsPerAxis*z] = calcZOrder(x,y,z);
					//curveIndices[x+C::numCellsPerAxis*y + C::numCellsPerAxis*C::numCellsPerAxis*z] = calcZOrder(x,y) + C::numCellsPerAxis*C::numCellsPerAxis*z;
				}
			}else{
				curveIndices[x+C::numCellsPerAxis*y] = calcZOrder(x,y);
			}
		}
	}
}

template <int dims, bool _3D_>
void EigenCalculator_CellForce<dims,_3D_>::buildCurveIndices_Peano(){
	indexCounter = 0;
	buildPeanoCurve(0, 0, 0, 0, 0);
}

template <int dims, bool _3D_>
void EigenCalculator_CellForce<dims,_3D_>::buildPeanoCurve(int x, int y, int z, int step, int direction){
	// direction: 
	// 0 = /^
	// 1 = ^\ 
	
	if(step == curveSteps){
		//highest recursion depth
		curveIndices[x+C::numCellsPerAxis*y+C::numCellsPerAxis*C::numCellsPerAxis*z] = indexCounter++;
	}else{
		x *= 3;
		y *= 3;
		z *= 3;
		step++;
		if(_3D_){
			///TODO
		}else{
			if(direction == 0){
				buildPeanoCurve(x+0, y+0, z, step, 0+direction);
				buildPeanoCurve(x+0, y+1, z, step, 1-direction);
				buildPeanoCurve(x+0, y+2, z, step, 0+direction);
				buildPeanoCurve(x+1, y+2, z, step, 1-direction);
				buildPeanoCurve(x+1, y+1, z, step, 0+direction);
				buildPeanoCurve(x+1, y+0, z, step, 1-direction);
				buildPeanoCurve(x+2, y+0, z, step, 0+direction);
				buildPeanoCurve(x+2, y+1, z, step, 1-direction);
				buildPeanoCurve(x+2, y+2, z, step, 0+direction);
			}else if(direction == 1){
				buildPeanoCurve(x+2, y+0, z, step, 0+direction);
				buildPeanoCurve(x+2, y+1, z, step, 1-direction);
				buildPeanoCurve(x+2, y+2, z, step, 0+direction);
				buildPeanoCurve(x+1, y+2, z, step, 1-direction);
				buildPeanoCurve(x+1, y+1, z, step, 0+direction);
				buildPeanoCurve(x+1, y+0, z, step, 1-direction);
				buildPeanoCurve(x+0, y+0, z, step, 0+direction);
				buildPeanoCurve(x+0, y+1, z, step, 1-direction);
				buildPeanoCurve(x+0, y+2, z, step, 0+direction);
			}
		}
	}
}



template <int dims, bool _3D_>
int EigenCalculator_CellForce<dims,_3D_>::calcZOrder(int xPos, int yPos)
{
    static const int MASKS[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
    static const int SHIFTS[] = {1, 2, 4, 8};

    int x = xPos;  // Interleave lower 16 bits of x and y, so the bits of x
    int y = yPos;  // are in the even positions and bits from y in the odd;

    x = (x | (x << SHIFTS[3])) & MASKS[3];
    x = (x | (x << SHIFTS[2])) & MASKS[2];
    x = (x | (x << SHIFTS[1])) & MASKS[1];
    x = (x | (x << SHIFTS[0])) & MASKS[0];

    y = (y | (y << SHIFTS[3])) & MASKS[3];
    y = (y | (y << SHIFTS[2])) & MASKS[2];
    y = (y | (y << SHIFTS[1])) & MASKS[1];
    y = (y | (y << SHIFTS[0])) & MASKS[0];

    const int result = x | (y << 1);
    return result;
}

template <int dims, bool _3D_>
int EigenCalculator_CellForce<dims,_3D_>::calcZOrder(int xPos, int yPos, int zPos)
{
	static const int sizeMask = 0b1111111111; //max. 10 bit per int => 30 bit result
    static const int MASKS[] = {0x49249249, 0xC30C30C3, 0x0F00F00F, 0xFF0000FF};
    static const int SHIFTS[] = {2, 4, 8, 16};

    int x = xPos & sizeMask;
    int y = yPos & sizeMask;
    int z = zPos & sizeMask;

    x = (x | (x << SHIFTS[3])) & MASKS[3];
    x = (x | (x << SHIFTS[2])) & MASKS[2];
    x = (x | (x << SHIFTS[1])) & MASKS[1];
    x = (x | (x << SHIFTS[0])) & MASKS[0];

    y = (y | (y << SHIFTS[3])) & MASKS[3];
    y = (y | (y << SHIFTS[2])) & MASKS[2];
    y = (y | (y << SHIFTS[1])) & MASKS[1];
    y = (y | (y << SHIFTS[0])) & MASKS[0];

    z = (z | (z << SHIFTS[3])) & MASKS[3];
    z = (z | (z << SHIFTS[2])) & MASKS[2];
    z = (z | (z << SHIFTS[1])) & MASKS[1];
    z = (z | (z << SHIFTS[0])) & MASKS[0];

    const int result = x | (y << 1) | (z << 2);
    return result;
}

template <int dims, bool _3D_>
void EigenCalculator_CellForce<dims,_3D_>::updateGridSize(){
	scalar gridWidth_;
	if(_3D_){
		gridWidth_ = (curUnit.size*std::max(boxSize.s0, std::max(boxSize.s1, boxSize.s2)))/numCellsPerAxis;
	}else{
		gridWidth_ = (curUnit.size*std::max(boxSize.s0, boxSize.s1))/numCellsPerAxis;
	}
	//gridWidth_ = std::max(curUnit.size*sphereSize.s1, gridWidth_);
	gridWidth = gridWidth_;
	printf("gridWidth: %5f \n", gridWidth);
	if(_3D_){
		gridSteps = (int)((curUnit.size*std::max(boxSize.s0, std::max(boxSize.s1, boxSize.s2)))/gridWidth);
	}else{
		gridSteps = (int)((curUnit.size*std::max(boxSize.s0, boxSize.s1))/gridWidth);
	}
	printf("Grid steps: %5d\n", gridSteps);
}


template <int dims, bool _3D_>
void EigenCalculator_CellForce<dims,_3D_>::buildCurveIndices(){
	//buildCurveIndices_RowColumn();
	buildCurveIndices_zOrder();
	//buildCurveIndices_Peano();
	//buildCurveIndices_Hilbert();
}


template <int dims, bool _3D_>
EigenCalculator_CellForce<dims,_3D_>::EigenCalculator_CellForce(EigenCalculator_Engine<dims,_3D_>* c, bool buildCurve):EigenCalculator_Force<dims,_3D_>(c){
	numCellsPerAxis = std::min(pow_int(rowsPerStep, curveSteps), maxCellsPerAxis);
	numCells_ = numCellsPerAxis*numCellsPerAxis*(_3D_?numCellsPerAxis:1);
	updateGridSize();
	gridIndex = new int*[spheresCount];
	
	
	curveIndices = new int[C::numCells_];
	if(buildCurve){
		buildCurveIndices();
	}
	/*
	for(int y = C::numCellsPerAxis-1; y>=0; y--){
		for(int x = 0; x<C::numCellsPerAxis; x++){
			printf("%3d ", curveIndices[x+C::numCellsPerAxis*y]);
		}
		printf("\n");
	}//*/
	
	for(int i = 0; i<spheresCount; i++){
		C::gridIndex[i] = new int[dims];

	}
}

template <int dims, bool _3D_>
void EigenCalculator_CellForce<dims,_3D_>::spheresCountChanged(int c){
	F::spheresCountChanged(c);
	printf("CellForce: SpheresCount changes from %5d to %5d\n", spheresCount, c);
	gridIndex = newCopy(gridIndex, spheresCount, c);
	for(int i = spheresCount; i<c; i++){
		gridIndex[i] = new int[dims];
	}
}

template class EigenCalculator_CellForce<2,false>;
template class EigenCalculator_CellForce<3,true>;
